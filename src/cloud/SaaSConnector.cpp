#include "SaaSConnector.hpp"
#include "core/Logger.hpp"
#include "fleet/NodeRegistry.hpp"
#include "intelligence/GlobalThreatCache.hpp"
#include "intelligence/IocBroadcaster.hpp"
#include "ota/CanaryOrchestrator.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <filesystem>

namespace sentinel::nexus::cloud {

struct ParsedUrl {
    bool is_https;
    std::string host;
    uint16_t port;
    std::string path;
};

static ParsedUrl parse_url(const std::string& url) {
    ParsedUrl res{false, "127.0.0.1", 8000, "/api/v1"};
    std::string temp = url;

    if (temp.rfind("https://", 0) == 0) {
        res.is_https = true;
        res.port = 443;
        temp = temp.substr(8);
    } else if (temp.rfind("http://", 0) == 0) {
        res.is_https = false;
        res.port = 80;
        temp = temp.substr(7);
    }

    size_t slash_pos = temp.find('/');
    if (slash_pos != std::string::npos) {
        res.path = temp.substr(slash_pos);
        temp = temp.substr(0, slash_pos);
    }

    size_t colon_pos = temp.find(':');
    if (colon_pos != std::string::npos) {
        res.host = temp.substr(0, colon_pos);
        res.port = static_cast<uint16_t>(std::stoi(temp.substr(colon_pos + 1)));
    } else {
        res.host = temp;
    }

    return res;
}

static std::string extract_json_field(const std::string& body, const std::string& key) {
    size_t pos = body.find("\"" + key + "\"");
    if (pos == std::string::npos) return "";

    size_t colon = body.find(':', pos);
    if (colon == std::string::npos) return "";

    size_t quote_start = body.find('"', colon + 1);
    if (quote_start == std::string::npos) return "";

    size_t quote_end = body.find('"', quote_start + 1);
    if (quote_end == std::string::npos) return "";

    return body.substr(quote_start + 1, quote_end - quote_start - 1);
}

bool SaaSConnector::start(const SaaSConfig& config) {
    config_ = config;

    if (!config_.enabled) {
        NEXUS_LOG_INFO("SaaS Connector disabled. Operating in 100% Sovereign Air-Gapped mode.");
        return true;
    }

    running_.store(true);
    NEXUS_LOG_INFO("SaaS Connector booting. Cloud endpoint: " + config_.cloud_endpoint);

    // Try loading cached token from disk, otherwise authenticate with credentials
    if (!load_token_from_disk()) {
        if (!authenticate()) {
            NEXUS_LOG_WARN("Initial authentication failed with " + config_.auth_email + ". Background workers will retry.");
        }
    }

    sync_thread_ = std::jthread([this](std::stop_token st) { outbound_sync_worker(st); });
    threat_feed_thread_ = std::jthread([this](std::stop_token st) { inbound_threat_feed_worker(st); });
    command_thread_ = std::jthread([this](std::stop_token st) { remote_command_worker(st); });

    return true;
}

void SaaSConnector::stop() {
    running_.store(false);
    cloud_connected_.store(false);
}

bool SaaSConnector::authenticate() {
    std::lock_guard<std::recursive_mutex> lock(auth_mutex_);

    std::cout << "\n\033[35m[SaaSConnector:AUTH] >>> Initiating Cloud Authentication...\033[0m" << std::endl;

    ParsedUrl purl = parse_url(config_.cloud_endpoint);
    std::string auth_path = purl.path + "/auth/login";

    // JSON payload
    std::string json_body = "{\"email\":\"" + config_.auth_email + "\",\"password\":\"" + config_.auth_password + "\",\"username\":\"" + config_.auth_email + "\"}";

    std::string resp;
    bool ok = http_post_json("/auth/login", json_body, resp, false);

    // If FastAPI expects form-encoded, retry if 422
    if (!ok && resp.find("422") != std::string::npos) {
        std::cout << "\033[33m[SaaSConnector:AUTH] Retrying authentication with form-urlencoded...\033[0m" << std::endl;
        
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        struct hostent* server = gethostbyname(purl.host.c_str());
        sockaddr_in serv_addr{};
        serv_addr.sin_family = AF_INET;
        memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
        serv_addr.sin_port = htons(purl.port);

        struct timeval tv{3, 0};
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

        if (sock >= 0 && connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0) {
            std::string form_body = "username=" + config_.auth_email + "&password=" + config_.auth_password;
            std::ostringstream form_req;
            form_req << "POST " << auth_path << " HTTP/1.1\r\n"
                     << "Host: " << purl.host << ":" << purl.port << "\r\n"
                     << "Content-Type: application/x-www-form-urlencoded\r\n"
                     << "Content-Length: " << form_body.size() << "\r\n"
                     << "Connection: close\r\n\r\n"
                     << form_body;
            std::string f_str = form_req.str();
            send(sock, f_str.data(), f_str.size(), 0);
            resp.clear();
            char buffer[4096];
            ssize_t bytes;
            while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
                buffer[bytes] = '\0';
                resp.append(buffer, bytes);
            }
            close(sock);
        }
    }

    std::string token = extract_json_field(resp, "access_token");
    if (token.empty()) token = extract_json_field(resp, "token");

    if (!token.empty()) {
        jwt_token_ = token;
        cloud_connected_.store(true);
        save_token_to_disk(jwt_token_);
        std::cout << "\033[32m[SaaSConnector:AUTH] [+] Authentication SUCCESS! JWT Acquired: " 
                  << token.substr(0, 20) << "...\033[0m\n" << std::endl;
        return true;
    }

    std::cout << "\033[31m[SaaSConnector:AUTH] [-] Authentication FAILED! Check response above.\033[0m\n" << std::endl;
    return false;
}

bool SaaSConnector::ensure_authenticated() {
    std::lock_guard<std::recursive_mutex> lock(auth_mutex_);
    if (jwt_token_.empty()) {
        return authenticate();
    }
    return true;
}

bool SaaSConnector::save_token_to_disk(const std::string& token) {
    std::error_code ec;
    std::filesystem::path path(config_.token_storage_path);
    std::filesystem::create_directories(path.parent_path(), ec);

    std::ofstream f(path);
    if (!f.is_open()) return false;

    f << "{\n  \"access_token\": \"" << token << "\",\n  \"saved_at\": " 
      << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() 
      << "\n}";
    return true;
}

bool SaaSConnector::load_token_from_disk() {
    std::ifstream f(config_.token_storage_path);
    if (!f.is_open()) return false;

    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    std::string token = extract_json_field(content, "access_token");
    if (!token.empty()) {
        std::lock_guard<std::recursive_mutex> lock(auth_mutex_);
        jwt_token_ = token;
        NEXUS_LOG_INFO("Loaded cached JWT token from " + config_.token_storage_path);
        return true;
    }
    return false;
}

std::string SaaSConnector::get_active_token() const {
    std::lock_guard<std::recursive_mutex> lock(auth_mutex_);
    return jwt_token_;
}

void SaaSConnector::push_threat_to_cloud(const std::string& attacker_ip, 
                                        const std::string& mitre_id, 
                                        float confidence, 
                                        const std::string& xai_summary) {
    if (!running_.load() || !config_.enabled) return;

    std::ostringstream ss;
    ss << "{\n"
       << "  \"attacker_ip\": \"" << attacker_ip << "\",\n"
       << "  \"mitre_id\": \"" << mitre_id << "\",\n"
       << "  \"confidence\": " << std::fixed << std::setprecision(3) << confidence << ",\n"
       << "  \"xai_audit_summary\": \"" << xai_summary << "\",\n"
       << "  \"timestamp\": " << std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::system_clock::now().time_since_epoch()).count() << "\n"
       << "}";

    std::lock_guard<std::mutex> lock(queue_mutex_);
    pending_threat_payloads_.push_back(ss.str());
}

void SaaSConnector::outbound_sync_worker(std::stop_token st) {
    while (!st.stop_requested() && running_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(config_.sync_interval_sec));

        if (!ensure_authenticated()) continue;

        // 1. Flush pending threats
        std::vector<std::string> threats_to_flush;
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            if (!pending_threat_payloads_.empty()) {
                threats_to_flush.swap(pending_threat_payloads_);
            }
        }

        for (const auto& threat_json : threats_to_flush) {
            std::string resp;
            http_post_json("/threats/broadcast", threat_json, resp);
        }

        // 2. Periodic Fleet Telemetry Sync
        if (config_.push_telemetry) {
            auto nodes = fleet::NodeRegistry::instance().get_all_nodes();
            std::ostringstream ss;
            ss << "{\n"
               << "  \"tenant_id\": \"" << config_.tenant_id << "\",\n"
               << "  \"nodes_count\": " << nodes.size() << ",\n"
               << "  \"nodes\": [\n";

            for (size_t i = 0; i < nodes.size(); ++i) {
                const auto& n = nodes[i];
                ss << "    {\n"
                   << "      \"node_id\": \"" << n.node_id << "\",\n"
                   << "      \"site\": \"" << n.site_identifier << "\",\n"
                   << "      \"status\": \"" << (n.status == fleet::NodeHealthStatus::ONLINE ? "ONLINE" : "OFFLINE") << "\",\n"
                   << "      \"cpu_pct\": " << n.latest_metrics.cpu_usage_pct() << ",\n"
                   << "      \"ebpf_drops\": " << n.latest_metrics.ebpf_packets_dropped() << ",\n"
                   << "      \"mitigation_latency_us\": " << n.latest_metrics.avg_mitigation_latency_us() << "\n"
                   << "    }" << (i + 1 < nodes.size() ? ",\n" : "\n");
            }
            ss << "  ]\n}";

            std::string response;
            if (http_post_json("/fleet/sync", ss.str(), response)) {
                cloud_connected_.store(true);
                last_sync_time_.store(std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count());
            }
        }
    }
}

void SaaSConnector::inbound_threat_feed_worker(std::stop_token st) {
    while (!st.stop_requested() && running_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(20));
        if (!config_.pull_global_threats || !cloud_connected_.load()) continue;

        std::string response;
        if (http_get_json("/threats/global-feed", response)) {
            size_t pos = 0;
            while ((pos = response.find("\"ip\":", pos)) != std::string::npos) {
                size_t start = response.find('"', pos + 5);
                size_t end = response.find('"', start + 1);
                if (start != std::string::npos && end != std::string::npos) {
                    std::string global_ip = response.substr(start + 1, end - start - 1);
                    
                    ::sentinel::nexus::ThreatIndicator global_threat;
                    global_threat.set_origin_node_id("ARYORITHM-CLOUD-FEED");
                    global_threat.set_attacker_ip(global_ip);
                    global_threat.set_type(::sentinel::nexus::THREAT_EXPLOIT_PAYLOAD);
                    global_threat.set_confidence(0.999f);

                    intelligence::IocBroadcaster::instance().broadcast_threat(global_threat);
                }
                pos = end;
            }
        }
    }
}

void SaaSConnector::remote_command_worker(std::stop_token st) {
    while (!st.stop_requested() && running_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        if (!cloud_connected_.load()) continue;

        std::string response;
        if (http_get_json("/tenants/" + config_.tenant_id + "/commands/pending", response)) {
            if (response.find("EMERGENCY_ROLLBACK") != std::string::npos) {
                NEXUS_LOG_CRIT("Received Cloud Remote Command: EMERGENCY_ROLLBACK.");
                ota::CanaryOrchestrator::instance().trigger_emergency_rollback();
            } else if (response.find("ADVANCE_MODEL") != std::string::npos) {
                NEXUS_LOG_INFO("Received Cloud Remote Command: ADVANCE_MODEL.");
                ota::CanaryOrchestrator::instance().advance_rollout_stage(::sentinel::nexus::STAGE_FLEET_WIDE);
            }
        }
    }
}

// -----------------------------------------------------------------------------
// HTTP Client Engine (Deadlock-Free, Timeout-Protected, Content-Length Parsing)
// -----------------------------------------------------------------------------
bool SaaSConnector::http_post_json(const std::string& url_path, const std::string& json_body, std::string& out_response, bool retry_on_401) {
    ParsedUrl purl = parse_url(config_.cloud_endpoint);
    std::string full_path = purl.path + url_path;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return false;

    // Set 3-second socket send/receive timeouts to prevent hanging
    struct timeval tv{3, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    struct hostent* server = gethostbyname(purl.host.c_str());
    if (!server) {
        close(sock);
        return false;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(purl.port);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sock);
        return false;
    }

    // Do NOT attach Authorization header to login route
    std::string token = (url_path == "/auth/login") ? "" : get_active_token();

    std::ostringstream req;
    req << "POST " << full_path << " HTTP/1.1\r\n"
        << "Host: " << purl.host << ":" << purl.port << "\r\n";
    
    if (!token.empty()) {
        req << "Authorization: Bearer " << token << "\r\n";
    }
    
    req << "X-Tenant-ID: " << config_.tenant_id << "\r\n"
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << json_body.size() << "\r\n"
        << "Connection: close\r\n\r\n"
        << json_body;

    std::string req_str = req.str();

    std::cout << "\n\033[36m==================================================================" << std::endl;
    std::cout << "[SaaSConnector:OUTBOUND-POST] >>> " << config_.cloud_endpoint << url_path << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;
    std::cout << req_str << std::endl;
    std::cout << "==================================================================\033[0m" << std::endl;

    send(sock, req_str.data(), req_str.size(), 0);

    char buffer[4096];
    ssize_t bytes;
    out_response.clear();

    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        out_response.append(buffer, bytes);

        // Check Content-Length to exit loop immediately when response body is complete
        size_t header_end = out_response.find("\r\n\r\n");
        if (header_end != std::string::npos) {
            size_t cl_pos = out_response.find("Content-Length: ");
            if (cl_pos == std::string::npos) cl_pos = out_response.find("content-length: ");
            if (cl_pos != std::string::npos) {
                size_t cl_end = out_response.find("\r\n", cl_pos);
                try {
                    int content_len = std::stoi(out_response.substr(cl_pos + 16, cl_end - cl_pos - 16));
                    if (out_response.size() >= (header_end + 4 + content_len)) {
                        break;
                    }
                } catch (...) {}
            }
        }
    }
    close(sock);

    std::cout << "\033[33m[SaaSConnector:INBOUND-RESP] <<< Response from FastAPI backend:" << std::endl;
    std::cout << out_response << "\033[0m\n" << std::endl;

    if (retry_on_401 && out_response.find("401 Unauthorized") != std::string::npos) {
        NEXUS_LOG_WARN("JWT Token expired (401 Unauthorized). Re-authenticating with backend...");
        if (authenticate()) {
            return http_post_json(url_path, json_body, out_response, false);
        }
    }

    return out_response.find("200 OK") != std::string::npos || out_response.find("201 Created") != std::string::npos;
}

bool SaaSConnector::http_get_json(const std::string& url_path, std::string& out_response, bool retry_on_401) {
    ParsedUrl purl = parse_url(config_.cloud_endpoint);
    std::string full_path = purl.path + url_path;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return false;

    struct timeval tv{3, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    struct hostent* server = gethostbyname(purl.host.c_str());
    if (!server) {
        close(sock);
        return false;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(purl.port);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sock);
        return false;
    }

    std::string token = get_active_token();

    std::ostringstream req;
    req << "GET " << full_path << " HTTP/1.1\r\n"
        << "Host: " << purl.host << ":" << purl.port << "\r\n";

    if (!token.empty()) {
        req << "Authorization: Bearer " << token << "\r\n";
    }

    req << "X-Tenant-ID: " << config_.tenant_id << "\r\n"
        << "Connection: close\r\n\r\n";

    std::string req_str = req.str();
    send(sock, req_str.data(), req_str.size(), 0);

    char buffer[4096];
    ssize_t bytes;
    out_response.clear();
    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        out_response.append(buffer, bytes);

        size_t header_end = out_response.find("\r\n\r\n");
        if (header_end != std::string::npos) {
            size_t cl_pos = out_response.find("Content-Length: ");
            if (cl_pos == std::string::npos) cl_pos = out_response.find("content-length: ");
            if (cl_pos != std::string::npos) {
                size_t cl_end = out_response.find("\r\n", cl_pos);
                try {
                    int content_len = std::stoi(out_response.substr(cl_pos + 16, cl_end - cl_pos - 16));
                    if (out_response.size() >= (header_end + 4 + content_len)) {
                        break;
                    }
                } catch (...) {}
            }
        }
    }
    close(sock);

    if (retry_on_401 && out_response.find("401 Unauthorized") != std::string::npos) {
        NEXUS_LOG_WARN("JWT Token expired on GET (401). Re-authenticating with backend...");
        if (authenticate()) {
            return http_get_json(url_path, out_response, false);
        }
    }

    return out_response.find("200 OK") != std::string::npos;
}

} // namespace sentinel::nexus::cloud
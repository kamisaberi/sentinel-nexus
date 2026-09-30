#include "SaaSConnector.hpp"
#include "core/Logger.hpp"
#include "fleet/NodeRegistry.hpp"
#include "intelligence/GlobalThreatCache.hpp"
#include "intelligence/IocBroadcaster.hpp"
#include "ota/CanaryOrchestrator.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace sentinel::nexus::cloud {

// Helper to parse URL: protocol, host, port, path
struct ParsedUrl {
    bool is_https;
    std::string host;
    uint16_t port;
    std::string path;
};

static ParsedUrl parse_url(const std::string& url) {
    ParsedUrl res{false, "api.aryorithm.com", 80, "/api/v1"};
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

bool SaaSConnector::start(const SaaSConfig& config) {
    config_ = config;

    if (!config_.enabled) {
        NEXUS_LOG_INFO("SaaS Connector disabled in configuration. Operating in 100% Sovereign Air-Gapped mode.");
        return true;
    }

    if (config_.api_key.empty() || config_.tenant_id.empty()) {
        NEXUS_LOG_WARN("SaaS Connector enabled but missing api_key or tenant_id. Cloud sync suspended.");
        return false;
    }

    running_.store(true);
    NEXUS_LOG_INFO("SaaS Connector initialized. Uplink destination: " + config_.cloud_endpoint + 
                   " (Tenant ID: " + config_.tenant_id + ")");

    // Launch dedicated background sync and listener workers
    sync_thread_ = std::jthread([this](std::stop_token st) { outbound_sync_worker(st); });
    threat_feed_thread_ = std::jthread([this](std::stop_token st) { inbound_threat_feed_worker(st); });
    command_thread_ = std::jthread([this](std::stop_token st) { remote_command_worker(st); });

    return true;
}

void SaaSConnector::stop() {
    running_.store(false);
    cloud_connected_.store(false);
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

// -----------------------------------------------------------------------------
// WORKER 1: Outbound Telemetry Synchronization (Pushes node health & metrics)
// -----------------------------------------------------------------------------
void SaaSConnector::outbound_sync_worker(std::stop_token st) {
    while (!st.stop_requested() && running_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(config_.sync_interval_sec));

        // 1. Flush any pending real-time threat broadcasts
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
            } else {
                cloud_connected_.store(false);
            }
        }
    }
}

// -----------------------------------------------------------------------------
// WORKER 2: Inbound Threat Intelligence Feed ("Global Immunity")
// -----------------------------------------------------------------------------
void SaaSConnector::inbound_threat_feed_worker(std::stop_token st) {
    while (!st.stop_requested() && running_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(20));
        if (!config_.pull_global_threats || !cloud_connected_.load()) continue;

        std::string response;
        if (http_get_json("/threats/global-feed", response)) {
            // Parse global IoC array: e.g. [{"ip": "185.x.x.x"}]
            size_t pos = 0;
            while ((pos = response.find("\"ip\":", pos)) != std::string::npos) {
                size_t start = response.find('"', pos + 5);
                size_t end = response.find('"', start + 1);
                if (start != std::string::npos && end != std::string::npos) {
                    std::string global_ip = response.substr(start + 1, end - start - 1);
                    
                    // Inject directly into local Collective Defense engine
                    ::sentinel::nexus::ThreatIndicator global_threat;
                    global_threat.set_origin_node_id("ARYORITHM-GLOBAL-FEED");
                    global_threat.set_attacker_ip(global_ip);
                    global_threat.set_type(::sentinel::nexus::THREAT_EXPLOIT_PAYLOAD);
                    global_threat.set_confidence(0.999f);

                    intelligence::IocBroadcaster::instance().broadcast_threat(global_threat);
                    NEXUS_LOG_INFO("Injected Global Threat Feed IoC [" + global_ip + "] into local eBPF grid.");
                }
                pos = end;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// WORKER 3: Remote CISO Cloud Commands (Emergency Rollback, Policy Tuning)
// -----------------------------------------------------------------------------
void SaaSConnector::remote_command_worker(std::stop_token st) {
    while (!st.stop_requested() && running_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        if (!cloud_connected_.load()) continue;

        std::string response;
        if (http_get_json("/tenants/" + config_.tenant_id + "/commands/pending", response)) {
            // Check for emergency remote commands
            if (response.find("EMERGENCY_ROLLBACK") != std::string::npos) {
                NEXUS_LOG_CRIT("Received Cloud Remote Command: EMERGENCY_ROLLBACK from CISO Dashboard.");
                ota::CanaryOrchestrator::instance().trigger_emergency_rollback();
            } else if (response.find("ADVANCE_MODEL") != std::string::npos) {
                NEXUS_LOG_INFO("Received Cloud Remote Command: ADVANCE_MODEL.");
                ota::CanaryOrchestrator::instance().advance_rollout_stage(::sentinel::nexus::STAGE_FLEET_WIDE);
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Client HTTP/HTTPS Network Engine
// -----------------------------------------------------------------------------
bool SaaSConnector::http_post_json(const std::string& url_path, const std::string& json_body, std::string& out_response) {
    ParsedUrl purl = parse_url(config_.cloud_endpoint);
    std::string full_path = purl.path + url_path;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return false;

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

    std::ostringstream req;
    req << "POST " << full_path << " HTTP/1.1\r\n"
        << "Host: " << purl.host << "\r\n"
        << "X-API-Key: " << config_.api_key << "\r\n"
        << "X-Tenant-ID: " << config_.tenant_id << "\r\n"
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << json_body.size() << "\r\n"
        << "Connection: close\r\n\r\n"
        << json_body;

    std::string req_str = req.str();
    send(sock, req_str.data(), req_str.size(), 0);

    char buffer[4096];
    ssize_t bytes;
    out_response.clear();
    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        out_response.append(buffer, bytes);
    }

    close(sock);
    return out_response.find("200 OK") != std::string::npos || out_response.find("201 Created") != std::string::npos;
}

bool SaaSConnector::http_get_json(const std::string& url_path, std::string& out_response) {
    ParsedUrl purl = parse_url(config_.cloud_endpoint);
    std::string full_path = purl.path + url_path;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return false;

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

    std::ostringstream req;
    req << "GET " << full_path << " HTTP/1.1\r\n"
        << "Host: " << purl.host << "\r\n"
        << "X-API-Key: " << config_.api_key << "\r\n"
        << "X-Tenant-ID: " << config_.tenant_id << "\r\n"
        << "Connection: close\r\n\r\n";

    std::string req_str = req.str();
    send(sock, req_str.data(), req_str.size(), 0);

    char buffer[4096];
    ssize_t bytes;
    out_response.clear();
    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        out_response.append(buffer, bytes);
    }

    close(sock);
    return out_response.find("200 OK") != std::string::npos;
}

} // namespace sentinel::nexus::cloud
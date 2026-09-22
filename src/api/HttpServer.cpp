#include "HttpServer.hpp"
#include "core/Logger.hpp"
#include "fleet/NodeRegistry.hpp"
#include "reporting/ReportGenerator.hpp"
#include "intelligence/IocBroadcaster.hpp"
#include "ota/CanaryOrchestrator.hpp"
#include "telemetry/ForgeBridge.hpp"
#include "intelligence/GlobalThreatCache.hpp"
#include "reporting/CmmcAuditEngine.hpp"
#include "reporting/ScadaAuditEngine.hpp"
#include "fleet/GroupManager.hpp"
#include "ota/ModelRepository.hpp"
#include "api/TelemetryStreamer.hpp"
#include "controllers/FleetController.hpp"
#include "controllers/ThreatController.hpp"
#include "controllers/ModelController.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iomanip>

namespace sentinel::nexus::api
{

    bool HttpServer::start(const std::string &bind_ip, uint16_t port, const std::string &web_root)
    {
        bind_ip_ = bind_ip;
        port_ = port;
        web_root_ = web_root;

        server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd_ < 0)
        {
            NEXUS_LOG_ERROR("Failed to create HTTP socket.");
            return false;
        }

        int opt = 1;
        setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        inet_pton(AF_INET, bind_ip_.c_str(), &addr.sin_addr);

        if (bind(server_fd_, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            NEXUS_LOG_ERROR("HTTP Server bind failed on port " + std::to_string(port_));
            close(server_fd_);
            return false;
        }

        if (listen(server_fd_, 64) < 0)
        {
            NEXUS_LOG_ERROR("HTTP Server listen failed.");
            close(server_fd_);
            return false;
        }

        running_ = true;
        worker_thread_ = std::jthread([this]()
                                      { server_loop(); });

        NEXUS_LOG_INFO("Sentinel-Nexus Web Command Center active at http://" + bind_ip_ + ":" + std::to_string(port_));
        return true;
    }

    void HttpServer::stop()
    {
        running_ = false;
        if (server_fd_ >= 0)
        {
            shutdown(server_fd_, SHUT_RDWR);
            close(server_fd_);
            server_fd_ = -1;
        }
    }

    void HttpServer::server_loop()
    {
        while (running_)
        {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            int client_sock = accept(server_fd_, (struct sockaddr *)&client_addr, &client_len);
            if (client_sock < 0)
            {
                if (!running_)
                    break;
                continue;
            }

            std::thread([this, client_sock]()
                        { handle_client(client_sock); })
                .detach();
        }
    }

    void HttpServer::handle_client(int client_sock)
    {
        char buffer[8192];
        ssize_t bytes_read = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0)
        {
            close(client_sock);
            return;
        }
        buffer[bytes_read] = '\0';

        std::istringstream req_stream(buffer);
        std::string method, path, protocol;
        req_stream >> method >> path >> protocol;

        // 1. Locate HTTP body if this is a POST request
        std::string raw_req(buffer, bytes_read);
        std::string body;
        size_t header_end = raw_req.find("\r\n\r\n");
        if (header_end != std::string::npos)
        {
            body = raw_req.substr(header_end + 4);
        }

        // 2. Strip query parameters for clean path matching (e.g. /api/v1/telemetry/stream?t=123)
        std::string route_path = path;
        size_t qmark = route_path.find('?');
        if (qmark != std::string::npos)
        {
            route_path = route_path.substr(0, qmark);
        }

        // 3. REAL-TIME STREAM INTERCEPTION:
        // If the browser requests the SSE stream, pass the open socket to TelemetryStreamer.
        // We RETURN IMMEDIATELY so that close(client_sock) is NEVER called here!
        if (route_path == "/api/v1/telemetry/stream")
        {
            TelemetryStreamer::instance().add_client(client_sock);
            return; // Socket remains alive and managed by TelemetryStreamer
        }

        // 4. Standard Request/Response cycle for REST API and HTML/CSS/JS files
        std::string content_type = "text/plain";
        int status_code = 200;
        std::string response_payload = route_request(method, path, body, content_type, status_code);

        std::string status_text = (status_code == 200) ? "200 OK" : ((status_code == 404) ? "404 Not Found" : "500 Internal Server Error");

        std::ostringstream resp_stream;
        resp_stream << "HTTP/1.1 " << status_text << "\r\n"
                    << "Content-Type: " << content_type << "\r\n"
                    << "Content-Length: " << response_payload.size() << "\r\n"
                    << "Access-Control-Allow-Origin: *\r\n"
                    << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                    << "Access-Control-Allow-Headers: Content-Type\r\n"
                    << "Connection: close\r\n\r\n"
                    << response_payload;

        std::string resp_str = resp_stream.str();
        send(client_sock, resp_str.data(), resp_str.size(), 0);

        // Standard HTTP request is finished; close socket
        close(client_sock);
    }

    std::string HttpServer::route_request(const std::string &method,
                                          const std::string &path,
                                          const std::string &body,
                                          std::string &content_type,
                                          int &status_code)
    {
        if (method == "OPTIONS")
        {
            status_code = 200;
            return "";
        }

        // Strip query parameters for routing (e.g. /api/v1/nodes?ts=123)
        std::string route_path = path;
        size_t qmark = route_path.find('?');
        if (qmark != std::string::npos)
        {
            route_path = route_path.substr(0, qmark);
        }

        // REST API Routes

        if (route_path == "/api/v1/models")
        {
            content_type = "application/json";
            return ota::ModelRepository::instance().generate_models_json();
        }
        if (route_path.rfind("/models/", 0) == 0)
        {
            std::string model_file = route_path.substr(8);
            ota::StoredModel m;
            if (ota::ModelRepository::instance().get_model(model_file, m))
            {
                content_type = "application/octet-stream";
                status_code = 200;
                std::ifstream f(m.full_path, std::ios::binary);
                std::ostringstream ss;
                ss << f.rdbuf();
                return ss.str();
            }
        }

        if (route_path == "/api/v1/fleet/nodes")
        {
            content_type = "application/json";
            return handle_get_nodes();
        }

        if (route_path == "/api/v1/fleet/groups")
        {
            content_type = "application/json";
            return fleet::GroupManager::instance().generate_groups_json();
        }

        if (route_path == "/api/v1/reports/compliance")
        {
            content_type = "application/json";
            return handle_get_compliance();
        }
        if (route_path == "/api/v1/ota/status")
        {
            content_type = "application/json";
            return handle_get_ota();
        }

        if (route_path == "/api/v1/models")
        {
            content_type = "application/json";
            return controllers::ModelController::list_models_json();
        }

        if (route_path == "/api/v1/threats/mitre")
        {
            content_type = "application/json";
            return intelligence::GlobalThreatCache::instance().generate_mitre_summary_json();
        }

        if (route_path == "/api/v1/threats/broadcast" && method == "POST")
        {
            content_type = "application/json";
            return handle_post_broadcast(body);
        }
        if (route_path == "/api/v1/ota/stage" && method == "POST")
        {
            content_type = "application/json";
            return handle_post_ota_stage(body);
        }
        if (route_path == "/api/v1/ota/advance" && method == "POST")
        {
            content_type = "application/json";
            return handle_post_ota_advance();
        }
        if (route_path == "/api/v1/ota/rollback" && method == "POST")
        {
            content_type = "application/json";
            return handle_post_ota_rollback();
        }

        if (route_path == "/api/v1/reports/cmmc")
        {
            content_type = "application/json";
            return reporting::CmmcAuditEngine::instance().generate_cmmc_assessment_json();
        }
        if (route_path == "/api/v1/reports/scada")
        {
            content_type = "application/json";
            return reporting::ScadaAuditEngine::instance().generate_iec62443_assessment_json();
        }

        // Static Web Dashboard Files
        return serve_static_file(route_path, content_type, status_code);
    }

    std::string HttpServer::handle_get_nodes()
    {
        auto nodes = fleet::NodeRegistry::instance().get_all_nodes();
        std::ostringstream ss;
        ss << "[\n";
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            const auto &n = nodes[i];
            ss << "  {\n"
               << "    \"node_id\": \"" << n.node_id << "\",\n"
               << "    \"site\": \"" << n.site_identifier << "\",\n"
               << "    \"hostname\": \"" << n.identity.hostname() << "\",\n"
               << "    \"kernel\": \"" << n.identity.kernel_version() << "\",\n"
               << "    \"status\": \"" << (n.status == fleet::NodeHealthStatus::ONLINE ? "ONLINE" : "OFFLINE") << "\",\n"
               << "    \"cpu_pct\": " << n.latest_metrics.cpu_usage_pct() << ",\n"
               << "    \"ram_mb\": " << n.latest_metrics.ram_usage_mb() << ",\n"
               << "    \"npu_temp_c\": " << n.latest_metrics.npu_gpu_temp_celsius() << ",\n"
               << "    \"packets_inspected\": " << n.latest_metrics.packets_inspected() << ",\n"
               << "    \"ebpf_drops\": " << n.latest_metrics.ebpf_packets_dropped() << ",\n"
               << "    \"mitigation_latency_us\": " << n.latest_metrics.avg_mitigation_latency_us() << "\n"
               << "  }" << (i + 1 < nodes.size() ? ",\n" : "\n");
        }
        ss << "]";
        return ss.str();
    }

    std::string HttpServer::handle_get_compliance()
    {
        return reporting::ReportGenerator::instance().generate_json_compliance_report();
    }

    std::string HttpServer::handle_get_ota()
    {
        auto &ota = ota::CanaryOrchestrator::instance();
        std::ostringstream ss;
        ss << "{\n"
           << "  \"stable_version\": \"" << ota.get_stable_version() << "\",\n"
           << "  \"candidate_version\": \"" << ota.get_candidate_version() << "\",\n"
           << "  \"stage\": " << static_cast<int>(ota.get_current_stage()) << "\n"
           << "}";
        return ss.str();
    }

    std::string HttpServer::handle_post_broadcast(const std::string &body)
    {
        std::string ip;
        size_t pos = body.find("\"ip\"");
        if (pos != std::string::npos)
        {
            size_t colon = body.find(':', pos);
            size_t start = body.find('"', colon + 1);
            size_t end = body.find('"', start + 1);
            if (start != std::string::npos && end != std::string::npos)
            {
                ip = body.substr(start + 1, end - start - 1);
            }
        }

        if (!ip.empty())
        {
            ::sentinel::nexus::ThreatIndicator threat;
            threat.set_origin_node_id("NEXUS-ADMIN-CONSOLE");
            threat.set_attacker_ip(ip);
            threat.set_type(::sentinel::nexus::THREAT_EXPLOIT_PAYLOAD);
            intelligence::IocBroadcaster::instance().broadcast_threat(threat);
            return "{\"status\": \"broadcast_dispatched\", \"target_ip\": \"" + ip + "\"}";
        }

        return "{\"status\": \"error\", \"message\": \"invalid IP\"}";
    }

    std::string HttpServer::handle_post_ota_stage(const std::string &body)
    {
        (void)body;
        ota::CanaryOrchestrator::instance().stage_candidate_model(
            "network_threat_v2.onnx", "8fa9c89b3f4618e47f5255470d9a690e7da3c6046e297893a776", "/models/v2.onnx");
        return "{\"status\": \"candidate_staged\", \"stage\": \"SHADOW_MODE\"}";
    }

    std::string HttpServer::handle_post_ota_advance()
    {
        auto &ota = ota::CanaryOrchestrator::instance();
        auto cur = ota.get_current_stage();
        if (cur == ::sentinel::nexus::STAGE_SHADOW_MODE)
        {
            ota.advance_rollout_stage(::sentinel::nexus::STAGE_CANARY_5_PCT);
            return "{\"status\": \"advanced\", \"stage\": \"CANARY_5_PCT\"}";
        }
        else if (cur == ::sentinel::nexus::STAGE_CANARY_5_PCT)
        {
            ota.advance_rollout_stage(::sentinel::nexus::STAGE_FLEET_WIDE);
            return "{\"status\": \"promoted\", \"stage\": \"FLEET_WIDE\"}";
        }
        return "{\"status\": \"no_action\"}";
    }

    std::string HttpServer::handle_post_ota_rollback()
    {
        ota::CanaryOrchestrator::instance().trigger_emergency_rollback();
        return "{\"status\": \"emergency_rollback_executed\"}";
    }

    std::string HttpServer::serve_static_file(const std::string &req_path, std::string &content_type, int &status_code)
    {
        std::string clean_path = (req_path == "/" || req_path.empty()) ? "index.html" : req_path;
        if (!clean_path.empty() && clean_path.front() == '/')
        {
            clean_path = clean_path.substr(1);
        }

        // Smart Multi-Path Resolution:
        // 1. Current working directory (./web/...)
        // 2. Parent directory (../web/... when running from build/)
        // 3. Executable's parent directory (/home/kami/sentinel-nexus/web/...)
        std::vector<std::filesystem::path> search_paths = {
            std::filesystem::path(web_root_) / clean_path,
            std::filesystem::path("..") / web_root_ / clean_path};

        try
        {
            auto exe_dir = std::filesystem::canonical("/proc/self/exe").parent_path();
            search_paths.push_back(exe_dir / web_root_ / clean_path);
            search_paths.push_back(exe_dir.parent_path() / web_root_ / clean_path);
        }
        catch (...)
        {
        }

        std::filesystem::path target_file;
        bool found = false;
        for (const auto &p : search_paths)
        {
            if (std::filesystem::exists(p) && !std::filesystem::is_directory(p))
            {
                target_file = p;
                found = true;
                break;
            }
        }

        if (!found)
        {
            NEXUS_LOG_WARN("HTTP 404: Could not find requested file: " + clean_path);
            status_code = 404;
            return "404 Not Found: " + clean_path;
        }

        std::string ext = target_file.extension().string();
        if (ext == ".html")
            content_type = "text/html; charset=utf-8";
        else if (ext == ".css")
            content_type = "text/css; charset=utf-8";
        else if (ext == ".js")
            content_type = "application/javascript; charset=utf-8";
        else if (ext == ".json")
            content_type = "application/json";
        else
            content_type = "application/octet-stream";

        std::ifstream file(target_file, std::ios::binary);
        std::ostringstream buffer;
        buffer << file.rdbuf();
        status_code = 200;
        return buffer.str();
    }

} // namespace sentinel::nexus::api
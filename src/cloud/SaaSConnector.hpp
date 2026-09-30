#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <memory>
#include <chrono>

namespace sentinel::nexus::cloud {

struct SaaSConfig {
    bool enabled{false};
    std::string cloud_endpoint{"http://127.0.0.1:8000/api/v1"};
    std::string tenant_id{"tenant-dev-local"};
    std::string auth_email{"kamisaberi@gmail.com"};
    std::string auth_password{"12345678"};
    std::string token_storage_path{"data/cloud_session.json"};
    uint32_t sync_interval_sec{5};
    uint32_t heartbeat_interval_sec{30};
    bool push_telemetry{true};
    bool pull_global_threats{true};
    bool pull_ota_models{true};
};

class SaaSConnector {
public:
    static SaaSConnector& instance() {
        static SaaSConnector inst;
        return inst;
    }

    bool start(const SaaSConfig& config);
    void stop();

    // Authenticate with FastAPI and obtain/refresh JWT access token
    bool authenticate();
    bool is_authenticated() const { return !jwt_token_.empty(); }

    void push_threat_to_cloud(const std::string& attacker_ip, 
                              const std::string& mitre_id, 
                              float confidence, 
                              const std::string& xai_summary);

    bool is_connected() const { return cloud_connected_.load(); }
    std::string get_active_token() const;

private:
    SaaSConnector() = default;

    void outbound_sync_worker(std::stop_token st);
    void inbound_threat_feed_worker(std::stop_token st);
    void remote_command_worker(std::stop_token st);

    bool ensure_authenticated();
    bool save_token_to_disk(const std::string& token);
    bool load_token_from_disk();

    // HTTP/HTTPS dispatchers with automated JWT header injection and 401 retry
    bool http_post_json(const std::string& url_path, const std::string& json_body, std::string& out_response, bool retry_on_401 = true);
    bool http_get_json(const std::string& url_path, std::string& out_response, bool retry_on_401 = true);

    SaaSConfig config_;
    std::atomic<bool> running_{false};
    std::atomic<bool> cloud_connected_{false};
    std::atomic<uint64_t> last_sync_time_{0};

    mutable std::mutex auth_mutex_;
    std::string jwt_token_{""};

    std::jthread sync_thread_;
    std::jthread threat_feed_thread_;
    std::jthread command_thread_;

    mutable std::mutex queue_mutex_;
    std::vector<std::string> pending_threat_payloads_;
};

} // namespace sentinel::nexus::cloud
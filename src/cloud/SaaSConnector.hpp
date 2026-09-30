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
    std::string cloud_endpoint{"https://api.aryorithm.com/api/v1"};
    std::string tenant_id{""};
    std::string api_key{""};
    uint32_t sync_interval_sec{15};
    uint32_t heartbeat_interval_sec{30};
    bool push_telemetry{true};
    bool pull_global_threats{true};
    bool pull_ota_models{true};
};

struct CloudCommand {
    std::string command_id;
    std::string action; // "EMERGENCY_DROP", "STAGE_MODEL", "ADVANCE_MODEL", "PURGE_IP"
    std::string target_payload;
};

class SaaSConnector {
public:
    static SaaSConnector& instance() {
        static SaaSConnector inst;
        return inst;
    }

    // Starts background outbound cloud synchronization threads
    bool start(const SaaSConfig& config);
    void stop();

    // Outbound real-time push: Transmit an edge zero-day threat to Aryorithm Global Threat Bus
    void push_threat_to_cloud(const std::string& attacker_ip, 
                              const std::string& mitre_id, 
                              float confidence, 
                              const std::string& xai_summary);

    bool is_connected() const { return cloud_connected_.load(); }
    uint64_t last_sync_timestamp() const { return last_sync_time_.load(); }

private:
    SaaSConnector() = default;

    void outbound_sync_worker(std::stop_token st);
    void inbound_threat_feed_worker(std::stop_token st);
    void remote_command_worker(std::stop_token st);

    // HTTP/HTTPS Client Dispatchers
    bool http_post_json(const std::string& url_path, const std::string& json_body, std::string& out_response);
    bool http_get_json(const std::string& url_path, std::string& out_response);

    // Command dispatch execution
    void execute_remote_command(const CloudCommand& cmd);

    SaaSConfig config_;
    std::atomic<bool> running_{false};
    std::atomic<bool> cloud_connected_{false};
    std::atomic<uint64_t> last_sync_time_{0};

    std::jthread sync_thread_;
    std::jthread threat_feed_thread_;
    std::jthread command_thread_;

    mutable std::mutex queue_mutex_;
    std::vector<std::string> pending_threat_payloads_;
};

} // namespace sentinel::nexus::cloud
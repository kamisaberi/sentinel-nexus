#pragma once
#include <string>
#include <cstdint>

namespace sentinel::nexus::core {

struct NexusConfig {
    std::string bind_address = "0.0.0.0";
    uint16_t grpc_port = 50051;
    uint16_t rest_port = 9443;
    uint16_t ws_port = 9444;
    uint32_t worker_threads = 8;
    
    uint32_t heartbeat_timeout_sec = 15;
    bool forge_enabled = true;
    std::string forge_buffer_path = "/tmp/sentinel_nexus_forge/";
};

class ConfigManager {
public:
    static ConfigManager& instance() {
        static ConfigManager inst;
        return inst;
    }

    bool load_config(const std::string& config_path);
    const NexusConfig& get() const { return config_; }

private:
    ConfigManager() = default;
    NexusConfig config_;
};

} // namespace sentinel::nexus::core
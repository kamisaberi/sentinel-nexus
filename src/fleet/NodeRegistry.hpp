#pragma once
#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <chrono>
#include <vector>
#include "common.pb.h"
#include "fleet.pb.h"

namespace sentinel::nexus::fleet {

enum class NodeHealthStatus {
    ONLINE,
    DEGRADED,
    OFFLINE
};

struct RegisteredNode {
    std::string node_id;
    std::string site_identifier;
    std::string software_version;
    HardwareIdentity identity;
    DeviceMetrics latest_metrics;
    std::chrono::system_clock::time_point last_heartbeat;
    NodeHealthStatus status;
};

class NodeRegistry {
public:
    static NodeRegistry& instance() {
        static NodeRegistry inst;
        return inst;
    }

    std::string register_node(const RegistrationRequest& req);
    bool update_heartbeat(const std::string& node_id, const DeviceMetrics& metrics);
    void evaluate_node_health(uint32_t timeout_seconds);

    size_t active_node_count() const;
    std::vector<RegisteredNode> get_all_nodes() const;

private:
    NodeRegistry() = default;
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, RegisteredNode> nodes_;
};

} // namespace sentinel::nexus::fleet
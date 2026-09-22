#include "HeartbeatMonitor.hpp"
#include "NodeRegistry.hpp"
#include "api/TelemetryStreamer.hpp"
#include "core/Logger.hpp"
#include <chrono>

namespace sentinel::nexus::fleet {

void HeartbeatMonitor::start(uint32_t timeout_seconds, uint32_t check_interval_ms) {
    if (running_.load()) return;
    running_.store(true);

    worker_thread_ = std::jthread([this, timeout_seconds, check_interval_ms]() {
        monitor_loop(timeout_seconds, check_interval_ms);
    });

    NEXUS_LOG_INFO("HeartbeatMonitor watchdog started (Timeout: " + 
                   std::to_string(timeout_seconds) + "s, Interval: " + 
                   std::to_string(check_interval_ms) + "ms)");
}

void HeartbeatMonitor::stop() {
    running_.store(false);
}

void HeartbeatMonitor::monitor_loop(uint32_t timeout_seconds, uint32_t check_interval_ms) {
    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(check_interval_ms));
        if (!running_.load()) break;

        size_t online_before = NodeRegistry::instance().active_node_count();
        NodeRegistry::instance().evaluate_node_health(timeout_seconds);
        size_t online_after = NodeRegistry::instance().active_node_count();

        // Broadcast state sync if node count changed
        if (online_before != online_after) {
            std::string payload = "{\"online_count\":" + std::to_string(online_after) + "}";
            api::TelemetryStreamer::instance().broadcast_event("heartbeat_sync", payload);
        }
    }
}

} // namespace sentinel::nexus::fleet
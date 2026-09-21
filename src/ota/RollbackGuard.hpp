#pragma once
#include <string>
#include <mutex>
#include <unordered_map>
#include "fleet.pb.h"

namespace sentinel::nexus::ota {

class RollbackGuard {
public:
    static RollbackGuard& instance() {
        static RollbackGuard inst;
        return inst;
    }

    void initialize(float max_sla_latency_us = 1000.0f, uint32_t drop_spike_threshold = 500);

    // Inspects heartbeats from nodes running canary/shadow models.
    // Automatically triggers CanaryOrchestrator::trigger_emergency_rollback() on SLA breach.
    void inspect_node_metrics(const std::string& node_id, 
                              const ::sentinel::nexus::DeviceMetrics& metrics,
                              bool is_canary_node);

    void reset_metrics();

private:
    RollbackGuard() = default;

    mutable std::mutex mutex_;
    float max_sla_latency_us_{1000.0f};
    uint32_t drop_spike_threshold_{500};

    std::unordered_map<std::string, uint64_t> previous_drop_counts_;
};

} // namespace sentinel::nexus::ota
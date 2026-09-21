#include "RollbackGuard.hpp"
#include "CanaryOrchestrator.hpp"
#include "core/Logger.hpp"

namespace sentinel::nexus::ota {

void RollbackGuard::initialize(float max_sla_latency_us, uint32_t drop_spike_threshold) {
    std::lock_guard<std::mutex> lock(mutex_);
    max_sla_latency_us_ = max_sla_latency_us;
    drop_spike_threshold_ = drop_spike_threshold;
    NEXUS_LOG_INFO("RollbackGuard initialized (Max Latency SLA: " + 
                   std::to_string(max_sla_latency_us_) + " us, Drop Surge Trigger: " + 
                   std::to_string(drop_spike_threshold_) + ")");
}

void RollbackGuard::inspect_node_metrics(const std::string& node_id, 
                                        const ::sentinel::nexus::DeviceMetrics& metrics,
                                        bool is_canary_node) {
    if (!is_canary_node) return;

    std::lock_guard<std::mutex> lock(mutex_);

    // 1. SLA Latency Breach Check (< 1.0 ms)
    if (metrics.avg_mitigation_latency_us() > max_sla_latency_us_) {
        NEXUS_LOG_CRIT("SLA VIOLATION on Canary Node " + node_id + 
                       ": Latency reached " + std::to_string(metrics.avg_mitigation_latency_us()) + 
                       " us (Max SLA: " + std::to_string(max_sla_latency_us_) + " us)!");
        
        CanaryOrchestrator::instance().trigger_emergency_rollback();
        return;
    }

    // 2. Sudden Abnormal Packet Drop Surge Check (Potential False Positive Cascades)
    auto it = previous_drop_counts_.find(node_id);
    if (it != previous_drop_counts_.end()) {
        uint64_t prev = it->second;
        uint64_t curr = metrics.ebpf_packets_dropped();
        if (curr > prev) {
            uint64_t delta = curr - prev;
            if (delta > drop_spike_threshold_) {
                NEXUS_LOG_CRIT("ANOMALOUS DROP SURGE on Canary Node " + node_id + 
                               ": " + std::to_string(delta) + " drops in single interval! Aborting candidate rollout.");
                
                CanaryOrchestrator::instance().trigger_emergency_rollback();
                return;
            }
        }
    }
    previous_drop_counts_[node_id] = metrics.ebpf_packets_dropped();
}

void RollbackGuard::reset_metrics() {
    std::lock_guard<std::mutex> lock(mutex_);
    previous_drop_counts_.clear();
}

} // namespace sentinel::nexus::ota
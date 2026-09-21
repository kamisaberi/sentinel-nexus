#include "IocBroadcaster.hpp"
#include "GlobalThreatCache.hpp"
#include "core/Logger.hpp"
#include <chrono>

namespace sentinel::nexus::intelligence {

void IocBroadcaster::register_subscriber(const std::string& node_id, StreamContext* stream) {
    std::lock_guard<std::mutex> lock(mutex_);
    subscribers_[node_id] = stream;
    NEXUS_LOG_INFO("Appliance subscribed to Collective Defense: " + node_id);
}

void IocBroadcaster::unregister_subscriber(const std::string& node_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    subscribers_.erase(node_id);
    NEXUS_LOG_INFO("Appliance unsubscribed from Collective Defense: " + node_id);
}

void IocBroadcaster::broadcast_threat(const ::sentinel::nexus::ThreatIndicator& threat, uint64_t ttl_seconds) {
    // Record into the MITRE threat cache
    GlobalThreatCache::instance().record_threat(threat);

    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    ::sentinel::nexus::FleetDefenseRule rule;
    rule.set_rule_id("RULE-EBPF-" + std::to_string(++rule_counter_));
    rule.set_target_ip(threat.attacker_ip());
    rule.set_target_subnet_mask(32);
    rule.set_expires_at_ns(now_ns + (ttl_seconds * 1'000'000'000ULL));
    rule.set_emergency_purge(false);

    NEXUS_LOG_WARN("Propagating kernel drop rule: [" + threat.attacker_ip() + 
                   "] fleet-wide across " + std::to_string(subscribers_.size()) + " appliances!");

    for (auto it = subscribers_.begin(); it != subscribers_.end();) {
        if (it->first == threat.origin_node_id()) {
            ++it;
            continue;
        }

        if (!it->second->Write(rule)) {
            NEXUS_LOG_WARN("Failed writing rule to node " + it->first + ", evicting dead stream.");
            it = subscribers_.erase(it);
        } else {
            ++it;
        }
    }
}

size_t IocBroadcaster::subscriber_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return subscribers_.size();
}

} // namespace sentinel::nexus::intelligence
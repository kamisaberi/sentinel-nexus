#include "GlobalThreatCache.hpp"
#include <sstream>

namespace sentinel::nexus::intelligence {

std::pair<std::string, std::string> GlobalThreatCache::map_mitre(::sentinel::nexus::ThreatType type) {
    switch (type) {
        case ::sentinel::nexus::THREAT_BRUTE_FORCE:
            return {"T1110", "Brute Force"};
        case ::sentinel::nexus::THREAT_C2_BEACON:
            return {"T1071", "Application Layer Protocol (C2 Beacon)"};
        case ::sentinel::nexus::THREAT_SCADA_ANOMALY:
            return {"T0855", "Unauthorized Command Message (ICS/SCADA)"};
        case ::sentinel::nexus::THREAT_PORT_SWEEP:
            return {"T1046", "Network Service Discovery"};
        case ::sentinel::nexus::THREAT_EXPLOIT_PAYLOAD:
            return {"T1190", "Exploit Public-Facing Application"};
        default:
            return {"T1000", "Generic Threat Behavior"};
    }
}

void GlobalThreatCache::record_threat(const ::sentinel::nexus::ThreatIndicator& threat) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto [mitre_id, mitre_name] = map_mitre(threat.type());

    CachedThreat item{
        .origin_node_id = threat.origin_node_id(),
        .attacker_ip = threat.attacker_ip(),
        .port = threat.port(),
        .type = threat.type(),
        .mitre_technique_id = mitre_id,
        .mitre_technique_name = mitre_name,
        .confidence = threat.confidence(),
        .timestamp_ns = threat.timestamp_ns()
    };

    threat_history_.push_front(std::move(item));
    if (threat_history_.size() > max_history_) {
        threat_history_.pop_back();
    }

    mitre_counts_[mitre_id]++;
}

std::vector<CachedThreat> GlobalThreatCache::get_recent_threats(size_t limit) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<CachedThreat> result;
    size_t count = std::min(limit, threat_history_.size());
    for (size_t i = 0; i < count; ++i) {
        result.push_back(threat_history_[i]);
    }
    return result;
}

std::string GlobalThreatCache::generate_mitre_summary_json() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream ss;
    ss << "[\n";
    size_t i = 0;
    for (const auto& [tech_id, count] : mitre_counts_) {
        ss << "  {\n"
           << "    \"technique_id\": \"" << tech_id << "\",\n"
           << "    \"count\": " << count << "\n"
           << "  }" << (++i < mitre_counts_.size() ? ",\n" : "\n");
    }
    ss << "]";
    return ss.str();
}

} // namespace sentinel::nexus::intelligence
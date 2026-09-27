#include "GlobalThreatCache.hpp"
#include <sstream>
#include <iomanip>

namespace sentinel::nexus::intelligence {

std::pair<std::string, std::string> GlobalThreatCache::map_mitre(::sentinel::nexus::ThreatType type) {
    switch (type) {
        case ::sentinel::nexus::THREAT_BRUTE_FORCE: return {"T1110", "Brute Force"};
        case ::sentinel::nexus::THREAT_C2_BEACON: return {"T1071", "C2 Application Protocol"};
        case ::sentinel::nexus::THREAT_SCADA_ANOMALY: return {"T0855", "Unauthorized Command Message"};
        case ::sentinel::nexus::THREAT_PORT_SWEEP: return {"T1046", "Network Service Discovery"};
        case ::sentinel::nexus::THREAT_EXPLOIT_PAYLOAD: return {"T1190", "Exploit Public-Facing Application"};
        default: return {"T1000", "Generic Threat Behavior"};
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
        .timestamp_ns = threat.timestamp_ns(),
        .attributions = {}
    };

    // Store XAI top-k attributions
    for (const auto& attr : threat.attributions()) {
        item.attributions.push_back(attr);
    }

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

std::string GlobalThreatCache::generate_xai_summary_json() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream ss;
    ss << "[\n";

    size_t count = std::min(size_t(20), threat_history_.size());
    for (size_t i = 0; i < count; ++i) {
        const auto& t = threat_history_[i];
        ss << "  {\n"
           << "    \"attacker_ip\": \"" << t.attacker_ip << "\",\n"
           << "    \"origin_node\": \"" << t.origin_node_id << "\",\n"
           << "    \"mitre_id\": \"" << t.mitre_technique_id << "\",\n"
           << "    \"mitre_name\": \"" << t.mitre_technique_name << "\",\n"
           << "    \"confidence\": " << std::fixed << std::setprecision(3) << t.confidence << ",\n"
           << "    \"attributions\": [\n";

        for (size_t j = 0; j < t.attributions.size(); ++j) {
            const auto& a = t.attributions[j];
            ss << "      {\n"
               << "        \"rank\": " << (j + 1) << ",\n"
               << "        \"feature\": \"" << a.feature_name() << "\",\n"
               << "        \"contribution_pct\": " << std::fixed << std::setprecision(1) << a.contribution_percentage() << ",\n"
               << "        \"observed\": \"" << a.observed_value() << "\",\n"
               << "        \"baseline\": \"" << a.baseline_expected() << "\",\n"
               << "        \"audit_note\": \"" << a.audit_summary() << "\"\n"
               << "      }" << (j + 1 < t.attributions.size() ? ",\n" : "\n");
        }
        ss << "    ]\n"
           << "  }" << (i + 1 < count ? ",\n" : "\n");
    }

    ss << "]";
    return ss.str();
}

} // namespace sentinel::nexus::intelligence
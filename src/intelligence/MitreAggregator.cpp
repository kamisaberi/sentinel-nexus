#include "MitreAggregator.hpp"
#include <sstream>

namespace sentinel::nexus::intelligence {

MitreAggregator::MitreAggregator() {
    matrix_["T1110"] = {"T1110", "Brute Force", "Credential Access", 0};
    matrix_["T1071"] = {"T1071", "C2 Application Protocol", "Command and Control", 0};
    matrix_["T0855"] = {"T0855", "Unauthorized Command Message", "ICS/SCADA Physical", 0};
    matrix_["T1046"] = {"T1046", "Network Service Discovery", "Discovery", 0};
    matrix_["T1190"] = {"T1190", "Exploit Public-Facing Application", "Initial Access", 0};
}

void MitreAggregator::record_attack(::sentinel::nexus::ThreatType type, const std::string& attacker_ip) {
    (void)attacker_ip;
    std::lock_guard<std::mutex> lock(mutex_);

    std::string key;
    switch (type) {
        case ::sentinel::nexus::THREAT_BRUTE_FORCE: key = "T1110"; break;
        case ::sentinel::nexus::THREAT_C2_BEACON: key = "T1071"; break;
        case ::sentinel::nexus::THREAT_SCADA_ANOMALY: key = "T0855"; break;
        case ::sentinel::nexus::THREAT_PORT_SWEEP: key = "T1046"; break;
        case ::sentinel::nexus::THREAT_EXPLOIT_PAYLOAD: key = "T1190"; break;
        default: return;
    }

    matrix_[key].occurrences++;
}

std::string MitreAggregator::generate_matrix_json() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream ss;
    ss << "[\n";
    size_t i = 0;
    for (const auto& [_, stat] : matrix_) {
        ss << "  {\n"
           << "    \"technique_id\": \"" << stat.technique_id << "\",\n"
           << "    \"name\": \"" << stat.name << "\",\n"
           << "    \"tactic\": \"" << stat.tactic << "\",\n"
           << "    \"count\": " << stat.occurrences << "\n"
           << "  }" << (++i < matrix_.size() ? ",\n" : "\n");
    }
    ss << "]";
    return ss.str();
}

} // namespace sentinel::nexus::intelligence
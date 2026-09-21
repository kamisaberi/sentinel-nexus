#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <deque>
#include "intelligence.pb.h"

namespace sentinel::nexus::intelligence {

struct CachedThreat {
    std::string origin_node_id;
    std::string attacker_ip;
    uint32_t port;
    ::sentinel::nexus::ThreatType type;
    std::string mitre_technique_id;
    std::string mitre_technique_name;
    float confidence;
    uint64_t timestamp_ns;
};

class GlobalThreatCache {
public:
    static GlobalThreatCache& instance() {
        static GlobalThreatCache inst;
        return inst;
    }

    void record_threat(const ::sentinel::nexus::ThreatIndicator& threat);
    std::vector<CachedThreat> get_recent_threats(size_t limit = 50) const;
    std::string generate_mitre_summary_json() const;

private:
    GlobalThreatCache() = default;

    static std::pair<std::string, std::string> map_mitre(::sentinel::nexus::ThreatType type);

    mutable std::mutex mutex_;
    std::deque<CachedThreat> threat_history_;
    std::unordered_map<std::string, uint64_t> mitre_counts_;
    const size_t max_history_{5000};
};

} // namespace sentinel::nexus::intelligence
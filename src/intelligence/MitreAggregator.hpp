#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include "intelligence.pb.h"

namespace sentinel::nexus::intelligence {

struct MitreStat {
    std::string technique_id;
    std::string name;
    std::string tactic;
    uint64_t occurrences;
};

class MitreAggregator {
public:
    static MitreAggregator& instance() {
        static MitreAggregator inst;
        return inst;
    }

    void record_attack(::sentinel::nexus::ThreatType type, const std::string& attacker_ip);
    std::string generate_matrix_json() const;

private:
    MitreAggregator();

    mutable std::mutex mutex_;
    std::unordered_map<std::string, MitreStat> matrix_;
};

} // namespace sentinel::nexus::intelligence
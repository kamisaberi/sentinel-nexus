#pragma once
#include <string>
#include <vector>
#include <mutex>

namespace sentinel::nexus::reporting {

class LatencySlaReporter {
public:
    static LatencySlaReporter& instance() {
        static LatencySlaReporter inst;
        return inst;
    }

    void record_latency(float latency_us);
    void calculate_percentiles(float& out_p50, float& out_p95, float& out_p99) const;
    bool is_compliant_sub_millisecond() const;
    std::string generate_sla_proof_json() const;

private:
    LatencySlaReporter() = default;

    mutable std::mutex mutex_;
    std::vector<float> latency_samples_;
    const size_t max_samples_{50000};
};

} // namespace sentinel::nexus::reporting
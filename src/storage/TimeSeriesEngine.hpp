#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <chrono>

namespace sentinel::nexus::storage {

struct MetricDataPoint {
    uint64_t timestamp_sec;
    float avg_cpu_pct;
    float avg_latency_us;
    uint64_t drop_count;
};

class TimeSeriesEngine {
public:
    static TimeSeriesEngine& instance() {
        static TimeSeriesEngine inst;
        return inst;
    }

    void record_point(float cpu_pct, float latency_us, uint64_t drops);
    std::string generate_history_json(size_t limit = 60) const;

private:
    TimeSeriesEngine() = default;

    mutable std::mutex mutex_;
    std::vector<MetricDataPoint> history_;
    const size_t max_history_{3600}; // 1 hour of second-by-second history
};

} // namespace sentinel::nexus::storage
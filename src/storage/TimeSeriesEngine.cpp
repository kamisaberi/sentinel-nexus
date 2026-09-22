#include "TimeSeriesEngine.hpp"
#include <sstream>

namespace sentinel::nexus::storage {

void TimeSeriesEngine::record_point(float cpu_pct, float latency_us, uint64_t drops) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    history_.push_back({
        .timestamp_sec = static_cast<uint64_t>(now),
        .avg_cpu_pct = cpu_pct,
        .avg_latency_us = latency_us,
        .drop_count = drops
    });

    if (history_.size() > max_history_) {
        history_.erase(history_.begin(), history_.begin() + 100);
    }
}

std::string TimeSeriesEngine::generate_history_json(size_t limit) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream ss;
    ss << "[\n";

    size_t start = (history_.size() > limit) ? (history_.size() - limit) : 0;
    for (size_t i = start; i < history_.size(); ++i) {
        const auto& pt = history_[i];
        ss << "  {\n"
           << "    \"ts\": " << pt.timestamp_sec << ",\n"
           << "    \"cpu\": " << pt.avg_cpu_pct << ",\n"
           << "    \"lat\": " << pt.avg_latency_us << ",\n"
           << "    \"drops\": " << pt.drop_count << "\n"
           << "  }" << (i + 1 < history_.size() ? ",\n" : "\n");
    }
    ss << "]";
    return ss.str();
}

} // namespace sentinel::nexus::storage
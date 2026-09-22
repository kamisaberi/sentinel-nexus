#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "telemetry.pb.h"

namespace sentinel::nexus::telemetry {

class VectorIngestQueue {
public:
    static VectorIngestQueue& instance() {
        static VectorIngestQueue inst;
        return inst;
    }

    void push(::sentinel::nexus::CandidateVector vector);
    bool pop(::sentinel::nexus::CandidateVector& out_vector, uint32_t wait_timeout_ms = 50);
    size_t size() const;
    void clear();

private:
    VectorIngestQueue() = default;

    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<::sentinel::nexus::CandidateVector> queue_;
    const size_t max_capacity_{200000};
};

} // namespace sentinel::nexus::telemetry
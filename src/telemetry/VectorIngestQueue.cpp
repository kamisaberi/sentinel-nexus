#include "VectorIngestQueue.hpp"
#include <chrono>

namespace sentinel::nexus::telemetry {

void VectorIngestQueue::push(::sentinel::nexus::CandidateVector vector) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (queue_.size() >= max_capacity_) {
        queue_.pop(); // Evict oldest if overloaded
    }
    queue_.push(std::move(vector));
    lock.unlock();
    cv_.notify_one();
}

bool VectorIngestQueue::pop(::sentinel::nexus::CandidateVector& out_vector, uint32_t wait_timeout_ms) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (queue_.empty()) {
        cv_.wait_for(lock, std::chrono::milliseconds(wait_timeout_ms), [this]() {
            return !queue_.empty();
        });
    }

    if (queue_.empty()) return false;

    out_vector = std::move(queue_.front());
    queue_.pop();
    return true;
}

size_t VectorIngestQueue::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

void VectorIngestQueue::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::queue<::sentinel::nexus::CandidateVector> empty;
    std::swap(queue_, empty);
}

} // namespace sentinel::nexus::telemetry
#pragma once
#include <thread>
#include <atomic>
#include <functional>

namespace sentinel::nexus::fleet {

class HeartbeatMonitor {
public:
    static HeartbeatMonitor& instance() {
        static HeartbeatMonitor inst;
        return inst;
    }

    void start(uint32_t timeout_seconds = 15, uint32_t check_interval_ms = 3000);
    void stop();

    bool is_running() const { return running_.load(); }

private:
    HeartbeatMonitor() = default;
    void monitor_loop(uint32_t timeout_seconds, uint32_t check_interval_ms);

    std::atomic<bool> running_{false};
    std::jthread worker_thread_;
};

} // namespace sentinel::nexus::fleet
#pragma once
#include <atomic>
#include <memory>
#include <string>
#include <chrono>

namespace sentinel::nexus::core {

class ServerContext {
public:
    static ServerContext& instance() {
        static ServerContext inst;
        return inst;
    }

    void set_running(bool running) { running_.store(running); }
    bool is_running() const { return running_.load(); }

    void record_boot_time() {
        boot_time_ = std::chrono::system_clock::now();
    }

    uint64_t uptime_seconds() const {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now - boot_time_).count();
    }

private:
    ServerContext() : running_(true) {
        record_boot_time();
    }

    std::atomic<bool> running_{true};
    std::chrono::system_clock::time_point boot_time_;
};

} // namespace sentinel::nexus::core
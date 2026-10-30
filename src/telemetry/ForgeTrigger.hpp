#pragma once
#include <string>
#include <mutex>
#include <atomic>
#include <thread>
#include <filesystem>
#include <chrono>

namespace sentinel::nexus::telemetry {

class ForgeTrigger {
public:
    static ForgeTrigger& instance() {
        static ForgeTrigger inst;
        return inst;
    }

    void initialize(const std::string& forge_cli_path = "/usr/local/bin/forge-cli",
                    size_t trigger_quota_samples = 50000,
                    const std::string& training_mode = "manual",
                    const std::string& remote_endpoint = "https://backend.aryorithm.com/api/v1/forge/train",
                    bool auto_trigger = false,
                    uint32_t cooldown_seconds = 1800);

    // Checks candidate sample volume and fires asynchronous retraining
    void evaluate_retraining_quota(size_t total_buffered_samples);

    // Explicit manual trigger (called by CLI 'aryo forge run')
    bool trigger_manual_training();

    bool is_training_active() const { return training_active_.load(); }
    std::string get_training_status_json() const;

private:
    ForgeTrigger() = default;
    void execute_forge_job();

    std::filesystem::path forge_cli_path_{"/usr/local/bin/forge-cli"};
    size_t trigger_quota_{50000};
    std::string training_mode_{"manual"};
    std::string remote_endpoint_{"https://backend.aryorithm.com/api/v1/forge/train"};
    bool auto_trigger_{false};
    uint32_t cooldown_seconds_{1800};

    std::atomic<bool> training_active_{false};
    std::chrono::steady_clock::time_point last_train_time_{};
    std::string last_trained_timestamp_{"Never"};
    uint32_t retraining_cycles_completed_{0};
    mutable std::mutex mutex_;
};

} // namespace sentinel::nexus::telemetry
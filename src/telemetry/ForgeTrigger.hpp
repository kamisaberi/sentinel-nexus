#pragma once
#include <string>
#include <mutex>
#include <atomic>
#include <thread>
#include <filesystem>

namespace sentinel::nexus::telemetry {

class ForgeTrigger {
public:
    static ForgeTrigger& instance() {
        static ForgeTrigger inst;
        return inst;
    }

    void initialize(const std::string& forge_cli_path = "/home/kami/blackbox-sentinel/xinfer-forge/deploy/run_adaptation.sh",
                    size_t trigger_quota_samples = 5000);

    // Checks candidate sample volume and fires asynchronous retraining
    void evaluate_retraining_quota(size_t total_buffered_samples);

    bool is_training_active() const { return training_active_.load(); }
    std::string get_training_status_json() const;

private:
    ForgeTrigger() = default;
    void execute_forge_job();

    std::filesystem::path forge_script_path_;
    size_t trigger_quota_{5000};
    std::atomic<bool> training_active_{false};
    std::string last_trained_timestamp_{"Never"};
    uint32_t retraining_cycles_completed_{0};
    mutable std::mutex mutex_;
};

} // namespace sentinel::nexus::telemetry
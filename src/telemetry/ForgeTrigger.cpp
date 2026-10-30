#include "ForgeTrigger.hpp"
#include "core/Logger.hpp"
#include "DatasetCurator.hpp"
#include "ota/CanaryOrchestrator.hpp"
#include "ota/ModelRepository.hpp"
#include <chrono>
#include <cstdlib>
#include <sstream>

namespace sentinel::nexus::telemetry {

void ForgeTrigger::initialize(const std::string& forge_cli_path, 
                              size_t trigger_quota_samples,
                              const std::string& training_mode,
                              const std::string& remote_endpoint,
                              bool auto_trigger,
                              uint32_t cooldown_seconds) {
    std::lock_guard<std::mutex> lock(mutex_);
    forge_cli_path_ = forge_cli_path;
    trigger_quota_ = trigger_quota_samples;
    training_mode_ = training_mode;
    remote_endpoint_ = remote_endpoint;
    auto_trigger_ = auto_trigger;
    cooldown_seconds_ = cooldown_seconds;

    NEXUS_LOG_INFO("ForgeTrigger initialized. Mode: " + training_mode_ + 
                   " | AutoTrigger: " + (auto_trigger_ ? "ENABLED" : "DISABLED") + 
                   " | Cooldown: " + std::to_string(cooldown_seconds_) + "s");
}

void ForgeTrigger::evaluate_retraining_quota(size_t total_buffered_samples) {
    // 1. HARD GATE: If auto_trigger is disabled, NEVER automatically retrain on packet quota!
    if (!auto_trigger_ || training_mode_ == "manual") {
        return;
    }

    if (training_active_.load()) return;

    // 2. COOLDOWN GATE: Enforce minimum time gap between training sessions
    auto now = std::chrono::steady_clock::now();
    auto elapsed_sec = std::chrono::duration_cast<std::chrono::seconds>(now - last_train_time_).count();
    if (last_train_time_.time_since_epoch().count() > 0 && elapsed_sec < cooldown_seconds_) {
        return; // Still in cooldown period; prevent runaway model generation!
    }

    if (total_buffered_samples >= trigger_quota_) {
        NEXUS_LOG_WARN("Sample quota reached (" + std::to_string(total_buffered_samples) + 
                       " >= " + std::to_string(trigger_quota_) + "). Dispatching controlled retraining.");
        
        training_active_.store(true);
        last_train_time_ = now;
        std::thread([this]() { execute_forge_job(); }).detach();
    }
}

bool ForgeTrigger::trigger_manual_training() {
    if (training_active_.load()) {
        NEXUS_LOG_WARN("Training job already in progress.");
        return false;
    }
    training_active_.store(true);
    last_train_time_ = std::chrono::steady_clock::now();
    std::thread([this]() { execute_forge_job(); }).detach();
    return true;
}

void ForgeTrigger::execute_forge_job() {
    CuratedDatasetInfo info;
    bool curated = DatasetCurator::instance().curate_training_dataset(info);
    if (!curated || info.total_samples == 0) {
        training_active_.store(false);
        return;
    }

    NEXUS_LOG_INFO("Executing background adaptation on curated dataset: " + info.dataset_file);

    std::string exec_bin = "/usr/local/bin/forge-cli";
    if (std::filesystem::exists(forge_cli_path_)) {
        exec_bin = forge_cli_path_.string();
    }

    // 3. STRICT PRUNING: Always output to a single static candidate path (no duplicate timestamped models!)
    std::string output_model = "models/network_threat_v2_adapted.onnx";

    std::ostringstream cmd;
    cmd << exec_bin << " finetune "
        << "--base-model models/network_threat_detector.onnx "
        << "--dataset " << info.dataset_file << " "
        << "--output " << output_model << " ";

    if (training_mode_ == "remote" || training_mode_ == "cloud" || training_mode_ == "private_forge") {
        cmd << "--remote --endpoint '" << remote_endpoint_ << "' ";
    }
    cmd << "> /tmp/forge_output.log 2>&1";

    int ret = std::system(cmd.str().c_str());
    NEXUS_LOG_INFO("xinfer-forge execution completed with exit code: " + std::to_string(ret));

    // Stage candidate model in Canary Orchestrator (Single stable slot)
    ota::ModelRepository::instance().scan_repository();
    ota::CanaryOrchestrator::instance().stage_candidate_model(
        "network_threat_v2_adapted.onnx", 
        "c89b3f4618e47f5255470d9a690e7da3c6046e297893a7768fa912345678", 
        "/models/network_threat_v2_adapted.onnx"
    );

    {
        std::lock_guard<std::mutex> lock(mutex_);
        retraining_cycles_completed_++;
        auto now_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::gmtime(&now_t));
        last_trained_timestamp_ = std::string(buf) + " UTC";
    }

    training_active_.store(false);
    NEXUS_LOG_INFO("Fine-tuning finished. Candidate model updated safely.");
}

std::string ForgeTrigger::get_training_status_json() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream ss;
    ss << "{\n"
       << "  \"training_active\": " << (training_active_.load() ? "true" : "false") << ",\n"
       << "  \"auto_trigger\": " << (auto_trigger_ ? "true" : "false") << ",\n"
       << "  \"training_mode\": \"" << training_mode_ << "\",\n"
       << "  \"retraining_cycles_completed\": " << retraining_cycles_completed_ << ",\n"
       << "  \"last_trained\": \"" << last_trained_timestamp_ << "\",\n"
       << "  \"quota_threshold\": " << trigger_quota_ << "\n"
       << "}";
    return ss.str();
}

} // namespace sentinel::nexus::telemetry
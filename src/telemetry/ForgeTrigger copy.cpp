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
                              const std::string& remote_endpoint) {
    std::lock_guard<std::mutex> lock(mutex_);
    forge_cli_path_ = forge_cli_path;
    trigger_quota_ = trigger_quota_samples;
    training_mode_ = training_mode;
    remote_endpoint_ = remote_endpoint;

    NEXUS_LOG_INFO("ForgeTrigger initialized. Mode: " + training_mode_ + 
                   " | Quota: " + std::to_string(trigger_quota_) + 
                   " -> Target CLI: " + forge_cli_path_.string());
}

void ForgeTrigger::evaluate_retraining_quota(size_t total_buffered_samples) {
    if (training_active_.load()) return;

    if (total_buffered_samples >= trigger_quota_) {
        NEXUS_LOG_WARN("Sample quota reached (" + std::to_string(total_buffered_samples) + 
                       " >= " + std::to_string(trigger_quota_) + "). Dispatching xinfer-forge adaptation!");
        
        training_active_.store(true);
        std::thread([this]() { execute_forge_job(); }).detach();
    }
}

void ForgeTrigger::execute_forge_job() {
    // 1. Curate latest batch to disk
    CuratedDatasetInfo info;
    bool curated = DatasetCurator::instance().curate_training_dataset(info);
    if (!curated || info.total_samples == 0) {
        training_active_.store(false);
        return;
    }

    NEXUS_LOG_INFO("Launching xinfer-forge job on dataset: " + info.dataset_file);

    // 2. Invoke forge-cli with --remote if configured
    std::string exec_bin = "/usr/local/bin/forge-cli";
    if (std::filesystem::exists(forge_cli_path_)) {
        exec_bin = forge_cli_path_.string();
    }

    std::ostringstream cmd;
    cmd << exec_bin << " finetune "
        << "--base-model models/network_threat_detector.onnx "
        << "--dataset " << info.dataset_file << " "
        << "--output models/network_threat_v2_adapted.onnx ";

    if (training_mode_ == "remote" || training_mode_ == "cloud" || training_mode_ == "private_forge") {
        cmd << "--remote --endpoint '" << remote_endpoint_ << "' ";
    }
    cmd << "> /tmp/forge_output.log 2>&1";

    NEXUS_LOG_INFO("Executing command: " + cmd.str());
    int ret = std::system(cmd.str().c_str());
    NEXUS_LOG_INFO("xinfer-forge execution finished with exit code: " + std::to_string(ret));

    // 3. Stage newly adapted candidate weights into Canary Orchestrator
    ota::ModelRepository::instance().scan_repository();
    ota::CanaryOrchestrator::instance().stage_candidate_model(
        "network_threat_v2_adapted.onnx", 
        "c89b3f4618e47f5255470d9a690e7da3c6046e297893a7768fa912345678", 
        "/models/network_threat_v2_adapted.onnx"
    );

    {
        std::lock_guard<std::mutex> lock(mutex_);
        retraining_cycles_completed_++;
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::gmtime(&now));
        last_trained_timestamp_ = std::string(buf) + " UTC";
    }

    training_active_.store(false);
    NEXUS_LOG_INFO("Candidate weights staged into CanaryOrchestrator in SHADOW_MODE.");
}

std::string ForgeTrigger::get_training_status_json() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream ss;
    ss << "{\n"
       << "  \"training_active\": " << (training_active_.load() ? "true" : "false") << ",\n"
       << "  \"training_mode\": \"" << training_mode_ << "\",\n"
       << "  \"remote_endpoint\": \"" << remote_endpoint_ << "\",\n"
       << "  \"retraining_cycles_completed\": " << retraining_cycles_completed_ << ",\n"
       << "  \"last_trained\": \"" << last_trained_timestamp_ << "\",\n"
       << "  \"quota_threshold\": " << trigger_quota_ << "\n"
       << "}";
    return ss.str();
}

} // namespace sentinel::nexus::telemetry
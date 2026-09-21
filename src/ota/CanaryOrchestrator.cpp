#include "CanaryOrchestrator.hpp"
#include "core/Logger.hpp"
#include <functional>

namespace sentinel::nexus::ota {

void CanaryOrchestrator::initialize(const std::string& stable_version, 
                                    const std::string& sha256, 
                                    const std::string& download_url) {
    std::lock_guard<std::mutex> lock(mutex_);
    stable_model_ = {
        .version = stable_version,
        .sha256_checksum = sha256,
        .download_url = download_url,
        .stage = ::sentinel::nexus::STAGE_FLEET_WIDE
    };
    candidate_active_ = false;
    NEXUS_LOG_INFO("CanaryOrchestrator initialized with stable model: " + stable_version);
}

void CanaryOrchestrator::stage_candidate_model(const std::string& version, 
                                              const std::string& sha256, 
                                              const std::string& download_url) {
    std::lock_guard<std::mutex> lock(mutex_);
    candidate_model_ = {
        .version = version,
        .sha256_checksum = sha256,
        .download_url = download_url,
        .stage = ::sentinel::nexus::STAGE_SHADOW_MODE
    };
    candidate_active_ = true;
    canary_cohort_.clear();

    NEXUS_LOG_WARN("New candidate model staged: " + version + " [Stage: SHADOW_MODE]");
}

void CanaryOrchestrator::advance_rollout_stage(::sentinel::nexus::RolloutStage new_stage) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!candidate_active_) {
        NEXUS_LOG_WARN("Cannot advance rollout stage: No candidate model active.");
        return;
    }

    candidate_model_.stage = new_stage;

    if (new_stage == ::sentinel::nexus::STAGE_FLEET_WIDE) {
        // Promote candidate to stable
        stable_model_ = candidate_model_;
        candidate_active_ = false;
        canary_cohort_.clear();
        NEXUS_LOG_INFO("Candidate model fully promoted to FLEET_WIDE stable: " + stable_model_.version);
    } else {
        NEXUS_LOG_INFO("Candidate model advanced to stage: " + std::to_string(static_cast<int>(new_stage)));
    }
}

void CanaryOrchestrator::trigger_emergency_rollback() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!candidate_active_) return;

    NEXUS_LOG_CRIT("EMERGENCY ROLLBACK TRIGGERED! Purging candidate model " + candidate_model_.version);
    candidate_active_ = false;
    canary_cohort_.clear();
}

bool CanaryOrchestrator::evaluate_appliance_update(const std::string& node_id,
                                                   const std::string& current_version,
                                                   ::sentinel::nexus::ModelPollResponse* response) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Default: Check if node needs the baseline stable model
    if (!candidate_active_) {
        if (current_version != stable_model_.version && !stable_model_.version.empty()) {
            response->set_update_required(true);
            response->set_target_version(stable_model_.version);
            response->set_model_sha256(stable_model_.sha256_checksum);
            response->set_download_url(stable_model_.download_url);
            response->set_stage(::sentinel::nexus::STAGE_FLEET_WIDE);
            return true;
        }
        response->set_update_required(false);
        return false;
    }

    // Candidate model active: Determine if node belongs to rollout group
    bool assign_candidate = false;

    if (candidate_model_.stage == ::sentinel::nexus::STAGE_SHADOW_MODE) {
        // Shadow mode: Edge runs inference passively without dropping packets
        assign_candidate = true;
    } else if (candidate_model_.stage == ::sentinel::nexus::STAGE_CANARY_5_PCT) {
        // Deterministic hash cohort (5% of nodes)
        size_t hash_val = std::hash<std::string>{}(node_id);
        if ((hash_val % 100) < 5) {
            assign_candidate = true;
            canary_cohort_.insert(node_id);
        }
    } else if (candidate_model_.stage == ::sentinel::nexus::STAGE_FLEET_WIDE) {
        assign_candidate = true;
    }

    if (assign_candidate && current_version != candidate_model_.version) {
        response->set_update_required(true);
        response->set_target_version(candidate_model_.version);
        response->set_model_sha256(candidate_model_.sha256_checksum);
        response->set_download_url(candidate_model_.download_url);
        response->set_stage(candidate_model_.stage);
        return true;
    }

    response->set_update_required(false);
    return false;
}

std::string CanaryOrchestrator::get_stable_version() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return stable_model_.version;
}

std::string CanaryOrchestrator::get_candidate_version() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return candidate_active_ ? candidate_model_.version : "None";
}

::sentinel::nexus::RolloutStage CanaryOrchestrator::get_current_stage() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return candidate_active_ ? candidate_model_.stage : ::sentinel::nexus::STAGE_FLEET_WIDE;
}

} // namespace sentinel::nexus::ota
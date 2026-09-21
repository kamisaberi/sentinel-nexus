#pragma once
#include <string>
#include <mutex>
#include <vector>
#include <unordered_set>
#include "model_ota.pb.h"

namespace sentinel::nexus::ota {

struct ModelMetadata {
    std::string version;
    std::string sha256_checksum;
    std::string download_url;
    ::sentinel::nexus::RolloutStage stage;
};

class CanaryOrchestrator {
public:
    static CanaryOrchestrator& instance() {
        static CanaryOrchestrator inst;
        return inst;
    }

    void initialize(const std::string& stable_version, const std::string& sha256, const std::string& download_url);

    // Staging controls
    void stage_candidate_model(const std::string& version, const std::string& sha256, const std::string& download_url);
    void advance_rollout_stage(::sentinel::nexus::RolloutStage new_stage);
    void trigger_emergency_rollback();

    // Query for appliances
    bool evaluate_appliance_update(const std::string& node_id,
                                   const std::string& current_version,
                                   ::sentinel::nexus::ModelPollResponse* response);

    std::string get_stable_version() const;
    std::string get_candidate_version() const;
    ::sentinel::nexus::RolloutStage get_current_stage() const;

private:
    CanaryOrchestrator() = default;

    mutable std::mutex mutex_;
    ModelMetadata stable_model_;
    ModelMetadata candidate_model_;
    bool candidate_active_{false};

    // Track which nodes are assigned to the 5% canary cohort
    std::unordered_set<std::string> canary_cohort_;
};

} // namespace sentinel::nexus::ota
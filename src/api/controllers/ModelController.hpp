#pragma once
#include <string>
#include <sstream>
#include "ota/CanaryOrchestrator.hpp"
#include "ota/ModelRepository.hpp"

namespace sentinel::nexus::api::controllers {

class ModelController {
public:
    static std::string get_ota_status_json() {
        auto& ota = ota::CanaryOrchestrator::instance();
        std::ostringstream ss;
        ss << "{\n"
           << "  \"stable_version\": \"" << ota.get_stable_version() << "\",\n"
           << "  \"candidate_version\": \"" << ota.get_candidate_version() << "\",\n"
           << "  \"stage\": " << static_cast<int>(ota.get_current_stage()) << "\n"
           << "}";
        return ss.str();
    }

    static std::string stage_candidate_json(const std::string& version = "network_threat_v2.onnx") {
        (void)version;
        ota::CanaryOrchestrator::instance().stage_candidate_model(
            "network_threat_v2.onnx", 
            "8fa9c89b3f4618e47f5255470d9a690e7da3c6046e297893a776", 
            "/models/v2.onnx");
        return "{\"status\": \"candidate_staged\", \"stage\": \"SHADOW_MODE\"}";
    }

    static std::string advance_stage_json() {
        auto& ota = ota::CanaryOrchestrator::instance();
        auto cur = ota.get_current_stage();
        if (cur == ::sentinel::nexus::STAGE_SHADOW_MODE) {
            ota.advance_rollout_stage(::sentinel::nexus::STAGE_CANARY_5_PCT);
            return "{\"status\": \"advanced\", \"stage\": \"CANARY_5_PCT\"}";
        } else if (cur == ::sentinel::nexus::STAGE_CANARY_5_PCT) {
            ota.advance_rollout_stage(::sentinel::nexus::STAGE_FLEET_WIDE);
            return "{\"status\": \"promoted\", \"stage\": \"FLEET_WIDE\"}";
        }
        return "{\"status\": \"no_action\"}";
    }

    static std::string rollback_json() {
        ota::CanaryOrchestrator::instance().trigger_emergency_rollback();
        return "{\"status\": \"emergency_rollback_executed\"}";
    }

    static std::string list_models_json() {
        return ota::ModelRepository::instance().generate_models_json();
    }
};

} // namespace sentinel::nexus::api::controllers
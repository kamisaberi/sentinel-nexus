#include "ModelOtaServiceImpl.hpp"
#include "ota/CanaryOrchestrator.hpp"
#include "core/Logger.hpp"

namespace sentinel::nexus::rpc {

grpc::Status ModelOtaServiceImpl::PollTargetModel(grpc::ServerContext* context, 
                                                 const ::sentinel::nexus::ModelPollRequest* request, 
                                                 ::sentinel::nexus::ModelPollResponse* response) {
    (void)context;

    bool update_ready = ota::CanaryOrchestrator::instance().evaluate_appliance_update(
        request->node_id(), request->active_model_version(), response);

    if (update_ready) {
        NEXUS_LOG_INFO("Serving OTA update instructions to node " + request->node_id() + 
                       " -> Target Version: " + response->target_version());
    }

    return grpc::Status::OK;
}

} // namespace sentinel::nexus::rpc
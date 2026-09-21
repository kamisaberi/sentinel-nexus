#include "FleetServiceImpl.hpp"
#include "fleet/NodeRegistry.hpp"
#include "core/Logger.hpp"

namespace sentinel::nexus::rpc {

grpc::Status FleetServiceImpl::RegisterAppliance(grpc::ServerContext* context, 
                                                const ::sentinel::nexus::RegistrationRequest* request, 
                                                ::sentinel::nexus::RegistrationResponse* response) {
    (void)context;
    
    if (request->identity().machine_uuid().empty() && request->identity().hostname().empty()) {
        response->mutable_status()->set_success(false);
        response->mutable_status()->set_message("Invalid Hardware Identity.");
        response->mutable_status()->set_code(400);
        return grpc::Status::OK;
    }

    std::string assigned_id = fleet::NodeRegistry::instance().register_node(*request);

    response->mutable_status()->set_success(true);
    response->mutable_status()->set_message("Appliance registered successfully.");
    response->mutable_status()->set_code(200);
    response->set_node_id(assigned_id);
    response->set_heartbeat_interval_seconds(5);

    return grpc::Status::OK;
}

grpc::Status FleetServiceImpl::SendHeartbeat(grpc::ServerContext* context, 
                                            const ::sentinel::nexus::HeartbeatRequest* request, 
                                            ::sentinel::nexus::HeartbeatResponse* response) {
    (void)context;

    bool updated = fleet::NodeRegistry::instance().update_heartbeat(
        request->node_id(), request->metrics());

    if (!updated) {
        response->set_acknowledged(false);
        response->set_config_drift_detected(true);
        return grpc::Status::OK;
    }

    response->set_acknowledged(true);
    response->set_config_drift_detected(false);
    response->set_model_update_available(false);

    return grpc::Status::OK;
}

} // namespace sentinel::nexus::rpc
#pragma once
#include <grpcpp/grpcpp.h>
#include "fleet.grpc.pb.h"

namespace sentinel::nexus::rpc {

class FleetServiceImpl final : public ::sentinel::nexus::FleetService::Service {
public:
    grpc::Status RegisterAppliance(grpc::ServerContext* context, 
                                   const ::sentinel::nexus::RegistrationRequest* request, 
                                   ::sentinel::nexus::RegistrationResponse* response) override;

    grpc::Status SendHeartbeat(grpc::ServerContext* context, 
                              const ::sentinel::nexus::HeartbeatRequest* request, 
                              ::sentinel::nexus::HeartbeatResponse* response) override;

    grpc::Status DeregisterAppliance(grpc::ServerContext* context,
                                    const ::sentinel::nexus::DeregistrationRequest* request,
                                    ::sentinel::nexus::ResponseStatus* response) override;
};

} // namespace sentinel::nexus::rpc
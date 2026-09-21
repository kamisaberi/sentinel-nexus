#pragma once
#include <grpcpp/grpcpp.h>
#include "fleet.grpc.pb.h"

namespace sentinel::nexus::rpc {

class FleetServiceImpl final : public fleet::FleetService::Service {
public:
    grpc::Status RegisterAppliance(grpc::ServerContext* context, 
                                   const fleet::RegistrationRequest* request, 
                                   fleet::RegistrationResponse* response) override;

    grpc::Status SendHeartbeat(grpc::ServerContext* context, 
                              const fleet::HeartbeatRequest* request, 
                              fleet::HeartbeatResponse* response) override;
};

} // namespace sentinel::nexus::rpc
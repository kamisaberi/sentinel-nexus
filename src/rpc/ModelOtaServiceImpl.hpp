#pragma once
#include <grpcpp/grpcpp.h>
#include "model_ota.grpc.pb.h"

namespace sentinel::nexus::rpc {

class ModelOtaServiceImpl final : public ::sentinel::nexus::ModelOtaService::Service {
public:
    grpc::Status PollTargetModel(grpc::ServerContext* context, 
                                const ::sentinel::nexus::ModelPollRequest* request, 
                                ::sentinel::nexus::ModelPollResponse* response) override;
};

} // namespace sentinel::nexus::rpc
#pragma once
#include <grpcpp/grpcpp.h>
#include "intelligence.grpc.pb.h"

namespace sentinel::nexus::rpc {

class IntelligenceServiceImpl final : public intelligence::IntelligenceService::Service {
public:
    grpc::Status SyncCollectiveImmunity(
        grpc::ServerContext* context, 
        grpc::ServerReaderWriter<intelligence::FleetDefenseRule, intelligence::ThreatIndicator>* stream) override;
};

} // namespace sentinel::nexus::rpc
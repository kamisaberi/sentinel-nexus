#pragma once
#include <grpcpp/grpcpp.h>
#include "intelligence.grpc.pb.h"

namespace sentinel::nexus::rpc {

class IntelligenceServiceImpl final : public ::sentinel::nexus::IntelligenceService::Service {
public:
    grpc::Status SyncCollectiveImmunity(
        grpc::ServerContext* context, 
        grpc::ServerReaderWriter<::sentinel::nexus::FleetDefenseRule, ::sentinel::nexus::ThreatIndicator>* stream) override;
};

} // namespace sentinel::nexus::rpc
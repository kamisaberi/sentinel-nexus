#pragma once
#include <grpcpp/grpcpp.h>
#include "telemetry.grpc.pb.h"

namespace sentinel::nexus::rpc {

class TelemetryServiceImpl final : public ::sentinel::nexus::TelemetryService::Service {
public:
    grpc::Status StreamCandidateVectors(
        grpc::ServerContext* context, 
        grpc::ServerReader<::sentinel::nexus::FeatureVectorStream>* reader, 
        ::sentinel::nexus::IngestSummary* response) override;
};

} // namespace sentinel::nexus::rpc
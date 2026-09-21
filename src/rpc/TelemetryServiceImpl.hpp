#pragma once
#include <grpcpp/grpcpp.h>
#include "telemetry.grpc.pb.h"

namespace sentinel::nexus::rpc {

class TelemetryServiceImpl final : public telemetry::TelemetryService::Service {
public:
    grpc::Status StreamCandidateVectors(
        grpc::ServerContext* context, 
        grpc::ServerReader<telemetry::FeatureVectorStream>* reader, 
        telemetry::IngestSummary* response) override;
};

} // namespace sentinel::nexus::rpc
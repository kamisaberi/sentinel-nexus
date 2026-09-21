#include "TelemetryServiceImpl.hpp"
#include "telemetry/ForgeBridge.hpp"
#include "core/Logger.hpp"

namespace sentinel::nexus::rpc {

grpc::Status TelemetryServiceImpl::StreamCandidateVectors(
    grpc::ServerContext* context, 
    grpc::ServerReader<::sentinel::nexus::FeatureVectorStream>* reader, 
    ::sentinel::nexus::IngestSummary* response) {
    (void)context;

    ::sentinel::nexus::FeatureVectorStream stream_batch;
    uint64_t total_received = 0;
    uint64_t total_routed_to_forge = 0;

    auto& bridge = telemetry::ForgeBridge::instance();

    while (reader->Read(&stream_batch)) {
        for (const auto& vector : stream_batch.vectors()) {
            total_received++;
            if (bridge.ingest_vector(vector)) {
                total_routed_to_forge++;
            }
        }
    }

    response->set_total_received(total_received);
    response->set_routed_to_forge(total_routed_to_forge);
    response->set_throttled(false);

    return grpc::Status::OK;
}

} // namespace sentinel::nexus::rpc
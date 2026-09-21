#include "IntelligenceServiceImpl.hpp"
#include "intelligence/IocBroadcaster.hpp"
#include "core/Logger.hpp"

namespace sentinel::nexus::rpc {

grpc::Status IntelligenceServiceImpl::SyncCollectiveImmunity(
    grpc::ServerContext* context, 
    grpc::ServerReaderWriter<intelligence::FleetDefenseRule, intelligence::ThreatIndicator>* stream) {
    (void)context;

    intelligence::ThreatIndicator incoming_threat;
    std::string assigned_node_id;

    // Read initial frame to identify node
    if (stream->Read(&incoming_threat)) {
        assigned_node_id = incoming_threat.origin_node_id();
        intelligence::IocBroadcaster::instance().register_subscriber(assigned_node_id, stream);
        
        // Broadcast the initial threat if it contains payload
        if (!incoming_threat.attacker_ip().empty()) {
            intelligence::IocBroadcaster::instance().broadcast_threat(incoming_threat);
        }

        // Loop and stream continuously
        while (stream->Read(&incoming_threat)) {
            if (!incoming_threat.attacker_ip().empty()) {
                intelligence::IocBroadcaster::instance().broadcast_threat(incoming_threat);
            }
        }
    }

    if (!assigned_node_id.empty()) {
        intelligence::IocBroadcaster::instance().unregister_subscriber(assigned_node_id);
    }

    return grpc::Status::OK;
}

} // namespace sentinel::nexus::rpc
#include "IntelligenceServiceImpl.hpp"
#include "intelligence/IocBroadcaster.hpp"
#include "core/Logger.hpp"

namespace sentinel::nexus::rpc {

grpc::Status IntelligenceServiceImpl::SyncCollectiveImmunity(
    grpc::ServerContext* context, 
    grpc::ServerReaderWriter<::sentinel::nexus::FleetDefenseRule, ::sentinel::nexus::ThreatIndicator>* stream) {
    (void)context;

    ::sentinel::nexus::ThreatIndicator incoming_threat;
    std::string assigned_node_id;

    if (stream->Read(&incoming_threat)) {
        assigned_node_id = incoming_threat.origin_node_id();
        intelligence::IocBroadcaster::instance().register_subscriber(assigned_node_id, stream);
        
        if (!incoming_threat.attacker_ip().empty()) {
            intelligence::IocBroadcaster::instance().broadcast_threat(incoming_threat);
        }

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
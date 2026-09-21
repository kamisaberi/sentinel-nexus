#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <queue>
#include <grpcpp/grpcpp.h>
#include "intelligence.grpc.pb.h"

namespace sentinel::nexus::intelligence {

class IocBroadcaster {
public:
    static IocBroadcaster& instance() {
        static IocBroadcaster inst;
        return inst;
    }

    using StreamContext = grpc::ServerReaderWriter<FleetDefenseRule, ThreatIndicator>;

    // Registers an appliance's live bidirectional intelligence stream
    void register_subscriber(const std::string& node_id, StreamContext* stream);
    void unregister_subscriber(const std::string& node_id);

    // Broadcasts an attack fingerprint to every connected appliance in the fleet (< 50ms)
    void broadcast_threat(const ThreatIndicator& threat, uint64_t ttl_seconds = 86400);

    size_t subscriber_count() const;

private:
    IocBroadcaster() = default;

    mutable std::mutex mutex_;
    std::unordered_map<std::string, StreamContext*> subscribers_;
    uint64_t rule_counter_{1000};
};

} // namespace sentinel::nexus::intelligence
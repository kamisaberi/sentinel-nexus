#pragma once
#include <string>
#include "intelligence/IocBroadcaster.hpp"
#include "intelligence/GlobalThreatCache.hpp"
#include "intelligence/MitreAggregator.hpp"
#include "api/TelemetryStreamer.hpp"

namespace sentinel::nexus::api::controllers {

class ThreatController {
public:
    static std::string broadcast_threat_json(const std::string& body) {
        std::string ip;
        size_t pos = body.find("\"ip\"");
        if (pos != std::string::npos) {
            size_t colon = body.find(':', pos);
            size_t start = body.find('"', colon + 1);
            size_t end = body.find('"', start + 1);
            if (start != std::string::npos && end != std::string::npos) {
                ip = body.substr(start + 1, end - start - 1);
            }
        }

        if (!ip.empty()) {
            ::sentinel::nexus::ThreatIndicator threat;
            threat.set_origin_node_id("NEXUS-ADMIN-CONSOLE");
            threat.set_attacker_ip(ip);
            threat.set_type(::sentinel::nexus::THREAT_EXPLOIT_PAYLOAD);
            threat.set_confidence(1.0f);

            intelligence::IocBroadcaster::instance().broadcast_threat(threat);
            intelligence::MitreAggregator::instance().record_attack(threat.type(), ip);
            TelemetryStreamer::instance().broadcast_event(
                "threat_broadcast", "{\"target_ip\":\"" + ip + "\"}");

            return "{\"status\": \"broadcast_dispatched\", \"target_ip\": \"" + ip + "\"}";
        }

        return "{\"status\": \"error\", \"message\": \"invalid IP address\"}";
    }

    static std::string get_mitre_summary_json() {
        return intelligence::MitreAggregator::instance().generate_matrix_json();
    }
};

} // namespace sentinel::nexus::api::controllers
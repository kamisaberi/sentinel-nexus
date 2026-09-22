#pragma once
#include <string>
#include <sstream>
#include "fleet/NodeRegistry.hpp"
#include "fleet/GroupManager.hpp"

namespace sentinel::nexus::api::controllers {

class FleetController {
public:
    static std::string get_nodes_json() {
        auto nodes = fleet::NodeRegistry::instance().get_all_nodes();
        std::ostringstream ss;
        ss << "[\n";
        for (size_t i = 0; i < nodes.size(); ++i) {
            const auto& n = nodes[i];
            ss << "  {\n"
               << "    \"node_id\": \"" << n.node_id << "\",\n"
               << "    \"site\": \"" << n.site_identifier << "\",\n"
               << "    \"hostname\": \"" << n.identity.hostname() << "\",\n"
               << "    \"kernel\": \"" << n.identity.kernel_version() << "\",\n"
               << "    \"status\": \"" << (n.status == fleet::NodeHealthStatus::ONLINE ? "ONLINE" : "OFFLINE") << "\",\n"
               << "    \"cpu_pct\": " << n.latest_metrics.cpu_usage_pct() << ",\n"
               << "    \"ram_mb\": " << n.latest_metrics.ram_usage_mb() << ",\n"
               << "    \"npu_temp_c\": " << n.latest_metrics.npu_gpu_temp_celsius() << ",\n"
               << "    \"packets_inspected\": " << n.latest_metrics.packets_inspected() << ",\n"
               << "    \"ebpf_drops\": " << n.latest_metrics.ebpf_packets_dropped() << ",\n"
               << "    \"mitigation_latency_us\": " << n.latest_metrics.avg_mitigation_latency_us() << "\n"
               << "  }" << (i + 1 < nodes.size() ? ",\n" : "\n");
        }
        ss << "]";
        return ss.str();
    }

    static std::string get_groups_json() {
        return fleet::GroupManager::instance().generate_groups_json();
    }
};

} // namespace sentinel::nexus::api::controllers
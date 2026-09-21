#include "StateDatabase.hpp"
#include "core/Logger.hpp"
#include <fstream>
#include <sstream>

namespace sentinel::nexus::storage {

bool StateDatabase::initialize(const std::string& database_path) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    db_path_ = database_path;

    std::error_code ec;
    std::filesystem::create_directories(db_path_.parent_path(), ec);
    if (ec) {
        NEXUS_LOG_ERROR("Failed to create state storage directory: " + ec.message());
        return false;
    }

    NEXUS_LOG_INFO("StateDatabase initialized at: " + db_path_.string());
    return true;
}

bool StateDatabase::save_fleet_state(const std::vector<fleet::RegisteredNode>& nodes) {
    std::lock_guard<std::mutex> lock(db_mutex_);

    std::ofstream file(db_path_, std::ios::trunc);
    if (!file.is_open()) {
        NEXUS_LOG_ERROR("Could not open state DB for writing: " + db_path_.string());
        return false;
    }

    file << "[\n";
    for (size_t i = 0; i < nodes.size(); ++i) {
        const auto& n = nodes[i];
        file << "  {\n"
             << "    \"node_id\": \"" << n.node_id << "\",\n"
             << "    \"site\": \"" << n.site_identifier << "\",\n"
             << "    \"hostname\": \"" << n.identity.hostname() << "\",\n"
             << "    \"kernel\": \"" << n.identity.kernel_version() << "\",\n"
             << "    \"uuid\": \"" << n.identity.machine_uuid() << "\",\n"
             << "    \"backend\": " << static_cast<int>(n.identity.primary_backend()) << ",\n"
             << "    \"packets_inspected\": " << n.latest_metrics.packets_inspected() << ",\n"
             << "    \"ebpf_drops\": " << n.latest_metrics.ebpf_packets_dropped() << "\n"
             << "  }" << (i + 1 < nodes.size() ? ",\n" : "\n");
    }
    file << "]\n";

    return true;
}

bool StateDatabase::load_fleet_state(std::vector<fleet::RegisteredNode>& out_nodes) {
    std::lock_guard<std::mutex> lock(db_mutex_);
    if (!std::filesystem::exists(db_path_)) {
        return false;
    }

    std::ifstream file(db_path_);
    if (!file.is_open()) return false;

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (content.empty() || content == "[]\n") return false;

    // Fast parser for saved state records
    size_t pos = 0;
    while ((pos = content.find("{\"node_id\":", pos)) != std::string::npos ||
           (pos = content.find("\"node_id\":", pos)) != std::string::npos) {
        
        auto extract_val = [&](const std::string& key) -> std::string {
            size_t k = content.find("\"" + key + "\":", pos);
            if (k == std::string::npos) return "";
            size_t quote_start = content.find('"', k + key.size() + 3);
            if (quote_start == std::string::npos) return "";
            size_t quote_end = content.find('"', quote_start + 1);
            if (quote_end == std::string::npos) return "";
            return content.substr(quote_start + 1, quote_end - quote_start - 1);
        };

        std::string node_id = extract_val("node_id");
        if (!node_id.empty()) {
            fleet::RegisteredNode node{};
            node.node_id = node_id;
            node.site_identifier = extract_val("site");
            node.identity.set_hostname(extract_val("hostname"));
            node.identity.set_kernel_version(extract_val("kernel"));
            node.identity.set_machine_uuid(extract_val("uuid"));
            node.status = fleet::NodeHealthStatus::OFFLINE; // Restored as offline until first heartbeat
            out_nodes.push_back(std::move(node));
        }

        pos += 10;
        size_t next_obj = content.find('{', pos);
        if (next_obj == std::string::npos) break;
        pos = next_obj;
    }

    NEXUS_LOG_INFO("Restored " + std::to_string(out_nodes.size()) + " appliance identities from disk.");
    return true;
}

} // namespace sentinel::nexus::storage
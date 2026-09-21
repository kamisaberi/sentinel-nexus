#include "GroupManager.hpp"
#include "core/Logger.hpp"
#include <sstream>

namespace sentinel::nexus::fleet {

GroupManager::GroupManager() {
    // Default system groups
    create_group("DEFAULT_DMZ", "Enterprise perimeter and general servers", false, 1000.0f);
    create_group("CRITICAL_OT", "Industrial SCADA/ICS isolated enclaves", true, 800.0f);
    create_group("HEALTHCARE_PACS", "Medical imaging and patient telemetry", false, 950.0f);
}

void GroupManager::create_group(const std::string& group_id, 
                                const std::string& description, 
                                bool scada_mode, 
                                float max_latency_us) {
    std::unique_lock lock(mutex_);
    groups_[group_id] = ApplianceGroup{
        .group_id = group_id,
        .description = description,
        .scada_mode_enabled = scada_mode,
        .max_allowed_latency_us = max_latency_us,
        .member_node_ids = {}
    };
    NEXUS_LOG_INFO("Configured appliance group: " + group_id + " [" + description + "]");
}

void GroupManager::assign_node_to_group(const std::string& node_id, const std::string& group_id) {
    std::unique_lock lock(mutex_);
    auto it = groups_.find(group_id);
    if (it == groups_.end()) {
        NEXUS_LOG_WARN("Cannot assign node " + node_id + " to non-existent group: " + group_id);
        return;
    }

    // Remove from previous group if present
    auto prev_it = node_to_group_map_.find(node_id);
    if (prev_it != node_to_group_map_.end()) {
        groups_[prev_it->second].member_node_ids.erase(node_id);
    }

    it->second.member_node_ids.insert(node_id);
    node_to_group_map_[node_id] = group_id;
    NEXUS_LOG_INFO("Node " + node_id + " assigned to group: " + group_id);
}

void GroupManager::remove_node_from_group(const std::string& node_id, const std::string& group_id) {
    std::unique_lock lock(mutex_);
    auto it = groups_.find(group_id);
    if (it != groups_.end()) {
        it->second.member_node_ids.erase(node_id);
    }
    node_to_group_map_.erase(node_id);
}

std::string GroupManager::get_node_group(const std::string& node_id) const {
    std::shared_lock lock(mutex_);
    auto it = node_to_group_map_.find(node_id);
    return (it != node_to_group_map_.end()) ? it->second : "DEFAULT_DMZ";
}

std::vector<std::string> GroupManager::get_group_members(const std::string& group_id) const {
    std::shared_lock lock(mutex_);
    auto it = groups_.find(group_id);
    if (it == groups_.end()) return {};

    return std::vector<std::string>(it->second.member_node_ids.begin(), it->second.member_node_ids.end());
}

std::string GroupManager::generate_groups_json() const {
    std::shared_lock lock(mutex_);
    std::ostringstream ss;
    ss << "[\n";
    size_t i = 0;
    for (const auto& [id, grp] : groups_) {
        ss << "  {\n"
           << "    \"group_id\": \"" << grp.group_id << "\",\n"
           << "    \"description\": \"" << grp.description << "\",\n"
           << "    \"scada_mode\": " << (grp.scada_mode_enabled ? "true" : "false") << ",\n"
           << "    \"max_latency_us\": " << grp.max_allowed_latency_us << ",\n"
           << "    \"node_count\": " << grp.member_node_ids.size() << "\n"
           << "  }" << (++i < groups_.size() ? ",\n" : "\n");
    }
    ss << "]";
    return ss.str();
}

} // namespace sentinel::nexus::fleet
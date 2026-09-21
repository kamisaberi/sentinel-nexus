#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>

namespace sentinel::nexus::fleet {

struct ApplianceGroup {
    std::string group_id;
    std::string description;
    bool scada_mode_enabled;
    float max_allowed_latency_us;
    std::unordered_set<std::string> member_node_ids;
};

class GroupManager {
public:
    static GroupManager& instance() {
        static GroupManager inst;
        return inst;
    }

    void create_group(const std::string& group_id, 
                      const std::string& description, 
                      bool scada_mode = false, 
                      float max_latency_us = 1000.0f);

    void assign_node_to_group(const std::string& node_id, const std::string& group_id);
    void remove_node_from_group(const std::string& node_id, const std::string& group_id);

    std::string get_node_group(const std::string& node_id) const;
    std::vector<std::string> get_group_members(const std::string& group_id) const;
    std::string generate_groups_json() const;

private:
    GroupManager();

    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, ApplianceGroup> groups_;
    std::unordered_map<std::string, std::string> node_to_group_map_;
};

} // namespace sentinel::nexus::fleet
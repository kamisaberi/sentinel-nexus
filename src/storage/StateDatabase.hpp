#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <filesystem>
#include "fleet/NodeRegistry.hpp"

namespace sentinel::nexus::storage {

class StateDatabase {
public:
    static StateDatabase& instance() {
        static StateDatabase inst;
        return inst;
    }

    bool initialize(const std::string& database_path = "data/nexus_state.json");
    bool save_fleet_state(const std::vector<fleet::RegisteredNode>& nodes);
    bool load_fleet_state(std::vector<fleet::RegisteredNode>& out_nodes);

private:
    StateDatabase() = default;

    std::filesystem::path db_path_;
    mutable std::mutex db_mutex_;
};

} // namespace sentinel::nexus::storage
#include "ConfigManager.hpp"
#include "Logger.hpp"
#include <fstream>
#include <sstream>

namespace sentinel::nexus::core {

bool ConfigManager::load_config(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        NEXUS_LOG_WARN("Could not open " + config_path + ", falling back to internal defaults.");
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, ':')) {
            std::string value;
            if (std::getline(is_line, value)) {
                // Trim leading/trailing whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t\""));
                value.erase(value.find_last_not_of(" \t\"") + 1);

                if (key == "bind_address") config_.bind_address = value;
                else if (key == "grpc_port") config_.grpc_port = std::stoi(value);
                else if (key == "rest_api_port") config_.rest_port = std::stoi(value);
                else if (key == "ws_stream_port") config_.ws_port = std::stoi(value);
                else if (key == "worker_threads") config_.worker_threads = std::stoi(value);
            }
        }
    }

    NEXUS_LOG_INFO("Config loaded successfully from: " + config_path);
    return true;
}

} // namespace sentinel::nexus::core
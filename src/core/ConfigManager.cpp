#include "ConfigManager.hpp"
#include "Logger.hpp"
#include <fstream>
#include <sstream>

namespace sentinel::nexus::core
{

    bool ConfigManager::load_config(const std::string &config_path)
    {
        std::ifstream file(config_path);
        if (!file.is_open())
        {
            NEXUS_LOG_WARN("Could not open " + config_path + ", falling back to internal defaults.");
            return false;
        }

        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            std::istringstream is_line(line);
            std::string key;
            if (std::getline(is_line, key, ':'))
            {
                std::string value;
                if (std::getline(is_line, value))
                {
                    // 1. Strip any inline comment starting with '#'
                    size_t comment_pos = value.find('#');
                    if (comment_pos != std::string::npos)
                    {
                        value = value.substr(0, comment_pos);
                    }

                    // 2. Trim whitespace and quotation marks
                    size_t k_start = key.find_first_not_of(" \t");
                    size_t k_end = key.find_last_not_of(" \t");
                    if (k_start != std::string::npos && k_end != std::string::npos)
                    {
                        key = key.substr(k_start, k_end - k_start + 1);
                    }

                    size_t v_start = value.find_first_not_of(" \t\"");
                    size_t v_end = value.find_last_not_of(" \t\"");
                    if (v_start != std::string::npos && v_end != std::string::npos)
                    {
                        value = value.substr(v_start, v_end - v_start + 1);
                    }
                    else
                    {
                        value = "";
                    }

                    // if (key == "bind_address")
                    //     config_.bind_address = value;
                    // else if (key == "grpc_port")
                    //     config_.grpc_port = std::stoi(value);
                    // else if (key == "rest_api_port")
                    //     config_.rest_port = std::stoi(value);
                    // else if (key == "ws_stream_port")
                    //     config_.ws_port = std::stoi(value);
                    // else if (key == "worker_threads")
                    //     config_.worker_threads = std::stoi(value);
                    // else if (key == "cloud_endpoint")
                    //     config_.saas_endpoint = value;
                    // else if (key == "tenant_id")
                    //     config_.saas_tenant_id = value;
                    // else if (key == "auth_email")
                    //     config_.saas_auth_email = value;
                    // else if (key == "auth_password")
                    //     config_.saas_auth_password = value;
                    // else if (key == "token_storage_path")
                    //     config_.saas_token_path = value;
                    // else if (key == "sync_interval_sec")
                    //     config_.saas_sync_interval = std::stoi(value);
                    // else if (key == "enabled" && value == "true")
                    //     config_.saas_enabled = true;
                    if (key == "bind_address")
                        config_.bind_address = value;
                    else if (key == "grpc_port")
                        config_.grpc_port = std::stoi(value);
                    else if (key == "rest_api_port")
                        config_.rest_port = std::stoi(value);
                    else if (key == "ws_stream_port")
                        config_.ws_port = std::stoi(value);
                    else if (key == "worker_threads")
                        config_.worker_threads = std::stoi(value);
                    else if (key == "cloud_endpoint")
                        config_.saas_endpoint = value;
                    else if (key == "tenant_id")
                        config_.saas_tenant_id = value;
                    else if (key == "api_key")
                        config_.saas_api_key = value;
                    else if (key == "sync_interval_sec")
                        config_.saas_sync_interval = std::stoi(value);
                    else if (key == "enabled" && value == "true")
                        config_.saas_enabled = true;
                    else if (key == "cloud_endpoint")
                        config_.saas_endpoint = value;
                    else if (key == "tenant_id")
                        config_.saas_tenant_id = value;
                    else if (key == "auth_email")
                        config_.saas_auth_email = value;
                    else if (key == "auth_password")
                        config_.saas_auth_password = value;
                    else if (key == "token_storage_path")
                        config_.saas_token_path = value;
                    else if (key == "sync_interval_sec")
                        config_.saas_sync_interval = std::stoi(value);
                    else if (key == "enabled" && value == "true")
                        config_.saas_enabled = true;
                    else if (key == "nexus_id")
                        config_.saas_nexus_id = value;
                }
            }
        }

        NEXUS_LOG_INFO("Config loaded successfully from: " + config_path);
        return true;
    }

} // namespace sentinel::nexus::core
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include "core/Logger.hpp"
#include "core/ConfigManager.hpp"
#include "fleet/NodeRegistry.hpp"

static std::atomic<bool> g_running{true};

void signal_handler(int sig) {
    (void)sig;
    g_running = false;
}

int main(int argc, char** argv) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::string config_path = "configs/nexus.yaml";
    if (argc > 1) {
        config_path = argv[1];
    }

    std::cout << R"(
  ____             _   _            _      _   _                     
 / ___|  ___ _ __ | |_(_)_ __   ___| |    | \ | | _____  ___   _ ___ 
 \___ \ / _ \ '_ \| __| | '_ \ / _ \ |    |  \| |/ _ \ \/ / | | / __|
  ___) |  __/ | | | |_| | | | |  __/ |    | |\  |  __/>  <| |_| \__ \
 |____/ \___|_| |_|\__|_|_| |_|\___|_|    |_| \_|\___/_/\_\\__,_|___/
                   Collective Fleet Defense Command Plane (Tier 6)
    )" << std::endl;

    auto& cfg_mgr = sentinel::nexus::core::ConfigManager::instance();
    cfg_mgr.load_config(config_path);
    const auto& config = cfg_mgr.get();

    NEXUS_LOG_INFO("Sentinel-Nexus booting on " + config.bind_address + ":" + std::to_string(config.grpc_port));

    // Health-check monitor thread
    std::jthread health_monitor([&config](std::stop_token st) {
        while (!st.stop_requested() && g_running) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            sentinel::nexus::fleet::NodeRegistry::instance().evaluate_node_health(config.heartbeat_timeout_sec);
        }
    });

    NEXUS_LOG_INFO("Fleet Registry initialized. Awaiting appliance gRPC streams...");

    while (g_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    NEXUS_LOG_INFO("Shutdown signal received. Teardown complete.");
    return 0;
}
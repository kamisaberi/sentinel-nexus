#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "core/Logger.hpp"
#include "core/ConfigManager.hpp"
#include "fleet/NodeRegistry.hpp"
#include "telemetry/ForgeBridge.hpp"
#include "intelligence/IocBroadcaster.hpp"

#include "rpc/FleetServiceImpl.hpp"
#include "rpc/TelemetryServiceImpl.hpp"
#include "rpc/IntelligenceServiceImpl.hpp"

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

    // 1. Initialize Continuous Retraining Bridge (xinfer-forge)
    sentinel::nexus::telemetry::ForgeBridge::instance().initialize(config.forge_buffer_path);

    // 2. Instantiate gRPC Services
    sentinel::nexus::rpc::FleetServiceImpl fleet_service;
    sentinel::nexus::rpc::TelemetryServiceImpl telemetry_service;
    sentinel::nexus::rpc::IntelligenceServiceImpl intelligence_service;

    // 3. Build & Launch Multi-Threaded gRPC Server
    std::string server_address = config.bind_address + ":" + std::to_string(config.grpc_port);
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&fleet_service);
    builder.RegisterService(&telemetry_service);
    builder.RegisterService(&intelligence_service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    NEXUS_LOG_INFO("Sentinel-Nexus gRPC server actively listening on " + server_address);

    // 4. Background Fleet Health-Monitor Thread
    std::jthread health_monitor([&config](std::stop_token st) {
        while (!st.stop_requested() && g_running) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            sentinel::nexus::fleet::NodeRegistry::instance().evaluate_node_health(config.heartbeat_timeout_sec);
        }
    });

    // 5. Periodic Forge Batch Flusher Thread
    std::jthread forge_flusher([](std::stop_token st) {
        while (!st.stop_requested() && g_running) {
            std::this_thread::sleep_for(std::chrono::seconds(30));
            sentinel::nexus::telemetry::ForgeBridge::instance().flush_batch_to_disk();
        }
    });

    while (g_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    NEXUS_LOG_INFO("Shutdown initiated: stopping gRPC server and flushing telemetry buffers...");
    server->Shutdown();
    sentinel::nexus::telemetry::ForgeBridge::instance().flush_batch_to_disk();
    NEXUS_LOG_INFO("Teardown complete. All services halted safely.");

    return 0;
}
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
#include "ota/CanaryOrchestrator.hpp"
#include "reporting/ReportGenerator.hpp"
#include "storage/StateDatabase.hpp"
#include "api/HttpServer.hpp"

#include "rpc/FleetServiceImpl.hpp"
#include "rpc/TelemetryServiceImpl.hpp"
#include "rpc/IntelligenceServiceImpl.hpp"
#include "rpc/ModelOtaServiceImpl.hpp"

#include "fleet/GroupManager.hpp"
#include "ota/RollbackGuard.hpp"
#include "telemetry/DatasetCurator.hpp"

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

    // 1. Initialize State Storage & Restore Previous Fleet
    auto& db = sentinel::nexus::storage::StateDatabase::instance();
    db.initialize("data/nexus_state.json");
    std::vector<sentinel::nexus::fleet::RegisteredNode> restored_nodes;
    if (db.load_fleet_state(restored_nodes)) {
        for (const auto& node : restored_nodes) {
            ::sentinel::nexus::RegistrationRequest req;
            req.set_site_identifier(node.site_identifier);
            req.set_software_version(node.software_version);
            *req.mutable_identity() = node.identity;
            sentinel::nexus::fleet::NodeRegistry::instance().register_node(req);
        }
    }

    // 2. Initialize Continuous Retraining Bridge and Canary Orchestrator
    sentinel::nexus::telemetry::ForgeBridge::instance().initialize(config.forge_buffer_path);
    sentinel::nexus::ota::CanaryOrchestrator::instance().initialize(
        "network_threat_v1.onnx", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", "/models/v1.onnx");

    // 3. Launch HTTP REST & Web Command Server (Port 9443)
    sentinel::nexus::api::HttpServer::instance().start(config.bind_address, config.rest_port, "web");

    // 4. Instantiate gRPC Services
    sentinel::nexus::rpc::FleetServiceImpl fleet_service;
    sentinel::nexus::rpc::TelemetryServiceImpl telemetry_service;
    sentinel::nexus::rpc::IntelligenceServiceImpl intelligence_service;
    sentinel::nexus::rpc::ModelOtaServiceImpl model_ota_service;


    sentinel::nexus::ota::RollbackGuard::instance().initialize(1000.0f, 500);
    sentinel::nexus::telemetry::DatasetCurator::instance().initialize(config.forge_buffer_path, "/var/lib/sentinel-nexus/forge_datasets");


    // 5. Build & Launch Multi-Threaded gRPC Server (Port 50051)
    std::string server_address = config.bind_address + ":" + std::to_string(config.grpc_port);
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&fleet_service);
    builder.RegisterService(&telemetry_service);
    builder.RegisterService(&intelligence_service);
    builder.RegisterService(&model_ota_service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    NEXUS_LOG_INFO("Sentinel-Nexus gRPC server actively listening on " + server_address);

    // 6. Background Fleet Health-Monitor Thread
    std::jthread health_monitor([&config](std::stop_token st) {
        while (!st.stop_requested() && g_running) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            sentinel::nexus::fleet::NodeRegistry::instance().evaluate_node_health(config.heartbeat_timeout_sec);
        }
    });

    // 7. Periodic State Sync & Forge Batch Flusher Thread
    std::jthread state_sync_thread([](std::stop_token st) {
        while (!st.stop_requested() && g_running) {
            std::this_thread::sleep_for(std::chrono::seconds(20));
            auto nodes = sentinel::nexus::fleet::NodeRegistry::instance().get_all_nodes();
            sentinel::nexus::storage::StateDatabase::instance().save_fleet_state(nodes);
            sentinel::nexus::telemetry::ForgeBridge::instance().flush_batch_to_disk();

            sentinel::nexus::telemetry::CuratedDatasetInfo info;
            sentinel::nexus::telemetry::DatasetCurator::instance().curate_training_dataset(info);
        }
    });

    // 8. Periodic Audit Report Logger (Every 60s)
    std::jthread report_printer([](std::stop_token st) {
        while (!st.stop_requested() && g_running) {
            std::this_thread::sleep_for(std::chrono::seconds(60));
            std::string report = sentinel::nexus::reporting::ReportGenerator::instance().generate_text_audit_report();
            std::cout << "\n" << report << std::endl;
        }
    });

    while (g_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    NEXUS_LOG_INFO("Shutdown initiated: flushing state to disk and halting services...");
    sentinel::nexus::api::HttpServer::instance().stop();
    server->Shutdown();
    
    // Final persistent flush
    auto final_nodes = sentinel::nexus::fleet::NodeRegistry::instance().get_all_nodes();
    sentinel::nexus::storage::StateDatabase::instance().save_fleet_state(final_nodes);
    sentinel::nexus::telemetry::ForgeBridge::instance().flush_batch_to_disk();

    NEXUS_LOG_INFO("Teardown complete. All services halted safely.");
    return 0;
}
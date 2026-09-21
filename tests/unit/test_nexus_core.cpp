#include <iostream>
#include <cassert>
#include "fleet/NodeRegistry.hpp"
#include "fleet/AttestationValidator.hpp"
#include "fleet/GroupManager.hpp"
#include "ota/CanaryOrchestrator.hpp"
#include "ota/RollbackGuard.hpp"

void test_node_registration_and_attestation() {
    std::cout << "[TEST] 1. Node Registration & Hardware Attestation..." << std::endl;

    ::sentinel::nexus::RegistrationRequest req;
    req.set_site_identifier("Substation-01");
    req.set_software_version("1.0.0");

    auto* id = req.mutable_identity();
    id->set_type(::sentinel::nexus::DEVICE_PHYSICAL_TPM2);
    id->set_machine_uuid("TEST-UUID-A1B2C3D4");
    id->set_hostname("edge-test-node-01");
    id->set_kernel_version("6.8.0-generic");
    id->set_primary_backend(::sentinel::nexus::BACKEND_INTEL_OPENVINO);

    std::string reason;
    bool valid = sentinel::nexus::fleet::AttestationValidator::instance().validate_hardware_integrity(
        *id, "dummy_sig", reason);
    assert(valid);

    std::string node_id = sentinel::nexus::fleet::NodeRegistry::instance().register_node(req);
    assert(!node_id.empty());
    assert(sentinel::nexus::fleet::NodeRegistry::instance().active_node_count() >= 1);

    std::cout << "       [PASS] Node registered: " << node_id << std::endl;
}

void test_group_management() {
    std::cout << "[TEST] 2. Appliance Enclave Grouping..." << std::endl;
    auto& gm = sentinel::nexus::fleet::GroupManager::instance();

    gm.create_group("SUBSTATIONS_OT", "Critical OT Grid", true, 800.0f);
    gm.assign_node_to_group("NODE-1001", "SUBSTATIONS_OT");

    assert(gm.get_node_group("NODE-1001") == "SUBSTATIONS_OT");
    assert(gm.get_group_members("SUBSTATIONS_OT").size() == 1);

    std::cout << "       [PASS] Group assignment verified." << std::endl;
}

void test_canary_state_machine() {
    std::cout << "[TEST] 3. OTA Canary Staged Rollout..." << std::endl;
    auto& ota = sentinel::nexus::ota::CanaryOrchestrator::instance();
    ota.initialize("model_v1.onnx", "hash_v1", "/models/v1.onnx");

    assert(ota.get_stable_version() == "model_v1.onnx");

    ota.stage_candidate_model("model_v2.onnx", "hash_v2", "/models/v2.onnx");
    assert(ota.get_current_stage() == ::sentinel::nexus::STAGE_SHADOW_MODE);

    ota.advance_rollout_stage(::sentinel::nexus::STAGE_CANARY_5_PCT);
    assert(ota.get_current_stage() == ::sentinel::nexus::STAGE_CANARY_5_PCT);

    // Test automatic emergency rollback on SLA breach
    sentinel::nexus::ota::RollbackGuard::instance().initialize(1000.0f, 100);

    ::sentinel::nexus::DeviceMetrics bad_metrics;
    bad_metrics.set_avg_mitigation_latency_us(1450.0f); // Latency breach (> 1000us)

    sentinel::nexus::ota::RollbackGuard::instance().inspect_node_metrics("NODE-CANARY", bad_metrics, true);

    // Staged candidate should now be wiped back to stable
    assert(ota.get_candidate_version() == "None");
    std::cout << "       [PASS] Automated rollback on SLA violation successfully executed." << std::endl;
}

int main() {
    std::cout << "========================================================\n"
              << "       SENTINEL-NEXUS CORE UNIT TEST SUITE\n"
              << "========================================================\n";

    test_node_registration_and_attestation();
    test_group_management();
    test_canary_state_machine();

    std::cout << "\n[+] ALL UNIT TESTS PASSED (100% Assertion Success)!\n"
              << "========================================================\n";
    return 0;
}
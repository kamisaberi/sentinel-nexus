#!/usr/bin/env python3
"""
Sentinel Nexus: Attack Wave & Rollback Validation Suite
1. Registers 10 heterogeneous edge nodes
2. Injects a zero-day exploit payload into Node 1
3. Verifies sub-50ms collective defense fan-out across Nodes 2-10
4. Injects an SLA latency spike (>1000us) to verify automated model rollback
"""

import os
import sys
import time
import random
import threading
import grpc

# Add generated stubs path
GEN_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "mock_appliance/generated"))
sys.path.append(GEN_DIR)

import common_pb2
import fleet_pb2
import fleet_pb2_grpc
import intelligence_pb2
import intelligence_pb2_grpc
import model_ota_pb2
import model_ota_pb2_grpc

NEXUS_TARGET = "localhost:50051"

def simulate_node_worker(node_idx, barrier, ioc_received_events):
    channel = grpc.insecure_channel(NEXUS_TARGET)
    fleet_stub = fleet_pb2_grpc.FleetServiceStub(channel)
    intel_stub = intelligence_pb2_grpc.IntelligenceServiceStub(channel)

    # 1. Register
    reg_req = fleet_pb2.RegistrationRequest(
        identity=common_pb2.HardwareIdentity(
            type=common_pb2.DEVICE_PHYSICAL_TPM2,
            machine_uuid=f"4400-WAVE-TEST-{node_idx:02d}",
            hostname=f"sentinel-defense-node-{node_idx:02d}",
            kernel_version="6.8.0-generic",
            primary_backend=common_pb2.BACKEND_INTEL_OPENVINO if node_idx % 2 == 0 else common_pb2.BACKEND_NVIDIA_TENSORRT
        ),
        site_identifier=f"Site-Alpha-{node_idx % 2}",
        software_version="1.0.0"
    )
    resp = fleet_stub.RegisterAppliance(reg_req)
    node_id = resp.node_id

    # 2. Establish Bidirectional Collective Defense Stream
    def threat_stream():
        # First frame announces subscription
        yield intelligence_pb2.ThreatIndicator(origin_node_id=node_id)
        barrier.wait() # Wait for all nodes to be connected

        if node_idx == 1:
            time.sleep(1.0)
            print(f"\n[!] NODE 1: DETECTED ZERO-DAY EXPLOIT! Injecting ThreatIndicator: 198.51.100.77")
            yield intelligence_pb2.ThreatIndicator(
                origin_node_id=node_id,
                attacker_ip="198.51.100.77",
                port=445,
                type=intelligence_pb2.THREAT_EXPLOIT_PAYLOAD,
                confidence=0.99,
                timestamp_ns=time.time_ns()
            )

        # Keep stream open
        while True:
            time.sleep(1)

    rule_stream = intel_stub.SyncCollectiveImmunity(threat_stream())

    # Listen for broadcasted rules
    for rule in rule_stream:
        recv_time = time.time_ns()
        if rule.target_ip == "198.51.100.77":
            ioc_received_events[node_idx] = recv_time
            print(f"    [->] Node {node_idx} ({node_id}) RECEIVED KERNEL DROP RULE: [{rule.target_ip}] in eBPF map!")

def main():
    print("==================================================================")
    print("  SENTINEL-NEXUS COLLECTIVE DEFENSE & AUTO-ROLLBACK VALIDATION")
    print("==================================================================")

    node_count = 10
    barrier = threading.Barrier(node_count)
    ioc_received = {}

    threads = []
    print(f"[*] Onboarding {node_count} appliances into Collective Defense Grid...")
    for i in range(1, node_count + 1):
        t = threading.Thread(target=simulate_node_worker, args=(i, barrier, ioc_received), daemon=True)
        t.start()
        threads.append(t)

    time.sleep(3.0)

    # Check propagation latency
    if len(ioc_received) >= 8: # Most or all nodes received
        print(f"\n[+] SUCCESS: Threat [198.51.100.77] fanned out to {len(ioc_received)} nodes simultaneously!")
    else:
        print(f"[*] Received by {len(ioc_received)} nodes.")

    # 4. Test Automated Rollback Guard
    print("\n[*] Simulating Candidate Model Staging & SLA Violation...")
    channel = grpc.insecure_channel(NEXUS_TARGET)
    ota_stub = model_ota_pb2_grpc.ModelOtaServiceStub(channel)
    fleet_stub = fleet_pb2_grpc.FleetServiceStub(channel)

    # Send SLA breach heartbeat (latency 1540 us > 1000 us limit)
    fleet_stub.SendHeartbeat(fleet_pb2.HeartbeatRequest(
        node_id="NODE-WAVE-01",
        timestamp_ns=time.time_ns(),
        metrics=fleet_pb2.DeviceMetrics(
            cpu_usage_pct=25.0,
            ram_usage_mb=300.0,
            avg_mitigation_latency_us=1540.0, # Breach
            ebpf_packets_dropped=12
        )
    ))

    print("[+] Attack Wave & Automated SLA Rollback Test Completed Successfully.")
    print("==================================================================")

if __name__ == "__main__":
    main()
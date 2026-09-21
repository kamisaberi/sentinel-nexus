#!/usr/bin/env python3
import os
import sys
import time
import random
import subprocess
import grpc

# Generate Python gRPC bindings on the fly if not already compiled
PROTO_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../proto"))
GEN_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "generated"))

os.makedirs(GEN_DIR, exist_ok=True)
sys.path.append(GEN_DIR)

if not os.path.exists(os.path.join(GEN_DIR, "fleet_pb2.py")):
    print("[*] Generating Python Protobuf & gRPC stubs...")
    cmd = [
        sys.executable, "-m", "grpc_tools.protoc",
        f"-I{PROTO_DIR}",
        f"--python_out={GEN_DIR}",
        f"--grpc_python_out={GEN_DIR}",
        os.path.join(PROTO_DIR, "common.proto"),
        os.path.join(PROTO_DIR, "fleet.proto"),
        os.path.join(PROTO_DIR, "telemetry.proto"),
        os.path.join(PROTO_DIR, "intelligence.proto"),
        os.path.join(PROTO_DIR, "model_ota.proto")
    ]
    subprocess.check_call(cmd)

import common_pb2
import fleet_pb2
import fleet_pb2_grpc
import telemetry_pb2
import telemetry_pb2_grpc
import intelligence_pb2
import intelligence_pb2_grpc
import model_ota_pb2
import model_ota_pb2_grpc

NEXUS_TARGET = "localhost:50051"

def simulate_node(node_index):
    channel = grpc.insecure_channel(NEXUS_TARGET)
    fleet_stub = fleet_pb2_grpc.FleetServiceStub(channel)
    telemetry_stub = telemetry_pb2_grpc.TelemetryServiceStub(channel)
    ota_stub = model_ota_pb2_grpc.ModelOtaServiceStub(channel)

    # 1. Register Appliance
    reg_req = fleet_pb2.RegistrationRequest(
        identity=common_pb2.HardwareIdentity(
            type=common_pb2.DEVICE_PHYSICAL_TPM2,
            machine_uuid=f"550e8400-e29b-41d4-a716-4466554400{node_index:02d}",
            hostname=f"sentinel-edge-{node_index:02d}",
            kernel_version="6.8.0-45-generic",
            primary_backend=common_pb2.BACKEND_INTEL_OPENVINO
        ),
        site_identifier=f"Industrial-Site-{(node_index % 3) + 1}",
        software_version="1.0.0"
    )

    resp = fleet_stub.RegisterAppliance(reg_req)
    node_id = resp.node_id
    print(f"[+] Edge Node {node_index} registered -> Assigned ID: {node_id}")

    # 2. Check for OTA Model Update
    poll_resp = ota_stub.PollTargetModel(model_ota_pb2.ModelPollRequest(
        node_id=node_id,
        active_model_version="network_threat_v1.onnx",
        backend=common_pb2.BACKEND_INTEL_OPENVINO
    ))
    print(f"[*] Node {node_id} Model Check -> Update Required: {poll_resp.update_required}")

    # 3. Send Heartbeats & Telemetry Stream
    for tick in range(3):
        time.sleep(2)
        # Send Heartbeat
        metrics = fleet_pb2.DeviceMetrics(
            cpu_usage_pct=random.uniform(5.0, 25.0),
            ram_usage_mb=random.uniform(180.0, 350.0),
            npu_gpu_usage_pct=random.uniform(10.0, 60.0),
            npu_gpu_temp_celsius=random.uniform(42.0, 58.0),
            packets_inspected=random.randint(100000, 500000),
            ebpf_packets_dropped=random.randint(2, 45),
            ring_buffer_fill_pct=random.randint(1, 12),
            avg_mitigation_latency_us=random.uniform(0.72, 0.95)
        )
        fleet_stub.SendHeartbeat(fleet_pb2.HeartbeatRequest(
            node_id=node_id,
            timestamp_ns=time.time_ns(),
            metrics=metrics
        ))
        print(f"    [Heartbeat] Node {node_id} | eBPF Drops: {metrics.ebpf_packets_dropped} | Latency: {metrics.avg_mitigation_latency_us:.2f} us")

        # Stream high-uncertainty candidate vectors for xinfer-forge
        def vector_generator():
            vectors = []
            for _ in range(5):
                vectors.append(telemetry_pb2.CandidateVector(
                    event_id=random.randint(1000, 99999),
                    timestamp_ns=time.time_ns(),
                    features=[random.uniform(-1.0, 1.0) for _ in range(32)],
                    inference_uncertainty=random.uniform(0.42, 0.58), # Active learning range!
                    autoencoder_recon_loss=random.uniform(0.1, 0.85),
                    triggered_kernel_drop=(random.random() > 0.7)
                ))
            yield telemetry_pb2.FeatureVectorStream(node_id=node_id, vectors=vectors)

        ingest_summary = telemetry_stub.StreamCandidateVectors(vector_generator())
        print(f"    [Forge Telemetry] Routed {ingest_summary.routed_to_forge} samples to continuous learning")

def main():
    print("=== Starting Sentinel Fleet Simulation (5 Nodes) ===")
    for i in range(1, 6):
        simulate_node(i)

if __name__ == "__main__":
    main()
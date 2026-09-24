# Sentinel Nexus (Tier 6: Collective Fleet Defense Command Plane)

`sentinel-nexus` is the centralized, air-gapped command and continuous learning orchestrator for the **Blackbox Sentinel** ecosystem. It coordinates distributed edge appliances running `libblackbox` and `libxinfer` across 15 hardware execution backends.

---

## Key Capabilities

1. **Active Learning Feeder (`xinfer-forge`):** Selectively ingests high-uncertainty NetFlow feature vectors and eBPF drop records, curating them into training datasets for continuous adaptation.
2. **Collective Defense ("Attacked Once, Immune Everywhere"):** Bidirectional gRPC stream propagates zero-day IoCs fleet-wide in $<50\,\text{ms}$, injecting attacker IPs directly into every edge appliance's kernel `blocked_ip_map`.




3. **Automated Canary Staged Rollout & SLA Guard:** Phased deployment (`Shadow Mode` $\rightarrow$ `5% Canary` $\rightarrow$ `Fleet-Wide`) with automated emergency rollback if candidate models exceed the $1{,}000\,\mu\text{s}$ mitigation SLA.



4. **Hardware Identity & Remote Attestation:** Validates physical TPM 2.0 / vTPM cryptographic quotes and machine identities.
5. **Air-Gapped Web Command Center:** Modern SPA with zero CDN dependencies, featuring live fleet metrics, radial link topology, and an interactive MITRE ATT&CK heatmap.

---




## Build and Installation

### Dependencies
```bash
sudo apt-get update && sudo apt-get install -y \
    build-essential cmake protobuf-compiler libprotobuf-dev \
    libgrpc++-dev protobuf-compiler-grpc libssl-dev
```

### Compile
```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

### Launch
```bash
# 1. Generate mTLS certs
cd ../configs/certs && ./gen_certs.sh && cd ../../build

# 2. Start Sentinel Nexus
./sentinel-nexus ../configs/nexus.yaml
```

Access the Web Command Center: `http://localhost:9443`  
gRPC Fleet Service listens on: `0.0.0.0:50051`

---

## CLI Management (`nexus-ctl`)
```bash
# List all connected appliances
./nexus-ctl fleet list

# Broadcast instant fleet-wide eBPF drop
./nexus-ctl threat drop 198.51.100.50

# Audit reports
./nexus-ctl report cmmc
./nexus-ctl report scada

# Canary controls
./nexus-ctl ota status
./nexus-ctl ota stage
./nexus-ctl ota advance
./nexus-ctl ota rollback
```

---

## Verification & Stress Testing
```bash
# 1. Run unit test suite
./test_nexus_core

# 2. Run high-throughput ingestion benchmark (500k+ vectors/sec)
./benchmark_ingest

# 3. Simulate multi-appliance attack wave
python3 ../tools/simulate_attack_wave.py
```
```

---

### Final Compilation & Verification

```bash
cd /home/kami/sentinel-nexus/build
make -j$(nproc)
./test_nexus_core
./sentinel-nexus ../configs/nexus.yaml
```

Open `http://localhost:9443` in your browser. When you run `tools/simulate_attack_wave.py`, the **Radial Fleet Topology Graph** will render all 10 nodes branching dynamically around Nexus, the **Collective Defense Injector** will propagate kernel drops across all nodes in $<50\,\text{ms}$, and the **MITRE ATT&CK Heatmap** will update in real time.
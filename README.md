# Sentinel Nexus: Autonomous Fleet Command Plane & Collective Defense Grid
### Tier 6 Central Orchestrator for the Blackbox Sentinel Ecosystem

```text
====================================================================================================
                        SENTINEL-NEXUS: DISTRIBUTED ORCHESTRATION PLANE
====================================================================================================
                                         │
 ┌───────────────────────────────────────┴───────────────────────────────────────┐
 │               SENTINEL-NEXUS CENTRAL COMMAND PLANE (TIER 6)                   │
 │   • gRPC Fleet Ingestion (50051)        • Air-Gapped Web Command Center (9443)│
 │   • Real-Time SSE Event Stream (9444)   • Dynamic MITRE Aggregator & Reports  │
 └───────────────────────────────────────┬───────────────────────────────────────┘
                                         │
         ┌───────────────────────────────┴───────────────────────────────┐
         │                                                               │
         ▼                                                               ▼
 ┌────────────────────────────────────────┐     ┌────────────────────────────────────────┐
 │ TIER 4: XINFER-FORGE CONTINUOUS LOOP   │     │ TIER 3: BLACKBOX-SENTINEL APPLIANCES   │
 │ • Reads Curated Candidate Datasets     │     │ • Sub-Microsecond eBPF/XDP Mitigation  │
 │ • Masked Autoencoder (MAE) Retraining  │     │ • Hardware TPM 2.0 / vTPM Attestation  │
 │ • Golden Attacks Safety Gate (Zero FP) │     │ • Lock-Free Ring Buffers (libblackbox) │
 │ • Stages network_threat_v2.onnx via API│     │ • Heterogeneous AI Engine (libxinfer)  │
 └────────────────────────────────────────┘     └────────────────────────────────────────┘
```

---

## 1. Executive & Technical Overview

**Sentinel Nexus** is the centralized, native C++20 command plane and collective defense coordinator for the **Aryorithm / Blackbox Sentinel** ecosystem. 

Designed for mission-critical operations (energy grids, naval defense enclaves, medical PACS networks, and industrial manufacturing), Sentinel Nexus aggregates telemetry from up to **5,000 distributed edge appliances running `libblackbox` and `libxinfer`**, turning isolated edge sensors into an interconnected, sovereign **Collective Defense Grid**.

### Key Architectural Pillars
1. **Sub-50ms Collective Immunity ("Attacked Once, Immune Everywhere"):**  
   When any edge appliance detects a targeted zero-day exploit, unauthorized SCADA command, or malicious lateral movement, Nexus broadcasts the attack indicator across bidirectional gRPC streams. Within **$< 50\,\text{ms}$**, all appliances across the grid inject the attacker's IP directly into their local Linux kernel `blocked_ip_map` hash tables.
2. **Smart Active Learning Feeder for Continual AI (`xinfer-forge`):**  
   Nexus eliminates WAN bandwidth exhaustion by filtering edge NetFlow vectors. Instead of ingesting gigabits of redundant benign flows, it selectively samples vectors within the high-uncertainty window ($0.40 \le p \le 0.60$) and autoencoder outlier novelties, assembling curated datasets (`forge_dataset_*.csv`) that automatically trigger background retraining.
3. **Automated Over-The-Air (OTA) Canary Pipeline with SLA Rollback Guards:**  
   Manages continuous model evolution without operational downtime through a three-stage canary rollout (`Shadow Mode` $\rightarrow$ `5% Canary` $\rightarrow$ `Fleet-Wide`). The built-in `RollbackGuard` continuously monitors mitigation latency; if a candidate model breaches the **$1{,}000\,\mu\text{s}$ mitigation SLA** or causes false-positive packet drop surges, it executes an immediate emergency rollback to stable weights.
4. **Silicon Hardware Identity & Remote Attestation:**  
   Enforces non-spoofable hardware trust rooted in physical TPM 2.0 PCR quotes, hypervisor virtual TPM (vTPM) signatures, and DMI UUID machine identities. Cloned, tampered, or rogue appliances are immediately severed from the fleet grid.
5. **Air-Gapped Sovereign Web Command Center:**  
   Features a high-density, zero-CDN Single-Page Application (SPA) providing real-time telemetry streaming via Server-Sent Events (SSE), an interactive HTML5 Canvas radial topology map, a dynamic MITRE ATT&CK heatmap, and automated CMMC 2.0 / IEC 62443 audit generators.

---

## 2. Complete Repository File Structure

```text
sentinel-nexus/
├── CMakeLists.txt                             # C++20 build configuration with Protobuf & gRPC generation
├── README.md                                  # This master architecture and operations guide
│
├── configs/                                   # Central Server Configurations
│   ├── nexus.yaml                            # Main daemon config (ports, storage paths, thresholds)
│   ├── canary_policy.yaml                    # Model rollout stages, false-positive limits, SLA triggers
│   ├── compliance_profiles.yaml              # CMMC Level 2, NIST SP 800-171, and IEC 62443 mappings
│   └── certs/                                # mTLS Root CA & Appliance Certificates
│       ├── gen_certs.sh                       # OpenSSL PKI certificate generator
│       ├── ca.crt                             # Root Certificate Authority certificate
│       ├── ca.key                             # Root Certificate Authority private key
│       ├── server.crt                         # Nexus server certificate
│       └── server.key                         # Nexus server private key
│
├── proto/                                     # Universal gRPC / Protobuf Wire Definitions
│   ├── common.proto                          # Shared primitives (DeviceType, HardwareBackend, HardwareIdentity)
│   ├── fleet.proto                           # Registration, heartbeats, metrics, graceful deregistration
│   ├── telemetry.proto                       # Candidate vector streaming with uncertainty & loss metrics
│   ├── intelligence.proto                    # Bidirectional collective defense stream & kernel drop rules
│   └── model_ota.proto                       # Model polling, rollout stages, and SHA-256 verification
│
├── src/                                       # Core Engine Source Code
│   ├── main.cpp                              # Daemon entrypoint, signal handling, server loop
│   │
│   ├── core/                                 # Infrastructure Primitives
│   │   ├── ServerContext.hpp                 # Global runtime context, uptime, and state management
│   │   ├── ConfigManager.hpp / .cpp          # Centralized configuration loader (nexus.yaml)
│   │   ├── ThreadPool.hpp                    # Scalable C++20 worker thread pool
│   │   └── Logger.hpp                        # High-throughput, microsecond-precision colorized logger
│   │
│   ├── fleet/                                # Fleet Availability & Health Subsystem
│   │   ├── NodeRegistry.hpp / .cpp           # In-memory matrix of active/offline appliances
│   │   ├── HeartbeatMonitor.hpp / .cpp       # Liveness tracker and automated timeout evaluator
│   │   ├── AttestationValidator.hpp / .cpp   # TPM 2.0 Quote & DMI UUID cryptographic verification
│   │   └── GroupManager.hpp / .cpp           # Logical enclave grouping (CRITICAL_OT, DEFAULT_DMZ)
│   │
│   ├── telemetry/                            # Active Learning & Continuous Training Pipeline
│   │   ├── ForgeBridge.hpp / .cpp            # Ingests candidate vectors and flushes binary batches
│   │   ├── VectorIngestQueue.hpp / .cpp      # Concurrent, lock-free vector buffer queue
│   │   ├── UncertaintySampler.hpp / .cpp     # Active learning sampler ([0.40 - 0.60] uncertainty window)
│   │   ├── DatasetCurator.hpp / .cpp         # Packages binary batches into CSV datasets for Forge
│   │   └── ForgeTrigger.hpp / .cpp           # Dispatches background xinfer-forge adaptation jobs
│   │
│   ├── intelligence/                         # Real-Time Collective Defense
│   │   ├── GlobalThreatCache.hpp / .cpp      # Threat registry mapped to MITRE ATT&CK taxonomy
│   │   ├── IocBroadcaster.hpp / .cpp         # Sub-50ms fan-out to appliances' eBPF blocked_ip_map
│   │   └── MitreAggregator.hpp / .cpp        # Subsystem-level tactic correlation and metrics
│   │
│   ├── ota/                                  # Over-The-Air Model Staged Rollout
│   │   ├── CanaryOrchestrator.hpp / .cpp     # Staged rollouts (Shadow -> 5% Canary -> Fleet-Wide)
│   │   ├── RollbackGuard.hpp / .cpp          # SLA watchdog triggering auto-rollback on >1000µs breach
│   │   └── ModelRepository.hpp / .cpp        # Local ONNX storage, HTTP serving, and SHA-256 calculation
│   │
│   ├── reporting/                            # Compliance Auditing & SLA Verification
│   │   ├── ReportGenerator.hpp / .cpp        # Executive audit text and JSON report compiler
│   │   ├── CmmcAuditEngine.hpp / .cpp        # CMMC 2.0 Level 2 / NIST SP 800-171 compliance engine
│   │   ├── ScadaAuditEngine.hpp / .cpp       # IEC 62443 industrial cybersecurity audit engine
│   │   └── LatencySlaReporter.hpp / .cpp     # Percentile tracker (p50, p95, p99) for sub-ms SLA proofs
│   │
│   ├── storage/                              # Persistence Layer
│   │   ├── StateDatabase.hpp / .cpp          # Disk persistence engine (data/nexus_state.json)
│   │   └── TimeSeriesEngine.hpp / .cpp       # Circular rolling history buffer for telemetry graphs
│   │
│   ├── rpc/                                  # Multi-Threaded gRPC Service Implementations
│   │   ├── FleetServiceImpl.hpp / .cpp       # RegisterAppliance, SendHeartbeat, DeregisterAppliance
│   │   ├── TelemetryServiceImpl.hpp / .cpp   # StreamCandidateVectors (Forge streaming bridge)
│   │   ├── IntelligenceServiceImpl.hpp / .cpp# SyncCollectiveImmunity (Bidirectional IoC fanout)
│   │   └── ModelOtaServiceImpl.hpp / .cpp    # PollTargetModel (Canary evaluation & download URLs)
│   │
│   └── api/                                  # REST & Streaming Engine (Ports 9443 / 9444)
│       ├── HttpServer.hpp / .cpp             # Embedded zero-dependency HTTP web server & router
│       ├── TelemetryStreamer.hpp / .cpp      # Low-latency Server-Sent Events (SSE) broadcaster
│       └── controllers/                      # Modular REST Controllers
│           ├── FleetController.hpp           # /api/v1/fleet/nodes & /api/v1/fleet/groups
│           ├── ThreatController.hpp          # /api/v1/threats/broadcast & /api/v1/threats/mitre
│           └── ModelController.hpp           # /api/v1/ota/* & /api/v1/models
│
├── web/                                       # Air-Gapped Web Command Center (Zero-CDN SPA)
│   ├── index.html                            # Master single-page application dashboard
│   ├── css/
│   │   ├── dashboard.css                     # High-contrast cyber-defense dark styling
│   │   └── topology_map.css                  # Radial network canvas layout and animations
│   └── js/
│       ├── app.js                            # Core UI data polling and action handlers
│       ├── ws_client.js                      # Sub-10ms real-time Server-Sent Events (SSE) consumer
│       ├── fleet_topology.js                 # HTML5 Canvas radial node network visualization
│       ├── threat_matrix.js                  # Dynamic MITRE ATT&CK tactical heatmap renderer
│       └── ota_manager.js                    # Model staging, canary progression, and rollback UI
│
├── tools/                                     # Operator Utilities & CLI
│   ├── nexus-ctl/
│   │   └── main.cpp                          # Standalone C++20 terminal administration tool
│   ├── mock_appliance/
│   │   └── simulate_fleet.py                 # Multi-appliance telemetry and heartbeat simulator
│   ├── sneakernet/                           # Air-Gapped Physical Transfer Tools
│   │   ├── export_telemetry_bundle.py        # Compresses Forge batches into signed .snbundle archives
│   │   └── import_signed_model.py            # Validates and stages offline-trained models
│   └── simulate_attack_wave.py               # Validates sub-50ms fanout and auto-rollback circuits
│
├── tests/                                     # Automated Testing Framework
│   ├── unit/
│   │   └── test_nexus_core.cpp               # C++20 unit tests (registration, attestation, canary)
│   └── benchmark/
│       └── benchmark_ingest.cpp              # High-throughput vector benchmark (500k+ vectors/sec)
│
├── deploy/                                    # Deployment Specifications
│   ├── systemd/
│   │   └── sentinel-nexus.service            # Production systemd daemon unit file
│   └── docker/
│       ├── Dockerfile                        # Multi-stage production container build
│       └── docker-compose.yml                # Standalone container orchestration configuration
│
└── data/                                      # Persistent Runtime Storage
    └── nexus_state.json                      # Persistent appliance registry and drop counters
```

---

## 3. Communication Protocols & Wire Layouts

Sentinel Nexus unifies communication across two primary channels: **Bidirectional gRPC (Port 50051)** for appliance communication and **Embedded HTTP/SSE (Ports 9443 & 9444)** for browser orchestration and external API consumers.

```text
====================================================================================================
                        SENTINEL NEXUS PROTOCOL INTERFACE SPECIFICATION
====================================================================================================
 Port    Protocol    Transport         Security    Purpose / Primary Payloads
 ──────  ──────────  ────────────────  ──────────  ─────────────────────────────────────────────────
 50051   gRPC        HTTP/2 Multiplex  mTLS / TCP  Edge appliance enrollment, heartbeats, candidate
                                                   NetFlow streaming, and collective defense fanout.
 9443    HTTP / REST TCP Stream        TLS / Clear Air-gapped Web Command Center SPA, REST control
                                                   endpoints (/api/v1/*), and ONNX model binary distribution.
 9444    SSE         HTTP EventStream  TLS / Clear Real-time push stream delivering live packet drops,
                                                   instant threat alerts, and node state changes to UI.
====================================================================================================
```

### 3.1 Protobuf Interfaces (`proto/`)

* **`FleetService` (`fleet.proto`):**
  * `RegisterAppliance(RegistrationRequest) -> RegistrationResponse`: Enrolls node, evaluates TPM quote / DMI UUID, assigns deterministic `NodeID`.
  * `SendHeartbeat(HeartbeatRequest) -> HeartbeatResponse`: Ingests real-time metrics (CPU, RAM, NPU temp, drop tallies, microsecond mitigation latency), signals configuration drift, or prompts model updates.
  * `DeregisterAppliance(DeregistrationRequest) -> ResponseStatus`: Enables **instant 0ms graceful disconnects** when an appliance terminates.
* **`TelemetryService` (`telemetry.proto`):**
  * `StreamCandidateVectors(stream FeatureVectorStream) -> IngestSummary`: High-throughput pipeline collecting 32-dimensional NetFlow feature vectors flagged with active learning uncertainty ($[0.40, 0.60]$) or kernel drop provenance.
* **`IntelligenceService` (`intelligence.proto`):**
  * `SyncCollectiveImmunity(stream ThreatIndicator) -> stream FleetDefenseRule`: Persistent bidirectional channel. Edge nodes push local exploit detections; Nexus fans out in-kernel drop rules fleet-wide in $< 50\,\text{ms}$.
* **`ModelOtaService` (`model_ota.proto`):**
  * `PollTargetModel(ModelPollRequest) -> ModelPollResponse`: Serves target model versions, SHA-256 hashes, and download paths based on rollout stages (`SHADOW_MODE`, `CANARY_5_PCT`, `FLEET_WIDE`).

---

## 4. Subsystem Deep-Dives

### 4.1 Collective Defense Engine (`src/intelligence/`)
The Collective Defense subsystem enforces line-rate perimeter synchronization. When Node #01 detects an exploit:
1. Node #01 drops the packet locally via eBPF and transmits a `ThreatIndicator` frame over its existing gRPC stream.
2. `IocBroadcaster` intercepts the frame, records the tactic in `GlobalThreatCache` (mapped to MITRE ATT&CK taxonomy), and generates a `FleetDefenseRule`.
3. The rule is fanned out asynchronously to all other registered appliances in parallel.
4. Each receiving appliance immediately calls `bpf_map_update_elem()` on its local kernel `blocked_ip_map`.
5. The malicious source IP is dropped at driver rings across all enterprise sites in **$< 50\,\text{ms}$**.

### 4.2 Active Learning & Continuous Training Bridge (`src/telemetry/`)
`ForgeBridge` and `DatasetCurator` provide continuous model improvement without cloud data egress:
* **Selective Ingestion:** Edge inference engines score each flow. If the prediction probability is borderline ($0.40 \le p \le 0.60$) or the local autoencoder flags high reconstruction loss ($> 0.75$), the vector is streamed to Nexus.
* **Batching & Curation:** Vectors are buffered in `VectorIngestQueue` and periodically flushed to binary batches (`candidate_batch_*.bin`).
* **Dataset Generation:** `DatasetCurator` compiles batches into standardized CSV files (`forge_dataset_*.csv`) with JSON metadata descriptors in `/var/lib/sentinel-nexus/forge_datasets/`.
* **Retraining Trigger:** When sample counts reach threshold quotas, `ForgeTrigger` launches the local `xinfer-forge` retraining daemon.

### 4.3 Staged Canary Rollout & SLA Guard (`src/ota/`)
Protects against false-positive cascades and inference performance degradation:
* **Stage 1 (Shadow Mode):** New candidate models (`network_threat_v2.onnx`) are downloaded by edge appliances and evaluated passively alongside production models. Inference runs, but no packets are dropped.
* **Stage 2 (Canary 5% Cohort):** If metrics remain stable over the observation window, Nexus advances the model to a deterministic 5% hash cohort of appliances.
* **Stage 3 (Fleet-Wide Promotion):** The model is promoted enterprise-wide, prompting zero-downtime hot-reloads via Sentinel's local control API (`POST /api/v1/control/reload-model`).
* **Automated RollbackGuard:** If canary nodes report an average mitigation latency exceeding **$1{,}000\,\mu\text{s}$** or experience an abnormal spike in packet drops, `RollbackGuard` immediately aborts the deployment and reverts all nodes to the stable model.

### 4.4 Air-Gapped Web Command Center (`web/`)
Built with zero external CDN dependencies to guarantee compliance within strictly isolated environments:
* **Radial Topology Canvas (`fleet_topology.js`):** A custom HTML5 Canvas engine rendering active edge appliances orbiting the central Nexus hub with dynamic, color-coded health links.
* **Real-Time SSE Streamer (`ws_client.js`):** Subscribes to `/api/v1/telemetry/stream` (HTTP-based Server-Sent Events) to deliver sub-10ms UI updates without polling overhead.
* **Dynamic MITRE ATT&CK Matrix (`threat_matrix.js`):** Aggregates detections from across all 26 edge modules and visualizes active tactics (`T0855`, `T1071`, `T1190`, `T1110`, `T1046`).

---

## 5. Prerequisites & System Dependencies

### Target Environments
* **Supported OS:** Ubuntu 22.04 LTS, Ubuntu 24.04 LTS, Ubuntu 26.04 LTS (x86_64 or aarch64)
* **Compiler:** Clang 16+ or GCC 12+ supporting native **C++20**
* **Hardware Allocation:** 4 vCPUs, 8 GB RAM, 20 GB dedicated storage.

### Required Host Packages
Install dependencies via `apt`:
```bash
sudo apt-get update && sudo apt-get install -y \
    build-essential \
    cmake \
    libssl-dev \
    libelf-dev \
    protobuf-compiler \
    libprotobuf-dev \
    libgrpc++-dev \
    protobuf-compiler-grpc \
    pkg-config \
    curl \
    git
```

---

## 6. Build & Installation Guide

### Step 1: Clone Repository
```bash
cd /home/kami
git clone https://github.com/kamisaberi/sentinel-nexus.git
cd sentinel-nexus
```

### Step 2: Generate Internal mTLS Certificates
Generate the Root CA, server certificates, and testing client keys:
```bash
cd configs/certs
chmod +x gen_certs.sh
./gen_certs.sh
cd ../..
```

### Step 3: Compile Sentinel Nexus & Operations CLI
Build the release binaries using CMake:
```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

This compiles:
* `sentinel-nexus` — The central command plane daemon.
* `nexus-ctl` — The standalone terminal administration CLI tool.
* `test_nexus_core` — The unit testing suite.
* `benchmark_ingest` — The high-throughput vector ingestion benchmark harness.

### Step 4: System Installation
Install binaries to system paths:
```bash
sudo make install
sudo ldconfig
```

---

## 7. Configuration Reference (`configs/nexus.yaml`)

Edit `configs/nexus.yaml` to customize ports, paths, and operational thresholds:

```yaml
server:
  bind_address: "0.0.0.0"
  grpc_port: 50051                  # Appliance gRPC ingestion port
  rest_api_port: 9443               # Web Command Center & REST API
  ws_stream_port: 9444              # Real-time SSE telemetry port
  worker_threads: 16                # Thread pool concurrency

tls:
  enabled: false                    # Set true for mTLS enforcement
  ca_cert: "configs/certs/ca.crt"
  server_cert: "configs/certs/server.crt"
  server_key: "configs/certs/server.key"

fleet:
  heartbeat_timeout_seconds: 15     # Grace window before marking nodes OFFLINE
  prune_offline_after_hours: 24     # Retention period for decommissioned node state

forge_bridge:
  enabled: true
  forge_ingest_path: "/var/lib/sentinel-nexus/forge_datasets/"
  uncertainty_threshold_min: 0.40   # Lower bound of active learning window
  uncertainty_threshold_max: 0.60   # Upper bound of active learning window
  max_buffer_vectors: 200000        # Maximum in-memory ring capacity
  batch_flush_size: 1000            # Sample limit before flushing batch to disk

intelligence:
  ioc_propagation_enabled: true     # Enable fleet-wide collective defense fan-out
  default_block_ttl_seconds: 86400  # Default kernel block expiration (24h)
```

---

## 8. Running the Command Plane

### Option A: Direct Foreground Execution
Run Nexus directly for testing and development:
```bash
cd /home/kami/sentinel-nexus
./build/sentinel-nexus configs/nexus.yaml
```

### Option B: Systemd Background Daemon
Install and manage Sentinel Nexus as a hardened Linux systemd service:
```bash
# 1. Copy service unit
sudo cp deploy/systemd/sentinel-nexus.service /etc/systemd/system/

# 2. Reload systemd and start service
sudo systemctl daemon-reload
sudo systemctl enable --now sentinel-nexus.service

# 3. Inspect service health and logs
sudo systemctl status sentinel-nexus.service
sudo journalctl -u sentinel-nexus.service -f
```

### Option C: Containerized Deployment
Run Sentinel Nexus inside an isolated Docker container:
```bash
cd deploy/docker
docker compose up -d --build
```

---

## 9. Operating the Web Command Center

Open any browser and navigate to:
```text
http://localhost:9443
```

### Key Interface Sections
1. **System Status Bar:** Displays active mTLS connection state and total online appliances.
2. **Key Metric Cards:**
   * **Active Appliances:** Current online nodes vs. total registered fleet size.
   * **Kernel Drops (eBPF):** Enterprise-wide cumulative packet drops enforced at driver hooks.
   * **Forge Candidate Samples:** High-utility vectors buffered for continuous retraining.
   * **Active ONNX Model:** Production model filename and active rollout stage (`SHADOW_MODE`, `CANARY_5_PCT`, `FLEET_WIDE`).
3. **Appliance Registry Matrix:** Live table displaying Node ID, Physical Site, Hostname, Health Status, CPU/Temp, eBPF Drop Counts, and Mitigation Latencies ($< 1.0\,\mu\text{s}$ SLA proof).
4. **Collective Defense Injector:** Interactive form allowing operators to inject an IP into the `blocked_ip_map` of all appliances fleet-wide in $< 50\,\text{ms}$.
5. **Model OTA Controls:** One-click management buttons to stage candidate models, advance canary rollout cohorts, or execute an emergency rollback.
6. **Fleet Topology Canvas:** Interactive radial visualization showing real-time health links and active attack lines.
7. **Aggregated MITRE ATT&CK Matrix:** Live heatmap showing tactic hit counters across connected sites.

---

## 10. Operations CLI Reference (`nexus-ctl`)

`nexus-ctl` is a native C++20 command-line administration tool for SOC analysts, incident response teams, and terminal workflows.

```bash
# ------------------------------------------------------------------------------
# FLEET MANAGEMENT
# ------------------------------------------------------------------------------
# List all registered appliances, sites, hardware types, and metrics
nexus-ctl fleet list

# ------------------------------------------------------------------------------
# COLLECTIVE DEFENSE INJECTION
# ------------------------------------------------------------------------------
# Broadcast an instant in-kernel eBPF drop rule across all connected appliances
nexus-ctl threat drop 198.51.100.44

# ------------------------------------------------------------------------------
# COMPLIANCE AUDITING & SLA REPORTING
# ------------------------------------------------------------------------------
# Display live CMMC 2.0 (Level 2) and NIST SP 800-171 compliance assessment
nexus-ctl report cmmc

# Display live IEC 62443 industrial control system security audit
nexus-ctl report scada

# ------------------------------------------------------------------------------
# MODEL OTA CANARY MANAGEMENT
# ------------------------------------------------------------------------------
# Check current ONNX model deployment status and rollout stage
nexus-ctl ota status

# Stage candidate weights into SHADOW_MODE
nexus-ctl ota stage

# Advance rollout stage (Shadow -> 5% Canary -> Fleet-Wide)
nexus-ctl ota advance

# Execute immediate emergency rollback to previous stable model
nexus-ctl ota rollback
```

---

## 11. REST API Documentation (`/api/v1/*`)

Nexus exposes an internal HTTP/REST API on port `9443` for integration with external dashboards, automated CI/CD runners, and SIEM forwarders:

| Method | Endpoint | Description | Request Body / Query |
| :--- | :--- | :--- | :--- |
| `GET` | `/api/v1/fleet/nodes` | Returns JSON array of all registered appliances and metrics | None |
| `GET` | `/api/v1/fleet/groups` | Returns JSON array of logical enclave groups and assigned nodes | None |
| `GET` | `/api/v1/reports/compliance` | Returns aggregated drop totals, active model, and SLA verification | None |
| `GET` | `/api/v1/reports/cmmc` | Returns structured CMMC 2.0 / NIST SP 800-171 findings | None |
| `GET` | `/api/v1/reports/scada` | Returns structured IEC 62443 industrial audit proof | None |
| `GET` | `/api/v1/threats/mitre` | Returns aggregated MITRE ATT&CK technique counts | None |
| `POST`| `/api/v1/threats/broadcast` | Broadcasts an attacker IP fleet-wide for instant eBPF kernel drops | `{"ip": "198.51.100.44"}` |
| `GET` | `/api/v1/ota/status` | Returns active stable version, candidate version, and rollout stage | None |
| `POST`| `/api/v1/ota/stage` | Stages candidate weights into `SHADOW_MODE` | `{"version": "v2.onnx", "sha256": "..."}` |
| `POST`| `/api/v1/ota/advance` | Advances candidate model to next rollout stage | None |
| `POST`| `/api/v1/ota/rollback` | Triggers emergency model rollback across all appliances | None |
| `GET` | `/api/v1/models` | Lists available ONNX models, file sizes, and download URLs | None |
| `GET` | `/models/{filename}` | Serves raw `.onnx` binary files for edge appliance auto-downloads | None |
| `GET` | `/api/v1/telemetry/stream` | Server-Sent Events (SSE) persistent stream pushing real-time alerts | None |

---

## 12. Air-Gapped Sneakernet Operations (`tools/sneakernet/`)

For classified, naval, or physically isolated enclaves that prohibit external WAN links, Nexus supports cryptographically signed sneakernet synchronization:

### Exporting Edge Telemetry to Encrypted USB Media
Run this script on an air-gapped Nexus node to package accumulated candidate vectors into a compressed, SHA-256 signed `.snbundle`:
```bash
python3 tools/sneakernet/export_telemetry_bundle.py
```
*Output: `sneakernet_bundles/nexus_telemetry_bundle_<timestamp>.snbundle`*

### Importing and Staging Models in Air-Gapped Facilities
When an updated model has been trained on an external workstation, transfer the `.onnx` file via secure media and stage it into the air-gapped Nexus instance:
```bash
python3 tools/sneakernet/import_signed_model.py path/to/network_threat_v2.onnx
```
*This verifies the cryptographic checksum, caches the model in `/opt/sentinel-nexus/models/`, and transitions it into `SHADOW_MODE` automatically.*

---

## 13. Testing, Simulation & Benchmarking

### 13.1 Automated Core Unit Tests
Execute the self-contained C++20 test suite verifying node registration, attestation logic, enclave grouping, and the Canary state machine:
```bash
cd build
./test_nexus_core
```
*Expected: `ALL UNIT TESTS PASSED (100% Assertion Success)`*

### 13.2 Vector Ingestion Throughput Benchmark
Measures the lock-free ingestion performance of high-uncertainty NetFlow vectors into the `ForgeBridge` batching pipeline:
```bash
cd build
./benchmark_ingest
```
*Target: **$> 500{,}000$ vectors/second** sustained throughput.*

### 13.3 Multi-Appliance Fleet Simulator
Simulate 5 active edge appliances reporting heartbeats, streaming uncertain vectors, and receiving model updates:
```bash
python3 tools/mock_appliance/simulate_fleet.py
```

### 13.4 Collective Defense & Auto-Rollback Attack Wave Validation
Simulates a 10-node mesh, executes a zero-day exploit against Node 01, verifies sub-50ms fan-out across Nodes 02–10, and triggers an SLA latency violation ($1{,}540\,\mu\text{s} > 1{,}000\,\mu\text{s}$) to verify automated emergency rollback:
```bash
python3 tools/simulate_attack_wave.py
```

---

## 14. Operational Troubleshooting

| Symptom / Error | Root Cause | Remediation |
| :--- | :--- | :--- |
| `Address already in use (50051 / 9443)` | A previous instance of `sentinel-nexus` is already running in the background. | Run `sudo pkill -f sentinel-nexus` or check port ownership with `sudo lsof -i :50051 -i :9443`. |
| HTTP Web UI returns `404 Not Found` | The executable was launched from `build/` and could not locate the `web/` directory. | The internal HTTP server includes multi-path search fallback, but ensure `./web/index.html` exists in the project root or specify absolute paths in `nexus.yaml`. |
| Edge nodes remain `OFFLINE` | Appliance hostname or machine UUID was rejected by `AttestationValidator`. | Check the appliance kernel release and UUID strings. If UUID revocation was tested, clear `revoked_uuids_` or delete `data/nexus_state.json`. |
| SSE Stream Disconnects | Client socket was prematurely closed during a standard request cycle. | Ensure `/api/v1/telemetry/stream` is intercepted before standard socket termination in `HttpServer::handle_client` (delegated to `TelemetryStreamer`). |
| Missing host libraries in Docker container | Host compiler used newer shared object versions (`libabsl`, `libre2`). | Mount the host's `/usr/local/lib/matrix-deps` volume or run `make init` in `sentinel-matrix` to bundle dynamic libraries. |

---

## 15. Security & Sovereignty Guarantees

* **Zero Cloud Data Egress:** Sentinel Nexus operates 100% on-premises. High-value site NetFlow vectors and raw packet payload buffers never leave local infrastructure.
* **Deterministic Native Fast-Paths:** All critical coordination paths are engineered in native C++20 with zero reliance on managed runtimes (no Python or JVM in the core execution path).
* **Hardware-Anchored Trust:** Integrates directly with physical TPM 2.0 cryptoprocessors (TCG TSS2 specification) to ensure cryptographic attestation across the fleet.
* **Air-Gapped Web Management:** The Web Command Center enforces strict CSP standards with zero external script tags, Google Fonts, or public CDN calls.
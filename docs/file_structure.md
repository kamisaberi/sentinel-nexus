### Project Name: `sentinel-nexus` (Tier 6)

**Rationale:**  
* **Sentinel:** Retains ecosystem lineage with edge nodes (`blackbox-sentinel`).
* **Nexus:** Signifies the central convergence point where distributed telemetry, fleet intelligence, continuous retraining pipelines (`xinfer-forge`), and kernel policy propagation meet.
* **Alternative options:** `sentinel-commander`, `blackbox-apex`, or `sentinel-grid`.

---

### File Structure: `sentinel-nexus`

Built with native **C++20** (consistent with Tiers 1–3), **gRPC/Protobuf** for bidirectional appliance streaming, an embedded air-gapped web console, and a storage engine optimized for telemetry and vector batching (DuckDB / RocksDB).

```text
sentinel-nexus/
├── CMakeLists.txt
├── README.md
│
├── configs/                                # Central Server Configurations
│   ├── nexus.yaml                          # Main daemon config (ports, storage, thresholds)
│   ├── canary_policy.yaml                  # Model rollout stages (5% -> 25% -> 100%)
│   ├── compliance_profiles.yaml            # CMMC Level 2, NIST SP 800-171, IEC 62443 rules
│   └── certs/                              # mTLS Root CA & Server Certificates
│       ├── ca.crt
│       ├── server.crt
│       └── server.key
│
├── proto/                                  # Universal gRPC / Wire Definitions
│   ├── common.proto                        # Shared types (NodeID, Timestamps, HardwareIdentity)
│   ├── fleet.proto                         # Heartbeats, node registration, TPM attestation
│   ├── telemetry.proto                     # Hard-negative vectors, anomaly batching for Forge
│   ├── intelligence.proto                  # IoC broadcast ("Attacked Once, Immune Everywhere")
│   └── model_ota.proto                     # ONNX model distribution, shadow testing, rollbacks
│
├── src/
│   ├── main.cpp                            # Nexus daemon entrypoint & signal traps
│   │
│   ├── core/                               # Core Engine Infrastructure
│   │   ├── ServerContext.hpp
│   │   ├── ConfigManager.hpp / .cpp
│   │   ├── ThreadPool.hpp
│   │   └── Logger.hpp
│   │
│   ├── fleet/                              # Objective #3: Fleet Availability & Health
│   │   ├── NodeRegistry.hpp / .cpp         # In-memory matrix of active/offline appliances
│   │   ├── HeartbeatMonitor.hpp / .cpp     # Liveness tracker, packet-drop counters, NPU load
│   │   ├── AttestationValidator.hpp / .cpp # TPM 2.0 Quote & DMI UUID cryptographic verification
│   │   └── GroupManager.hpp / .cpp         # Logical clustering (e.g., "Factory-A", "Substation-9")
│   │
│   ├── telemetry/                          # Objective #1: Data Collection for xinfer-forge
│   │   ├── VectorIngestQueue.hpp / .cpp    # Lock-free ingestion queue for incoming feature streams
│   │   ├── UncertaintySampler.hpp / .cpp   # Filters high-uncertainty / novel outlier vectors
│   │   ├── DatasetCurator.hpp / .cpp       # Packages vectors into training batches
│   │   └── ForgeBridge.hpp / .cpp          # Feeds curated datasets directly to xinfer-forge
│   │
│   ├── intelligence/                       # Real-Time Collective Defense
│   │   ├── GlobalThreatCache.hpp / .cpp    # Distributed registry of known attack signatures
│   │   ├── IocBroadcaster.hpp / .cpp       # Sub-50ms fan-out to all appliances' eBPF blocked_ip_map
│   │   └── MitreAggregator.hpp / .cpp      # Global correlation across 26 subsystem event types
│   │
│   ├── ota/                                # Safe Model Over-The-Air Lifecycle
│   │   ├── ModelRepository.hpp / .cpp      # Versioned storage for ONNX models (v1, v2, candidate)
│   │   ├── CanaryOrchestrator.hpp / .cpp   # Staged rollouts (shadow mode -> canary -> full fleet)
│   │   └── RollbackGuard.hpp / .cpp        # Auto-triggers rollback if candidate drift is detected
│   │
│   ├── reporting/                          # Objective #2: Security Reporting & Compliance
│   │   ├── ReportGenerator.hpp / .cpp      # Executive PDF / JSON summary compiler
│   │   ├── CmmcAuditEngine.hpp / .cpp      # CMMC 2.0 / NIST 800-171 compliance verification
│   │   ├── ScadaAuditEngine.hpp / .cpp     # IEC 62443 / OT safety proof generator
│   │   └── LatencySlaReporter.hpp / .cpp   # Verifies sub-millisecond edge mitigation SLAs
│   │
│   ├── rpc/                                # gRPC Service Implementations
│   │   ├── FleetServiceImpl.hpp / .cpp
│   │   ├── TelemetryServiceImpl.hpp / .cpp
│   │   └── IntelligenceServiceImpl.hpp / .cpp
│   │
│   ├── api/                                # REST & WebSocket Engine (Port 9443 / 9444)
│   │   ├── RestServer.hpp / .cpp           # REST router for external integrations
│   │   ├── WsTelemetryStream.hpp / .cpp    # Real-time WebSocket multiplexer for Web UI
│   │   └── controllers/
│   │       ├── FleetController.hpp
│   │       ├── ThreatController.hpp
│   │       └── ModelController.hpp
│   │
│   └── storage/                            # Embedded Persistence Layer
│       ├── StateDatabase.hpp / .cpp        # SQLite / RocksDB for node state and audit logs
│       └── TimeSeriesEngine.hpp / .cpp     # Embedded DuckDB for lightning-fast telemetry analytics
│
├── web/                                    # Air-Gapped Master Web Command Center
│   ├── index.html                          # Single-Page Application (SPA)
│   ├── css/
│   │   ├── dashboard.css
│   │   └── topology_map.css
│   ├── js/
│   │   ├── app.js                          # Core UI engine
│   │   ├── ws_client.js                    # Live telemetry stream handler
│   │   ├── fleet_topology.js               # Visual node grid (NPU/GPU/CPU/drops)
│   │   ├── threat_matrix.js                # Aggregated MITRE ATT&CK visualization
│   │   └── ota_manager.js                  # 1-Click model rollout & rollback controls
│   └── assets/                             # Zero-CDN embedded fonts, icons, branding
│
├── tools/                                  # Utilities & Offline Operations
│   ├── nexus-ctl/                          # Terminal CLI for operations engineers
│   │   ├── main.go or main.cpp
│   ├── sneakernet/                         # Air-Gapped / Isolated Enclave Tooling
│   │   ├── export_telemetry_bundle.py      # Dumps vectors to encrypted physical media
│   │   └── import_signed_model.py          # Verifies cryptographic signature of offline models
│   └── mock_appliance/                     # Stress testing & fleet simulator
│       └── simulate_fleet.py               # Simulates 500 active appliances reporting via gRPC
│
└── tests/
    ├── unit/
    ├── integration/
    └── benchmark/                          # Benchmarks gRPC ingestion @ 500k events/sec
```

---

### Architectural Highlights of this Layout

1. **Strict Decoupling from Edge Appliances:**  
   Edge nodes (`blackbox-sentinel`) communicate via generated stubs in `proto/`. If an appliance loses WAN connection, it continues running autonomously with its local eBPF and `libxinfer` engine without dropping packets.
2. **Dedicated Pipeline to `xinfer-forge` (`src/telemetry/ForgeBridge.hpp`):**  
   Curated vectors (uncertainties, novelties, hard negatives) bypass raw log storage and stream directly into Forge's continuous learning loop.
3. **Hardware Trust Rooted in Tier 2 (`src/fleet/AttestationValidator.hpp`):**  
   Validates the TPM 2.0 identity created in `blackbox-essential` before admitting any appliance to the network.
4. **Isolated Air-Gapped Support (`tools/sneakernet/`):**  
   Provides a clean path for secure sites that can never establish an outbound network connection to the central server.
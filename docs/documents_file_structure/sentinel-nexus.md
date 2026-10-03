# Project 6 of 8: `sentinel-nexus` (`sentinel-nexus`)
## Complete Documentation & Help Desk System File Structure (`docs/`)

This is the complete, full-version documentation tree for **`sentinel-nexus`**. It is formatted for enterprise documentation engines (**MkDocs Material**, **Docusaurus**, or **Starlight**) and covers distributed fleet coordination, the **sub-50ms collective defense bus**, **active learning dataset curation**, **Canary OTA staged rollouts with automated RollbackGuard**, **Microsecond Residual XAI feature attribution**, the **4-tier hierarchical asset topology**, and the **air-gapped Web Command Center**.

---

```text
sentinel-nexus/docs/
├── mkdocs.yml                                 # Documentation site configuration (navigation, theme, search)
├── index.md                                   # Documentation home & command plane overview
│
├── getting-started/                           # Onboarding & Setup
│   ├── overview.md                            # Central fleet command plane & collective defense architecture
│   ├── system-requirements.md                 # Ubuntu 22.04/24.04/26.04, gRPC, Protobuf, OpenSSL, and RAM
│   ├── installation-and-build.md              # Building daemon, nexus-ctl CLI, tests, and benchmarks
│   ├── ten-minute-quickstart.md               # Booting Nexus and connecting your first edge appliance
│   ├── systemd-deployment.md                  # Hardened systemd production service deployment
│   ├── docker-deployment.md                   # Containerized execution & docker-compose configurations
│   ├── verifying-services.md                  # Validating gRPC (50051), REST (9443), and SSE (9444) ports
│   └── architecture-at-a-glance.md            # High-level diagram: 5,000 Appliances <-> Nexus <-> Forge
│
├── architecture/                              # Deep Systems Design
│   ├── command-plane-architecture.md          # Multi-threaded core engine & thread pool architecture
│   ├── ports-and-protocols-matrix.md          # gRPC HTTP/2 (50051) vs. REST (9443) vs. SSE Stream (9444)
│   ├── in-memory-state-engine.md              # Read-heavy shared_mutex synchronization & NodeRegistry
│   ├── data-persistence-model.md              # StateDatabase (nexus_state.json) & TimeSeriesEngine
│   ├── air-gapped-sovereignty.md              # Operating without WAN egress & zero external CDN scripts
│   └── high-availability-clustering.md        # Multi-Nexus cluster design and failover topology
│
├── collective-defense/                        # Collective Immunity Subsystem
│   ├── collective-defense-overview.md         # Sub-50ms "Attacked Once, Immune Everywhere" paradigm
│   ├── ioc-broadcaster-mechanics.md           # Asynchronous parallel gRPC distribution engine
│   ├── sub-50ms-fanout-timeline.md            # Nanosecond-by-nanosecond timeline: Ingress -> Drop -> Fanout
│   ├── originator-loop-suppression.md         # Preventing redundant drop loops back to reporting nodes
│   ├── fleet-defense-rule-schema.md           # Structure of FleetDefenseRule & ephemeral TTL management
│   └── emergency-ip-purge.md                  # Global false-positive unblocking across 5,000 kernel maps
│
├── active-learning-pipeline/                  # Forge Continuous Retraining Bridge
│   ├── active-learning-architecture.md        # Edge vector streaming to Forge dataset generation
│   ├── vector-ingest-queue.md                 # Concurrent, lock-free ring buffer (200k vector capacity)
│   ├── uncertainty-sampling-rules.md          # Ingesting vectors within the [0.40 - 0.60] entropy window
│   ├── dataset-curator-engine.md              # Packaging binary batches into forge_dataset_*.csv files
│   ├── forge-trigger-automation.md            # Detecting batch quotas and firing background retraining
│   └── closed-loop-flywheel-testing.md        # Verifying autonomous retraining without human labeling
│
├── canary-ota-rollout/                        # Staged Model Deployment & SLA Safety
│   ├── staged-rollout-lifecycle.md            # State machine: SHADOW_MODE -> CANARY_5_PCT -> FLEET_WIDE
│   ├── canary-orchestrator-engine.md          # Hash-based 5% cohort selection and version management
│   ├── rollback-guard-sla-watchdog.md         # Automated emergency rollback on >1000µs SLA latency breach
│   ├── false-positive-surge-protection.md     # Auto-aborting candidate models on abnormal drop bursts
│   ├── model-repository-and-hashing.md        # Local ONNX storage, HTTP streaming, and SHA-256 validation
│   └── zero-downtime-hot-reload-flow.md       # Triggering edge reloads without packet loss
│
├── explainable-ai-xai/                        # Edge XAI Attribution Aggregator
│   ├── xai-aggregator-overview.md             # Ingesting Microsecond Residual Decomposition (MRD) vectors
│   ├── top-3-feature-attribution-schema.md    # Schema: feature_name, contribution_pct, observed, baseline
│   ├── semantic-dictionary-mapping.md         # Translating 32 tensor dimensions to physical SCADA metrics
│   ├── global-threat-cache-indexing.md        # Indexing XAI records alongside MITRE ATT&CK taxonomy
│   ├── xai-api-endpoints.md                   # Querying /api/v1/threats/xai for compliance auditors
│   └── rendering-xai-in-web-and-tui.md        # Live UI visualization: Impact percentage bars & audit notes
│
├── hierarchical-asset-topology/               # 4-Tier Enterprise Asset Model
│   ├── four-tier-hierarchy-model.md           # Tenant -> Nexus Hub -> Sentinel Node -> Sensor/PLC
│   ├── deterministic-sensor-identifiers.md    # Deriving IDs for Modbus PLCs, Coils, DICOM, and Cameras
│   ├── cascading-health-engine.md             # Health states: ONLINE, DEGRADED, OFFLINE, UNREACHABLE
│   ├── liveness-heartbeat-tracking.md         # 15-second grace window and timeout transitions
│   ├── instant-0ms-graceful-disconnect.md     # Handling DeregistrationRequest on SIGINT/Ctrl+C
│   └── fleet-sync-payload-schema.md           # Nested JSON structure for POST /api/v1/fleet/sync
│
├── web-command-center/                        # Air-Gapped Web Management (Ports 9443 & 9444)
│   ├── web-console-architecture.md            # Zero-dependency, zero-CDN Single-Page Application (SPA)
│   ├── radial-topology-canvas.md              # HTML5 Canvas real-time radial node visualizer (fleet_topology.js)
│   ├── mitre-attack-heatmap.md                # Dynamic MITRE ATT&CK tactical matrix (threat_matrix.js)
│   ├── real-time-sse-stream.md                # Sub-10ms Server-Sent Events push engine (ws_client.js)
│   ├── active-kpi-telemetry-cards.md          # Live display: Online nodes, eBPF drops, SLA, active model
│   ├── collective-defense-injector-ui.md      # Manual 1-click IP broadcast tool from web browser
│   └── ota-canary-management-ui.md            # Interactive staging, advancing, and rollback controls
│
├── operations-cli-nexus-ctl/                  # Standalone Administration CLI
│   ├── nexus-ctl-overview.md                  # C++20 terminal admin tool syntax and flags
│   ├── command-fleet-list.md                  # `nexus-ctl fleet list`: Node health, CPU, drops, latency SLA
│   ├── command-threat-drop.md                 # `nexus-ctl threat drop <IP>`: Manual fleet-wide kernel drop
│   ├── command-ota-management.md              # `nexus-ctl ota [status|stage|advance|rollback]`
│   ├── command-compliance-reports.md          # `nexus-ctl report [cmmc|scada]`: Audit evaluation checks
│   └── command-auth-login.md                  # `nexus-ctl auth login [email] [pass]`: Cloud JWT acquisition
│
├── rest-api-reference/                        # Complete HTTP/REST API Specification (/api/v1/*)
│   ├── api-overview.md                        # Base URLs, headers (X-Tenant-ID, Authorization), and error codes
│   ├── endpoints-fleet.md                     # `/api/v1/fleet/nodes` and `/api/v1/fleet/groups`
│   ├── endpoints-threats.md                   # `/api/v1/threats/broadcast`, `/threats/mitre`, `/threats/xai`
│   ├── endpoints-ota-models.md                # `/api/v1/ota/*`, `/api/v1/models`, and `/models/{file}`
│   ├── endpoints-compliance.md                # `/api/v1/reports/compliance`, `/reports/cmmc`, `/reports/scada`
│   └── endpoints-sse-stream.md                # `/api/v1/telemetry/stream` (Persistent EventStream)
│
├── cloud-saas-uplink/                         # Hybrid SaaS Connector (Outbound Client)
│   ├── saas-connector-architecture.md         # Decoupled C++ outbound HTTPS client (SaaSConnector.cpp)
│   ├── jwt-authentication-and-renewal.md      # Initial login, JWT caching (cloud_session.json), and 401 retry
│   ├── cloud-fleet-sync.md                    # POST /fleet/sync nested 4-tier tree transmission every 5s
│   ├── inbound-global-threat-feed.md          # Polling /threats/global-feed and injecting into local eBPF
│   └── remote-ciso-commands.md                # Polling /commands/pending for cloud-initiated emergency rollbacks
│
├── compliance-engines/                        # Regulatory & Industrial GRC
│   ├── cmmc-2.0-audit-engine.md               # Verifying AC.L2-3.1.1, IA.L2-3.5.1, and SI.L2-3.14.1
│   ├── iec-62443-audit-engine.md              # Verifying FR 3 (System Integrity) & FR 5 (Zone Segmentation)
│   ├── latency-sla-percentile-proofs.md       # Microsecond percentile proofs: p50 (0.84µs) to p99.9 (1.04µs)
│   └── tamper-evident-audit-logging.md        # Cryptographic state journaling in data/nexus_state.json
│
├── tutorials/                                 # Practical Administrative Guides
│   ├── scaling-to-5000-appliances.md          # Tuning Linux TCP buffers, worker threads, and file descriptors
│   ├── setting-up-mtls-pki.md                 # Generating CA, server, and appliance certificates with gen_certs.sh
│   ├── handling-zero-day-incident-in-seconds.md# Walkthrough of a coordinated multi-site attack containment
│   ├── integrating-fastapi-cloud-backend.md   # Connecting local Nexus instances to app.aryorithm.com
│   └── air-gapped-sneakernet-sync.md          # Using export_telemetry_bundle.py & import_signed_model.py
│
└── troubleshooting/                           # Help Desk & Diagnostics
    ├── port-binding-and-socket-errors.md      # Fixing "Address already in use" on 50051, 9443, and 9444
    ├── appliance-registration-rejections.md   # Debugging rejected hardware identities and revoked UUIDs
    ├── jwt-auth-401-failures.md               # Resolving cloud backend authentication and token expiry issues
    ├── sse-stream-disconnects.md              # Fixing browser stream drops and proxy buffer timeouts
    ├── config-file-parsing-errors.md          # Resolving inline YAML comment stripping in ConfigManager.cpp
    ├── faq.md                                 # Technical Frequently Asked Questions
    └── support.md                             # Issue tracker, security disclosures, and enterprise support SLAs
```

---

*This concludes the complete documentation system structure for **Project 6: `sentinel-nexus`**.*  
*Ready to proceed to **Project 7: `sentinel-matrix` (`sentinel-matrix` autonomous cyber-range & simulation mesh)** upon your confirmation.*
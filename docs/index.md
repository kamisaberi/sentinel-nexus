### Part 1: Root Configuration & Getting Started (`mkdocs.yml`, `index.md`, and `getting-started/*`)

This initial set of 10 files establishes the full documentation engine configuration, the executive command plane overview, and the complete onboarding track for **`sentinel-nexus`** (`sentinel-nexus`).

---

### File: `sentinel-nexus/docs/mkdocs.yml`

```yaml
site_name: Sentinel-Nexus Documentation
site_description: Central Fleet Command Plane, Sub-50ms Collective Defense Grid, and Active Learning Orchestrator (Tier 6)
site_author: Aryorithm Technologies B.V.
site_url: https://docs.aryorithm.com/nexus/
repo_name: kamisaberi/sentinel-nexus
repo_url: https://github.com/kamisaberi/sentinel-nexus

theme:
  name: material
  language: en
  palette:
    - scheme: slate
      primary: cyan
      accent: teal
      toggle:
        icon: material/weather-night
        name: Switch to light mode
    - scheme: default
      primary: cyan
      accent: teal
      toggle:
        icon: material/weather-sunny
        name: Switch to dark mode
  features:
    - navigation.instant
    - navigation.tracking
    - navigation.tabs
    - navigation.sections
    - navigation.expand
    - navigation.top
    - search.suggest
    - search.highlight
    - content.code.copy
    - content.code.annotate

plugins:
  - search

markdown_extensions:
  - admonition
  - pymdownx.details
  - pymdownx.superfences:
      custom_fences:
        - name: mermaid
          class: mermaid
          format: !!python/name:pymdownx.superfences.fence_code_format
  - pymdownx.highlight:
      anchor_linenums: true
      line_spans: __span
      pygments_lang_class: true
  - pymdownx.inlinehilite
  - pymdownx.tabbed:
      alternate_style: true
  - pymdownx.arithmatex:
      generic: true
  - tables
  - attr_list
  - md_in_html

extra_javascript:
  - https://polyfill.io/v3/polyfill.min.js?features=es6
  - https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js

nav:
  - Home: index.md
  - Getting Started:
      - Overview: getting-started/overview.md
      - System Requirements: getting-started/system-requirements.md
      - Installation & Build: getting-started/installation-and-build.md
      - 10-Minute Quickstart: getting-started/ten-minute-quickstart.md
      - Systemd Deployment: getting-started/systemd-deployment.md
      - Docker Deployment: getting-started/docker-deployment.md
      - Verifying Services: getting-started/verifying-services.md
      - Architecture at a Glance: getting-started/architecture-at-a-glance.md
  - Architecture:
      - Command Plane Architecture: architecture/command-plane-architecture.md
      - Ports & Protocols Matrix: architecture/ports-and-protocols-matrix.md
      - In-Memory State Engine: architecture/in-memory-state-engine.md
      - Data Persistence Model: architecture/data-persistence-model.md
      - Air-Gapped Sovereignty: architecture/air-gapped-sovereignty.md
      - High-Availability Clustering: architecture/high-availability-clustering.md
  - Collective Defense:
      - Collective Defense Overview: collective-defense/collective-defense-overview.md
      - IoC Broadcaster Mechanics: collective-defense/ioc-broadcaster-mechanics.md
      - Sub-50ms Fanout Timeline: collective-defense/sub-50ms-fanout-timeline.md
      - Originator Loop Suppression: collective-defense/originator-loop-suppression.md
      - Fleet Defense Rule Schema: collective-defense/fleet-defense-rule-schema.md
      - Emergency IP Purge: collective-defense/emergency-ip-purge.md
  - Active Learning Pipeline:
      - Active Learning Architecture: active-learning-pipeline/active-learning-architecture.md
      - Vector Ingest Queue: active-learning-pipeline/vector-ingest-queue.md
      - Uncertainty Sampling Rules: active-learning-pipeline/uncertainty-sampling-rules.md
      - Dataset Curator Engine: active-learning-pipeline/dataset-curator-engine.md
      - Forge Trigger Automation: active-learning-pipeline/forge-trigger-automation.md
      - Closed-Loop Flywheel Testing: active-learning-pipeline/closed-loop-flywheel-testing.md
  - Canary OTA Rollout:
      - Staged Rollout Lifecycle: canary-ota-rollout/staged-rollout-lifecycle.md
      - Canary Orchestrator Engine: canary-ota-rollout/canary-orchestrator-engine.md
      - RollbackGuard SLA Watchdog: canary-ota-rollout/rollback-guard-sla-watchdog.md
      - False-Positive Surge Protection: canary-ota-rollout/false-positive-surge-protection.md
      - Model Repository & Hashing: canary-ota-rollout/model-repository-and-hashing.md
      - Zero-Downtime Hot Reload: canary-ota-rollout/zero-downtime-hot-reload-flow.md
  - Explainable AI (XAI):
      - XAI Aggregator Overview: explainable-ai-xai/xai-aggregator-overview.md
      - Top-3 Feature Attribution Schema: explainable-ai-xai/top-3-feature-attribution-schema.md
      - Semantic Dictionary Mapping: explainable-ai-xai/semantic-dictionary-mapping.md
      - Global Threat Cache Indexing: explainable-ai-xai/global-threat-cache-indexing.md
      - XAI API Endpoints: explainable-ai-xai/xai-api-endpoints.md
      - Rendering XAI in Web & TUI: explainable-ai-xai/rendering-xai-in-web-and-tui.md
  - Hierarchical Asset Topology:
      - 4-Tier Hierarchy Model: hierarchical-asset-topology/four-tier-hierarchy-model.md
      - Deterministic Sensor Identifiers: hierarchical-asset-topology/deterministic-sensor-identifiers.md
      - Cascading Health Engine: hierarchical-asset-topology/cascading-health-engine.md
      - Liveness Heartbeat Tracking: hierarchical-asset-topology/liveness-heartbeat-tracking.md
      - Instant 0ms Disconnect: hierarchical-asset-topology/instant-0ms-graceful-disconnect.md
      - Fleet Sync Payload Schema: hierarchical-asset-topology/fleet-sync-payload-schema.md
  - Web Command Center:
      - Web Console Architecture: web-command-center/web-console-architecture.md
      - Radial Topology Canvas: web-command-center/radial-topology-canvas.md
      - MITRE ATT&CK Heatmap: web-command-center/mitre-attack-heatmap.md
      - Real-Time SSE Stream: web-command-center/real-time-sse-stream.md
      - Active KPI Telemetry Cards: web-command-center/active-kpi-telemetry-cards.md
      - Collective Defense Injector UI: web-command-center/collective-defense-injector-ui.md
      - OTA Canary Management UI: web-command-center/ota-canary-management-ui.md
  - Operations CLI (nexus-ctl):
      - nexus-ctl Overview: operations-cli-nexus-ctl/nexus-ctl-overview.md
      - nexus-ctl fleet list: operations-cli-nexus-ctl/command-fleet-list.md
      - nexus-ctl threat drop: operations-cli-nexus-ctl/command-threat-drop.md
      - nexus-ctl ota: operations-cli-nexus-ctl/command-ota-management.md
      - nexus-ctl report: operations-cli-nexus-ctl/command-compliance-reports.md
      - nexus-ctl auth login: operations-cli-nexus-ctl/command-auth-login.md
  - REST API Reference:
      - API Overview: rest-api-reference/api-overview.md
      - Fleet Endpoints: rest-api-reference/endpoints-fleet.md
      - Threat Endpoints: rest-api-reference/endpoints-threats.md
      - OTA Model Endpoints: rest-api-reference/endpoints-ota-models.md
      - Compliance Endpoints: rest-api-reference/endpoints-compliance.md
      - SSE Stream Endpoint: rest-api-reference/endpoints-sse-stream.md
  - Cloud SaaS Uplink:
      - SaaS Connector Architecture: cloud-saas-uplink/saas-connector-architecture.md
      - JWT Authentication & Renewal: cloud-saas-uplink/jwt-authentication-and-renewal.md
      - Cloud Fleet Sync: cloud-saas-uplink/cloud-fleet-sync.md
      - Global Threat Feed: cloud-saas-uplink/inbound-global-threat-feed.md
      - Remote CISO Commands: cloud-saas-uplink/remote-ciso-commands.md
  - Compliance Engines:
      - CMMC 2.0 Audit Engine: compliance-engines/cmmc-2.0-audit-engine.md
      - IEC 62443 Audit Engine: compliance-engines/iec-62443-audit-engine.md
      - Latency SLA Percentile Proofs: compliance-engines/latency-sla-percentile-proofs.md
      - Tamper-Evident Audit Logging: compliance-engines/tamper-evident-audit-logging.md
  - Tutorials:
      - Scaling to 5,000 Appliances: tutorials/scaling-to-5000-appliances.md
      - Setting up mTLS PKI: tutorials/setting-up-mtls-pki.md
      - Zero-Day Containment in Seconds: tutorials/handling-zero-day-incident-in-seconds.md
      - FastAPI Cloud Integration: tutorials/integrating-fastapi-cloud-backend.md
      - Air-Gapped Sneakernet Sync: tutorials/air-gapped-sneakernet-sync.md
  - Troubleshooting:
      - Port Binding & Socket Errors: troubleshooting/port-binding-and-socket-errors.md
      - Registration Rejections: troubleshooting/appliance-registration-rejections.md
      - JWT 401 Auth Failures: troubleshooting/jwt-auth-401-failures.md
      - SSE Stream Disconnects: troubleshooting/sse-stream-disconnects.md
      - Config Parsing Errors: troubleshooting/config-file-parsing-errors.md
      - FAQ: troubleshooting/faq.md
      - Support & SLAs: troubleshooting/support.md
```

---

### File: `sentinel-nexus/docs/index.md`

```markdown
# Sentinel-Nexus (`sentinel-nexus`)

**Central Fleet Command Plane, Collective Defense Grid & Continual AI Orchestrator**  
*Tier 6 Master Command Tier of the Aryorithm / Blackbox Sentinel Ecosystem*

---

## Executive Architectural Overview

`sentinel-nexus` is the high-performance, central fleet command plane and collective defense engine of the Aryorithm ecosystem. Implemented in native ISO C++20, it coordinates and defends up to **5,000 edge appliances (`blackbox-sentinel`)** across globally distributed or sovereign air-gapped industrial facilities.

At the core of `sentinel-nexus` is the **Sub-50ms Collective Defense Bus**: when any edge appliance detects and drops an active zero-day attack, Nexus broadcasts the threat indicator fleet-wide, programming all 5,000 remote Linux kernel eBPF tables in under $50\,\text{milliseconds}$ (*"Attacked Once, Immune Everywhere"*).

```text
====================================================================================================
                        SENTINEL-NEXUS FLEET COMMAND PLANE ARCHITECTURE
====================================================================================================
 [FLEET APPLIANCES]             UP TO 5,000 EDGE NODES (blackbox-sentinel daemons)
                                 ▲                │
            gRPC Port 50051      │ Telemetry Sync │ Collective Defense Rules (< 50ms)
            Mutual TLS 1.3       │ Heartbeats     ▼ Canary OTA Model Rollouts
 ┌───────────────────────────────┴─────────────────────────────────────────────────────────────────┐
 │ SENTINEL-NEXUS DAEMON (/usr/local/bin/sentinel-nexus)                                           │
 │                                                                                                 │
 │  ┌──────────────────────────────────────────────┐  ┌────────────────────────────────────────┐  │
 │  │ Sub-50ms Collective Defense Grid             │  │ DatasetCurator (Active Learning Bridge)│  │
 │  │ • Asynchronous Parallel gRPC Broadcast       │  │ • Uncertainty Sampling [0.40 - 0.60]   │  │
 │  │ • Originator Loopback Suppression            │  │ • Packages forge_dataset_*.csv Batches │  │
 │  └──────────────────────────────────────────────┘  └───────────────────┬────────────────────┘  │
 │                                                                        │ Inotify / REST Trigger│
 │  ┌──────────────────────────────────────────────┐                      ▼                       │
 │  │ Canary OTA Rollout Engine & RollbackGuard    │     ┌─────────────────────────────────────┐  │
 │  │ • Shadow Mode -> 5% Canary -> Fleet-Wide     │     │ Tier 4: xinfer-forge (forge-cli)    │  │
 │  │ • Automated Abort on >1000µs SLA Latency     │     │ • Self-Supervised Tabular MAE       │  │
 │  └──────────────────────────────────────────────┘     │ • Golden Attacks Safety Gate (100%) │  │
 │                                                       └─────────────────────────────────────┘  │
 │  ┌──────────────────────────────────────────────┐  ┌────────────────────────────────────────┐  │
 │  │ 4-Tier Hierarchical Asset Topology           │  │ Microsecond Residual XAI Aggregator    │  │
 │  │ Tenant -> Nexus Hub -> Sentinel -> Sensors   │  │ Top-3 Feature Attributions (< 80ns MRD)│  │
 │  └──────────────────────────────────────────────┘  └────────────────────────────────────────┘  │
 │                                                                                                │
 │  ┌──────────────────────────────────────────────┐  ┌────────────────────────────────────────┐  │
 │  │ Air-Gapped Web Command Center (Port 9443)    │  │ Real-Time SSE Push Stream (Port 9444)  │  │
 │  │ • HTML5 Canvas Radial Node Topology          │  │ • 100Hz Sub-10ms Telemetry Pipeline    │  │
 │  │ • Dynamic MITRE ATT&CK Heatmap (Zero CDNs)   │  │ • Decoupled Outbound SaaSConnector     │  │
 │  └──────────────────────────────────────────────┘  └────────────────────────────────────────┘  │
 └─────────────────────────────────────────────────────────────────────────────────────────────────┘
====================================================================================================
```

---

## Core Invariants

1. **Sub-50ms Collective Defense Bus:** Translates localized edge discoveries into fleet-wide in-kernel immunity in $< 50\,\text{ms}$, preventing lateral propagation across multi-site infrastructure.
2. **Autonomous Closed-Loop Adaptation:** Pairs with `xinfer-forge` to curate ambiguous traffic ($0.40 \le f(x) \le 0.60$), retrain models on-premises, and deploy candidate weights without human intervention.
3. **RollbackGuard SLA Enforcement:** Protects edge nodes during Canary rollouts; an operational latency spike exceeding $1{,}000\,\mu\text{s}$ triggers an automated, immediate rollback.
4. **Data Sovereignty ($0.00 Cloud Egress):** Operates on-premises within air-gapped enclaves. The embedded web management console (port 9443) enforces a **Zero-CDN guarantee**.
5. **Decoupled Outbound SaaS Connector:** Maintains a 4-tier asset tree (`Tenant` $\to$ `Nexus` $\to$ `Sentinel` $\to$ `Sensor`), transmitting health states to `app.aryorithm.com` over a decoupled, non-blocking outbound HTTPS client.
```

---

### File: `sentinel-nexus/docs/getting-started/overview.md`

```markdown
# Central Fleet Command Plane & Collective Defense Architecture

Managing cybersecurity across distributed critical infrastructure—such as dozens of regional water treatment plants, hundreds of electrical substations, or thousands of maritime vessels—presents operational challenges:

* **Siloed Defenses:** When an edge appliance encounters an unknown zero-day exploit, other regional nodes remain vulnerable until centralized security teams release signatures days or weeks later.
* **Model Management Friction:** Deploying machine learning updates to thousands of edge appliances requires manual staging and risks deploying models that breach sub-microsecond line-rate SLAs.
* **Asset Fragmentation:** Industrial sites feature diverse sensors (Modbus PLCs, Siemens controllers, DICOM scanners, IP cameras) lacking unified hierarchical visibility.

`sentinel-nexus` is the **central nervous system** that unifies edge defense appliances into an autonomous collective defense grid.

---

## 1. The "Attacked Once, Immune Everywhere" Paradigm

```text
 1. ADVERSARY LAUNCHES ZERO-DAY ATTACK:
    Target: Electrical Substation Alpha (Munich East)
    Mitigation: In-kernel eBPF drop executed in 0.82 µs.
              │
              ▼ Emits Threat IoC to Nexus Hub via gRPC Port 50051 (Transit: ~14 ms)
 2. NEXUS COLLECTIVE DEFENSE BUS:
    Evaluates severity, checks originator loop suppression, and fans out rule.
              │
              ▼ Parallel Fanout over Streaming gRPC Channels (Transit: ~18 ms)
 3. FLEET-WIDE IMMUNITY ENFORCED:
    Target: 4,999 Remaining Substations across Europe
    Reaction: KernelDropInjector writes IP to blocked_ip_map in driver space.
              │
              ▼
 TOTAL FLEET SYNCHRONIZATION TIME: 32 ms (< 50 ms SLA Bound)
```

---

## 2. Core Pillars of `sentinel-nexus`

* **Collective Defense Engine:** Distributes threat signatures across the fleet in $< 50\,\text{ms}$, stopping automated worm propagation and coordinated botnet sweeps.
* **Dataset Curator for Active Learning:** Filters ambiguous flow records ($0.40 \le f(x) \le 0.60$) and packages them into training batches for `xinfer-forge`.
* **Canary OTA Staging & RollbackGuard:** Manages staged rollouts (`Shadow Mode` $\to$ `5% Canary` $\to$ `Fleet-Wide`), monitoring edge latency and executing emergency rollbacks if performance degrades.
* **Air-Gapped Web Command Center:** Serves an administrative dashboard on port **9443** (HTML5 Canvas radial topology, MITRE ATT&CK matrix) with a real-time Server-Sent Events (SSE) telemetry stream on port **9444**.
```

---

### File: `sentinel-nexus/docs/getting-started/system-requirements.md`

```markdown
# System Requirements & Prerequisites

Review the toolchain, network configuration, and hardware requirements before building and running `sentinel-nexus`.

---

## 1. Operating System Baseline

* **Operating System:** 64-bit Linux (Ubuntu 22.04 LTS, Ubuntu 24.04 LTS, or Ubuntu 26.04 Devel).
* **Linux Kernel:** Kernel version **>= 5.15** (Kernel **>= 6.8** recommended).
* **C++ Toolchain:** Clang 16.0+ or GCC 12.1+ supporting **ISO C++20**.
* **Core Libraries:** `libgrpc++-dev`, `protobuf-compiler-grpc`, `libssl-dev`, `nlohmann-json3-dev`.

---

## 2. Hardware Resource Sizing

Hardware requirements scale based on the number of managed edge appliances:

| Managed Fleet Sizing | CPU Cores | System RAM | NVMe SSD Storage | Network Interface |
| :--- | :--- | :--- | :--- | :--- |
| **Small Fleet (< 100 Nodes)** | 8 Cores (3.0 GHz) | 16 GB DDR4/DDR5 | 100 GB NVMe | 1 GbE RJ45 |
| **Regional Hub (100 - 1,000 Nodes)** | 16 Cores (AMD/Xeon)| 32 GB DDR5 ECC | 500 GB NVMe | 10 GbE SFP+ |
| **Enterprise Grid (Up to 5,000 Nodes)**| 32 Cores (64 Threads)| 64 GB DDR5 ECC | 2 TB Enterprise NVMe| Dual 10/25 GbE |

---

## 3. Network Ports & Firewall Rules

Ensure the following ports are open on the host:

| Port Number | Protocol | Direction | Subsystem Purpose |
| :--- | :--- | :--- | :--- |
| **`50051`** | TCP / HTTP/2 | Inbound | Mutual TLS (mTLS) gRPC interface for edge appliance connections. |
| **`9443`** | TCP / HTTPS | Inbound | Air-gapped Web Command Center and administrative REST API. |
| **`9444`** | TCP / HTTP | Inbound | Real-time Server-Sent Events (SSE) streaming endpoint. |
| **`443`** | TCP / HTTPS | Outbound | Optional outbound link to Aryorithm SaaS (`app.aryorithm.com`). |
```

---

### File: `sentinel-nexus/docs/getting-started/installation-and-build.md`

```markdown
# Building & Installing Sentinel-Nexus

This guide covers building the core `sentinel-nexus` daemon, the `nexus-ctl` operations CLI, and unit benchmarks from source.

---

## 1. Install System Dependencies

### Ubuntu 24.04 / 22.04 LTS

```bash
sudo apt-get update && sudo apt-get install -y \
    build-essential \
    clang-16 \
    lld-16 \
    cmake \
    ninja-build \
    pkg-config \
    libssl-dev \
    libgrpc++-dev \
    protobuf-compiler-grpc \
    libprotobuf-dev \
    nlohmann-json3-dev \
    git
```

---

## 2. Clone the Repository

```bash
git clone --recurse-submodules https://github.com/kamisaberi/sentinel-nexus.git
cd sentinel-nexus
```

---

## 3. Configure and Compile

Build using CMake and Ninja:

```bash
mkdir build && cd build

cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=clang++-16 \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DNEXUS_BUILD_TESTS=ON \
    -DNEXUS_BUILD_CLI=ON ..

ninja -j$(nproc)
```

### Generated Artifacts in `build/bin/`:
* `sentinel-nexus`: The master central fleet command daemon.
* `nexus-ctl`: Standalone C++20 operations and administration CLI.
* `nexus_tests`: Comprehensive GoogleTest integration and unit test suite.

---

## 4. Install System-Wide

Install binaries, configuration directories, and static web assets:

```bash
sudo ninja install
sudo ldconfig

# Initialize configuration and data directories
sudo mkdir -p /etc/sentinel-nexus/certs
sudo mkdir -p /var/lib/sentinel-nexus/data
sudo mkdir -p /var/lib/sentinel-nexus/models
sudo mkdir -p /var/lib/sentinel-nexus/forge_datasets
```
```

---

### File: `sentinel-nexus/docs/getting-started/ten-minute-quickstart.md`

```markdown
# 10-Minute Quickstart: Booting Nexus & Connecting Your First Node

This walkthrough guides you through generating local mTLS certificates, booting `sentinel-nexus`, and verifying communication with a test appliance.

---

## 1. Step 1: Generate Local Test mTLS Certificates

`sentinel-nexus` includes a script to generate development PKI certificates:

```bash
cd /opt/sentinel-nexus/scripts
chmod +x gen_certs.sh
sudo ./gen_certs.sh /etc/sentinel-nexus/certs
```

This generates `ca.crt`, `server.crt`, `server.key`, `client.crt`, and `client.key`.

---

## 2. Step 2: Launch `sentinel-nexus`

Start the daemon directly in foreground console mode to observe startup logs:

```bash
sentinel-nexus --config /etc/sentinel-nexus/nexus.yaml
```

### Expected Console Output
```text
================================================================================
                    ARYORITHM SENTINEL-NEXUS FLEET HUB
================================================================================
Version                : 2.4.0 (Release Build)
Node Capacity          : 5,000 Edge Appliances Supported
gRPC Fleet Service     : Listening on 0.0.0.0:50051 (mTLS 1.3 Active)
Web Command Center     : Listening on https://0.0.0.0:9443 (Zero CDNs)
Real-Time SSE Stream   : Listening on http://0.0.0.0:9444/stream
Active Model Version   : network_threat_v1.onnx (SHA256: e9a2c31e...)
================================================================================
[INFO] StateDatabase: Restored 0 registered nodes from data/nexus_state.json
[INFO] CollectiveDefenseBus: Armed and ready for sub-50ms rule fanouts.
```

---

## 3. Step 3: Connect a Blackbox-Sentinel Appliance

On your edge appliance, configure `/etc/sentinel/sentinel.yaml` to point to the Nexus IP:

```yaml
nexus_uplink:
  enabled: true
  hub_address: "10.240.0.10" # IP of your Sentinel-Nexus host
  hub_port: 50051
  tls_enabled: true
  ca_certificate: "/etc/sentinel/certs/ca.crt"
  client_certificate: "/etc/sentinel/certs/client.crt"
  client_private_key: "/etc/sentinel/certs/client.key"
```

Restart the edge appliance daemon (`sudo systemctl restart sentinel`). 

Within seconds, the Nexus console logs:
```text
[INFO] gRPC: Handshake completed for node 'edge-substation-alpha' [TPM: TIER 1 IFX]
[INFO] FleetRegistry: Node edge-substation-alpha registered. Total Active: 1
```

---

## 4. Step 4: Verify the Connection via CLI

In a new terminal on the Nexus server, query the active fleet:

```bash
nexus-ctl fleet list
```

### Output:
```text
UUID                       HOSTNAME                 STATUS    VERSION   DROPS    SLA (LATENCY)
edge-substation-alpha      substation-01.internal   ONLINE    2.4.0     1,420    0.82 µs
```
```

---

### File: `sentinel-nexus/docs/getting-started/systemd-deployment.md`

```markdown
# Hardened Systemd Service Deployment

In production environments, `sentinel-nexus` runs as an automated, hardened systemd service configured for maximum file descriptors and process resilience.

---

## 1. Systemd Service Unit (`/etc/systemd/system/sentinel-nexus.service`)

```ini
[Unit]
Description=Aryorithm Sentinel-Nexus Fleet Command Plane & Collective Defense Grid
Documentation=https://docs.aryorithm.com/nexus/
After=network-online.target local-fs.target
Wants=network-online.target

[Service]
Type=simple
User=root
Group=root

# Executable & Configuration
ExecStart=/usr/local/bin/sentinel-nexus --config /etc/sentinel-nexus/nexus.yaml
ExecReload=/bin/kill -HUP $MAINPID
Restart=always
RestartSec=3s

# High-Concurrency Network Tuning (Supporting 5,000 Appliances)
LimitNOFILE=1048576
LimitNPROC=65536
LimitMEMLOCK=infinity

# CPU Scheduling & Priority
Nice=-10
OOMScoreAdjust=-500

# Security Hardening Sandbox
ProtectHome=true
ProtectSystem=full
PrivateTmp=true

[Install]
WantedBy=multi-user.target
```

---

## 2. Activation Commands

```bash
sudo systemctl daemon-reload
sudo systemctl enable sentinel-nexus
sudo systemctl start sentinel-nexus
sudo systemctl status sentinel-nexus
```
```

---

### File: `sentinel-nexus/docs/getting-started/docker-deployment.md`

```markdown
# Containerized Deployment & Docker Compose

For containerized environments and digital-twin cyber ranges (`sentinel-matrix`), `sentinel-nexus` can be deployed via Docker.

---

## 1. Docker Compose Configuration (`docker-compose.yml`)

```yaml
version: "3.8"

services:
  nexus:
    build:
      context: .
      dockerfile: Dockerfile
    image: aryorithm/sentinel-nexus:2.4.0
    container_name: sentinel-nexus
    restart: always
    network_mode: "host" # Recommended for low-latency line-rate gRPC
    volumes:
      - /opt/sentinel-nexus/config:/etc/sentinel-nexus
      - /opt/sentinel-nexus/certs:/etc/sentinel-nexus/certs:ro
      - /opt/sentinel-nexus/data:/var/lib/sentinel-nexus/data
      - /opt/sentinel-nexus/models:/var/lib/sentinel-nexus/models
      - /opt/sentinel-nexus/forge_datasets:/var/lib/sentinel-nexus/forge_datasets
    environment:
      - LOG_LEVEL=INFO
      - RUST_LOG=info
    healthcheck:
      test: ["CMD", "curl", "-k", "-f", "https://localhost:9443/api/v1/health"]
      interval: 10s
      timeout: 3s
      retries: 3
```

---

## 2. Running the Container

```bash
# Build and run in background
docker compose up -d

# Check live logs
docker compose logs -f nexus
```
```

---

### File: `sentinel-nexus/docs/getting-started/verifying-services.md`

```markdown
# Verifying Services & Port Status

Confirm that all three network services exposed by `sentinel-nexus` are bound and accepting traffic.

---

## 1. Network Port Verification

Run `ss` or `netstat` to verify listener bindings:

```bash
sudo ss -tulpn | grep -E '50051|9443|9444'
```

### Expected Output
```text
tcp   LISTEN 0      4096   0.0.0.0:50051   0.0.0.0:*   users:(("sentinel-nexus",pid=12040,fd=14))
tcp   LISTEN 0      128    0.0.0.0:9443    0.0.0.0:*   users:(("sentinel-nexus",pid=12040,fd=18))
tcp   LISTEN 0      128    0.0.0.0:9444    0.0.0.0:*   users:(("sentinel-nexus",pid=12040,fd=22))
```

---

## 2. Verifying the REST API & Web Command Center (Port 9443)

Query the health endpoint:

```bash
curl -k -s https://localhost:9443/api/v1/health | jq .
```

### Response:
```json
{
  "status": "HEALTHY",
  "version": "2.4.0",
  "active_appliances": 1,
  "collective_defense_bus": "ARMED",
  "active_model_sha256": "e9a2c31e847b2c94b13a7b41e2d9010000000000000000000000000000000000"
}
```

---

## 3. Testing the Real-Time SSE Stream (Port 9444)

Listen to the continuous Server-Sent Events stream:

```bash
curl -N http://localhost:9444/stream
```

### Stream Output:
```text
event: fleet_tick
data: {"timestamp_ns":1791172800184000000,"online_nodes":1,"total_drops_today":1420,"fleet_sla_us":0.82}
```
```

---

### File: `sentinel-nexus/docs/getting-started/architecture-at-a-glance.md`

```markdown
# Architecture at a Glance

The diagram below maps the interaction between edge defense appliances (`blackbox-sentinel`), the central command hub (`sentinel-nexus`), continual AI retraining (`xinfer-forge`), and cloud visibility (`app.aryorithm.com`).

---

```text
 ┌──────────────────────────────────────────────────────────────────────────────────────────┐
 │ CLOUD SAAS PORTAL: app.aryorithm.com (Optional Multi-Tenant Executive Dashboard)         │
 └──────────────────────────────────────────▲───────────────────────────────────────────────┘
                                            │ Outbound HTTPS Sync (POST /api/v1/fleet/sync)
                                            │ Decoupled SaaSConnector ($0.00 Egress Local)
 ┌──────────────────────────────────────────┴───────────────────────────────────────────────┐
 │ SENTINEL-NEXUS CENTRAL COMMAND PLANE (Tier 6 Hub)                                        │
 │                                                                                          │
 │  ┌──────────────────────────────────────────────┐  ┌──────────────────────────────────┐  │
 │  │ Sub-50ms Collective Defense Bus              │  │ DatasetCurator (Active Learning) │  │
 │  │ • Fans out FleetDefenseRules to 5,000 nodes  │  │ • Curation window: [0.40 - 0.60] │  │
 │  │ • Originator loopback suppression            │  │ • Emits forge_dataset_*.csv      │  │
 │  └──────────────────────┬───────────────────────┘  └────────────────┬─────────────────┘  │
 │                         │                                           │                    │
 │                         │                                           ▼ File Watcher       │
 │                         │                      ┌──────────────────────────────────────┐  │
 │                         │                      │ Tier 4: xinfer-forge Continual AI    │  │
 │                         │                      │ • Self-Supervised Tabular MAE        │  │
 │                         │                      │ • Golden Attacks Safety Gate (100%)  │  │
 │                         │                      │ • Automated ONNX Opset 17 Export     │  │
 │                         │                      └────────────────────┬─────────────────┘  │
 │                         │                                           │                    │
 │                         ▼ Bi-Directional mTLS Channels              ▼ POST /ota/stage    │
 │  ┌──────────────────────────────────────────────────────────────────┴─────────────────┐  │
 │  │ gRPC Fleet Service (Port 50051)                                                    │  │
 │  │ • StreamFleetRules (< 50ms propagation) • SubmitHeartbeats • Canary OTA Updates    │  │
 │  └──────────────────────┬─────────────────────────────────────────────────────────────┘  │
 └─────────────────────────┼────────────────────────────────────────────────────────────────┘
                           │
        ┌──────────────────┼──────────────────┐ Parallel Distribution
        ▼                  ▼                  ▼ (Up to 5,000 Nodes)
 ┌──────────────┐   ┌──────────────┐   ┌──────────────┐
 │ Appliance 1  │   │ Appliance 2  │   │ Appliance N  │
 │ (Substation) │   │ (Water Plant)│   │ (Hospital)   │
 └──────────────┘   └──────────────┘   └──────────────┘
```
```


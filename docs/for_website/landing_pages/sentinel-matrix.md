# Project 7 of 8: `sentinel-matrix` (`sentinel-matrix`)
## Master Landing Page & Technical Architecture Document
**Target URL:** `aryorithm.com/technology/matrix`  
**Repository:** `https://github.com/kamisaberi/sentinel-matrix`  
**Artifact:** `sentinel-matrix` (Encapsulated Autonomous Cyber-Range & Digital Twin Mesh)

---

```text
========================================================================================================
                                     PAGE STRUCTURE OUTLINE
========================================================================================================
 1. Hero Section (Headline, Value Proposition, Real-Time Mesh Metric Strip)
 2. The Cyber-Range Imperative: Why Mock Scripting Fails Complex Systems Validation
 3. Hypervisor Encapsulation & The 10.240.0.0/24 VMware-Optimized Subnet
 4. OmniFlow Multi-Modal Traffic Engine: 7 Concurrent Deep-Inspection Channels
 5. Authentic Malware PCAP Replay Pipeline (Industroyer, Triton/Trisis, Stuxnet)
 6. The Live Adversary Red-Team Node (10.240.0.99 with nmap, mbpoll, curl)
 7. Dual Observability Consoles: High-Density Terminal TUI & Air-Gapped Web UI
 8. Autonomous Closed-Loop AI Retraining (The xInfer-Forge Flywheel)
 9. Chaos Engineering & Fault-Tolerance Verification (SLA Breaches & 0ms Disconnects)
 10. Makefile Command Reference & Operations Runbook
 11. Technical Frequently Asked Questions (FAQ)
 12. Conversion Call-To-Action (CTA) & Cyber-Range Access
========================================================================================================
```

---

## 1. Hero Section

### Badge
`TIER 7 CYBER-RANGE` `VMWARE-OPTIMIZED` `OMNIFLOW 7-CHANNEL ENGINE` `LIVE ADVERSARY NODE`

### Headline
# The Autonomous Cyber-Physical Range. Multi-Modal Traffic. Real Malware PCAPs. Live In-Kernel Drops.

### Subheadline
**`sentinel-matrix`** is an encapsulated, containerized digital twin and cyber-range simulation mesh designed for VMware and Linux server environments. Operating over an isolated **`10.240.0.0/24`** subnet, it coordinates Sentinel Nexus, xInfer Forge, an active Red-Team adversary node (`10.240.0.99`), and multiple heterogeneous edge appliances in an **infinite, continuous loop** of ambient NetFlow, live `nmap` sweeps, authentic SCADA malware replays (Industroyer, Triton, S7Comm), and autonomous self-improving neural hot-reloads.

### Primary CTA Group
* `[ View on GitHub ]` $\rightarrow$ `https://github.com/kamisaberi/sentinel-matrix`
* `[ Explore 7 OmniFlow Channels ]` $\rightarrow$ `#omniflow-engine`
* `[ Launch VMware Range Guide ]` $\rightarrow$ `#quickstart-runbook`

### Live KPI Strip (Metrics Display Grid)
```text
+---------------------+---------------------+---------------------+---------------------+
|      7 Channels     |     0.84 µs SLA     |       < 50 ms       |     10.240.0.0/24   |
| Multi-Modal Wire,   | Verified In-Kernel  | Collective Defense  | Isolated VMware     |
| SCADA, Video & Bots | Driver Mitigation   | Fleet-Wide Fanout   | Collision-Free Grid |
+---------------------+---------------------+---------------------+---------------------+
```

---

## 2. The Cyber-Range Imperative: Why Mock Scripting Fails Complex Systems

```text
  TRADITIONAL CYBERSECURITY MOCK SIMULATORS
  ┌──────────────────────────────────────────────────────────────────────────────────┐
  │ • Synthetic Python scripts sleep for 5 seconds, sending static JSON strings.     │
  │ • Zero real network packets traverse the physical or virtual network stack.      │
  │ • Operating systems don't experience raw socket contention or memory pressure.   │
  │ • Cannot test driver-level eBPF/XDP hooks, NIC ring buffers, or raw packet drops.│
  │ [ RESULT: Systems look great on paper, but collapse under real wire-speed load ]  │
  └──────────────────────────────────────────────────────────────────────────────────┘
                                          VS.
  THE SENTINEL-MATRIX AUTONOMOUS DIGITAL TWIN
  ┌──────────────────────────────────────────────────────────────────────────────────┐
  │ • Full multi-container network running genuine C++20 daemons and BPF bytecode.   │
  │ • Live adversary node fires real TCP/UDP handshakes, nmap sweeps, and Modbus CLI.│
  │ • Real-world malware PCAPs (Industroyer, Triton, S7Comm) replayed byte-for-byte. │
  │ • Real-time closed-loop AI adaptation: Edge -> Nexus -> Forge -> Canary -> Edge. │
  │ [ REAL WIRE PACKETS • REAL KERNEL DROPS • REAL-TIME XAI FEATURE ATTRIBUTION ]     │
  └──────────────────────────────────────────────────────────────────────────────────┘
```

Validating a sub-microsecond cyber-physical defense platform requires real network physics. Synthetic API mockers cannot test:
* Whether the Linux kernel's **XDP driver hooks** can inspect and discard packets before socket memory allocation (`sk_buff`).
* Whether an adversary executing a real **TCP SYN scan (`nmap -sS`)** genuinely experiences a connection freeze when an in-kernel rule drops their traffic.
* Whether real industrial PLCs experience communication jitter when deep packet inspection runs on Modbus TCP and PROFINET streams.

`sentinel-matrix` was engineered to deliver a **complete, living cyber range**. It runs the identical compiled binaries, shared libraries, and eBPF bytecode that deploy to physical 1U hardware appliances—encapsulated within a private, reproducible virtual network.

---

## 3. Hypervisor Encapsulation & The 10.240.0.0/24 Subnet

Running complex eBPF programs, raw network taps, and containerized routers inside a **VMware Linux guest** (such as Ubuntu on VMware Workstation, ESXi, or Fusion) introduces unique networking challenges that `sentinel-matrix` solves at the architectural level:

```text
========================================================================================================
                          SENTINEL-MATRIX CONTAINER NETWORK TOPOLOGY
========================================================================================================

                               ┌────────────────────────────────┐
                               │   VMWARE GUEST HOST (Ubuntu)   │
                               │   Interface: ens33 / eth0      │
                               └───────────────┬────────────────┘
                                               │
                               ┌───────────────┴────────────────┐
                               │ DOCKER BRIDGE: sentinel-grid   │
                               │ Subnet: 10.240.0.0/24 (Zero Cl)|
                               └───────────────┬────────────────┘
                                               │
         ┌──────────────────┬──────────────────┼──────────────────┬──────────────────┐
         │                  │                  │                  │                  │
         ▼                  ▼                  ▼                  ▼                  ▼
 ┌───────────────┐  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐  ┌───────────────┐
 │ matrix-nexus  │  │ matrix-forge  │  │ traffic-gen   │  │ adversary     │  │ edge-01 (OT)  │
 │ 10.240.0.10   │  │ 10.240.0.20   │  │ 10.240.0.50   │  │ 10.240.0.99   │  │ 10.240.0.101  │
 │ Ports: 50051  │  │ PyTorch MAE   │  │ OmniFlow      │  │ nmap, curl    │  │ OpenVINO      │
 │        9443   │  │ Safety Gate   │  │ 7 Channels    │  │ mbpoll        │  │ Modbus/DNP3   │
 │        9444   │  │ ONNX Stager   │  │ Live Streamer │  │ Live Wire     │  │ XDP SKB Drop  │
 └───────────────┘  └───────────────┘  └───────────────┘  └───────────────┘  └───────────────┘
                                                                                     │
                                    ┌──────────────────┬─────────────────────────────┘
                                    │                  │
                                    ▼                  ▼
                            ┌───────────────┐  ┌───────────────┐
                            │ edge-02 (PACS)│  │ edge-03 (REF) │
                            │ 10.240.0.102  │  │ 10.240.0.103  │
                            │ TensorRT      │  │ RKNN NPU      │
                            │ DICOM / HL7   │  │ PROFINET / S7 │
                            │ XDP SKB Drop  │  │ XDP SKB Drop  │
                            └───────────────┘  └───────────────┘
```

### Hypervisor Compatibility Adaptations
1. **Collision-Free Subnet (`10.240.0.0/24`):** Standard Docker installations pick subnets from `172.17.0.0/16` through `172.31.0.0/16`. In VMware environments, VMware's virtual adapters (`VMnet1` / `VMnet8`) frequently use these identical ranges, triggering `Pool overlaps with other one on this address space`. `sentinel-matrix` uses an isolated `10.240.0.0/24` block, guaranteeing zero route collisions.
2. **Generic SKB Mode (`XDP_FLAGS_SKB_MODE`):** Virtual vNICs (`vmxnet3`, `veth`) do not support native hardware-driver XDP hooks. The edge containers automatically engage Generic SKB mode with `NET_ADMIN` and `BPF` capabilities, achieving line-rate drops in $< 1\,\mu\text{s}$ within the virtual machine.
3. **Silicon Library Dynamic Linking:** Host-compiled C++ binaries requiring newer dependencies (`libabsl_*`, `libre2.so.11`, `libgrpc++`, `libprotobuf`) are automatically harvested from the host via `make init` and mounted into `/usr/local/lib/matrix-deps` with `LD_LIBRARY_PATH` configuration, eliminating glibc/ABI incompatibilities.

---

## 4. OmniFlow Multi-Modal Traffic Engine: 7 Concurrent Channels

The traffic generator (`matrix-traffic-gen`) runs **`OmniFlow`**—a multi-threaded C++/Python engine running **7 concurrent worker threads**, each generating telemetry and attack vectors matching Sentinel's 26 modules:

```text
========================================================================================================
                          THE 7 OMNIFLOW MULTI-MODAL TRAFFIC CHANNELS
========================================================================================================
 Channel Identifier      Traffic Modality               Target Sentinel Module    Payload & Protocol Dynamics
 ──────────────────────  ─────────────────────────────  ────────────────────────  ──────────────────────────────────────────────
 Channel 1: SCADA / OT   Industrial Control Systems     18_cps_sec (SCADA Guard)  Modbus TCP (FC03/FC05) & DNP3 read/writes.
                                                        Plugins 01--06            Injects unauthorized coil overrides (T0855).

 Channel 2: Edge Vision  Optical & Thermal Sensors      libxinfer Vision Plugins  Synthesizes 30 FPS video tensor buffers.
                                                        YOLOv8 / UltraFace        Injects perimeter intrusions & thermal spikes.

 Channel 3: Web & API    L7 Application & Bot Traffic   05_waf, 10_bad            REST API queries, SQLi, and BOLA/IDOR.
                                                        (Bot Abuse Defense)       Simulates non-human linear mouse kinematics.

 Channel 4: Identity     Identity & Access Governance   12_itdr, 14_ato           Simulates Kerberos SPN ticket abuse (T1208).
                                                        (Account Takeover)        Injects impossible travel velocity anomalies.

 Channel 5: Host/Syscall Endpoint & Container Security  07_epp_ngav, 09_cwpp      Traces eBPF container breakouts at sys_enter.
                                                        (Container Workload Guard)Injects high-entropy buffers (7.95 bits).

 Channel 6: Med / IoT    Specialized Protocol Streams   17_iot_sec, Plugins 07-12 Streams DICOM PACS radiology images & HL7.
                                                        (Medical & Maritime IoT)  Injects MAVLink UAV waypoint spoofing packets.

 Channel 7: NetFlow Blst Line-Rate Active Learning Feed 01_siem_core, 04_ids_ips  Blasts 32-dim directional NetFlow vectors.
                                                        xinfer-forge (Tier 4)     Injects uncertainty window [0.40 - 0.60].
========================================================================================================
```

---

## 5. Authentic Malware PCAP Replay Pipeline

`sentinel-matrix` incorporates a dual-mode PCAP replay infrastructure:
1. **The Offline Generator (`tools/generate_real_pcaps.py`):** Self-contained binary generator creating byte-authentic `.pcap` files offline for air-gapped enclaves.
2. **The Live Downloader (`tools/download_real_pcaps.py`):** Automatically queries GitHub's LFS Batch API, resolves Git LFS pointer files, and downloads genuine historical PCAP captures from **Nozomi Networks**, **CISA**, and university testbeds.

```text
========================================================================================================
                         AUTHENTIC HISTORICAL MALWARE PCAP REPLAY ARSENAL
========================================================================================================
 Threat Artifact         Target Protocol          Target Appliance         MITRE Tactic & Attack Objective
 ──────────────────────  ───────────────────────  ───────────────────────  ─────────────────────────────────────────────
 Industroyer /           IEC 60870-5-104          Edge-Substation-01       T0855: Unauthorized Command Message
 CrashOverride           Telecontrol (Port 2404)  (10.240.0.101)           APDU Type 45 single command execution 
                                                                           attempting to trip power grid circuit breakers.

 Triton / Trisis         Schneider Electric       Edge-Hospital-PACS-02    T0843: Program Download
                         TriStation 1131 (19999)  (10.240.0.102)           TriStation protocol memory overwrite payload 
                                                                           attempting to disable safety shutdown loops.

 Stuxnet                 Siemens S7Comm           Edge-Refinery-PLC-03     T0831: Manipulation of Control
                         ISO-on-TCP (Port 102)    (10.240.0.103)           TPKT/COTP job request with Function 0x28 
                                                                           attempting to tamper with centrifuge speeds.
========================================================================================================
```

### High-Speed Binary Streaming Engine (`src/traffic/pcap_streamer.py`)
`pcap_streamer.py` parses PCAP global and packet headers directly in binary:
* Reconstructs raw Ethernet, IPv4, and TCP/UDP frames.
* Controls injection rate with microsecond precision (`--rate 50` packets/second).
* Emits live threat telemetry to Sentinel Nexus in parallel, demonstrating that **the edge appliance drops the malicious packets in $< 0.84\,\mu\text{s}$ while Nexus fans out collective defense rules across all sister nodes in $< 50\,\text{ms}$**.

---

## 6. The Live Adversary Red-Team Node (`matrix-adversary`)

In addition to PCAP streaming, `sentinel-matrix` includes an **active, containerized Red-Team adversary node** at IP **`10.240.0.99`**.

Running `live_adversary_daemon.py`, this container uses real Linux network auditing tools to execute live, dynamic network attacks against the edge appliances over virtual wires:

```text
========================================================================================================
                       LIVE WIRE ADVERSARY EXECUTION (CONTAINER 10.240.0.99)
========================================================================================================

 [ MODE 1: AMBIENT WIRE TRAFFIC (CONTINUOUS) ]
  • curl sends real HTTP GET requests to edge control interfaces on port 8443.
  • mbpoll polls Modbus holding registers (1-4) on port 502 every 2 seconds.

 [ MODE 2: RECONNAISSANCE & PORT DISCOVERY (T1046) ]
  • Fires live nmap TCP SYN sweeps: nmap -sS -Pn -p 80,443,502,102,2404 10.240.0.101
  • Result: Evaluates how eBPF transitions ports from "open" to "filtered" in real time.

 [ MODE 3: SCADA COIL OVERRIDE MANIPULATION (T0855) ]
  • Dispatches real Modbus FC05 write commands via mbpoll:
    mbpoll -m tcp -a 1 -r 105 -t 0 10.240.0.101 1
  • Result: Sentinel's Module 18 intercepts the forced write, drops the packet, and alerts Nexus.

 [ MODE 4: HIGH-VELOCITY API ABUSE & FUZZING (T1190) ]
  • Rapid curl burst attempting credential stuffing and endpoint traversal on port 8443.
  • Result: Module 05 (WAF) and Module 10 (Bot Defense) block the source IP in the kernel.
========================================================================================================
```

---

## 7. Dual Observability Consoles: Live TUI & Web Command Center

### Console A: High-Density Terminal Dashboard (TUI)
Run `make tui` to launch the live, split-screen terminal interface:

```text
┌─────────────────────────────────────── CONNECTED CYBER-PHYSICAL APPLIANCES ───────────────────────────────────────┐
│ Node ID      Site / Infrastructure    Status    CPU %    eBPF Drops    Mitigation SLA                           │
│ NODE-8fa901  PowerGrid-North-01       ONLINE    14.2%            48          0.84 µs                            │
│ NODE-c34b12  Metro-General-Hospital   ONLINE    18.7%            35          0.79 µs                            │
│ NODE-77e190  Coastal-Refinery-ZoneB   ONLINE    11.5%            29          0.88 µs                            │
└─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────── ACTIVE MITRE DETECTIONS ─────────────────────────────────────────┐
│ Tactic ID   Modality / Technique Name                               Hits                                        │
│ T0855       Unauthorized Command (Industroyer / Modbus Override)     14                                         │
│ T0843       Program Download (Triton TriStation Memory Overwrite)     3                                         │
│ T0831       Manipulation of Control (Stuxnet S7Comm PLC Tamper)       6                                         │
│ T1071       Application Layer Protocol (C2 Egress Beacon)             8                                         │
│ T1046       Network Service Discovery (Live nmap SYN Sweep)          22                                         │
└─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
┌────────────────────────────────── REAL-TIME XAI FEATURE ATTRIBUTION & AUDIT PROOF ──────────────────────────────┐
│ Target IP: [198.51.100.45]  | Tactic: T0855 (Unauthorized Command) | Mitigation: 0.84 µs (XDP_DROP)             │
│   #1 [54.2%] SCADA_Function_Code    : Observed: 0x05 (Force Coil)   | Expected: 0x03 (Read Only)                │
│              Audit Note: Unauthorized coil override attempting physical valve manipulation                      │
│   #2 [28.1%] Forward_Packet_Rate    : Observed: 184.2 Hz            | Expected: 18.4 Hz                         │
│              Audit Note: Command injection velocity exceeded safety threshold by >10x                           │
│   #3 [14.8%] SCADA_Register_Address : Observed: 105 (Cooling Valve) | Expected: 0-100 (Sensor Zone)             │
│              Audit Note: Target register belongs to restricted physical actuation zone                          │
└─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
```

### Console B: Air-Gapped Web Command Center (`http://localhost:9443`)
* **Radial Topology Canvas:** Interactive HTML5 Canvas displaying real-time connection lines between Nexus (`10.240.0.10`) and edge nodes.
* **XAI Explainability Cards:** Color-coded cards showing the top-3 physical deviations, observed values, and baseline expectations.
* **1-Click Quarantine:** Instantly unblock or purge IPs across all appliances with a single click.

---

## 8. Autonomous Closed-Loop AI Retraining (The Flywheel)

The entire machine learning adaptation lifecycle executes without manual intervention:

```text
========================================================================================================
                      THE CONTINUOUS ON-DEVICE ADAPTATION FLYWHEEL
========================================================================================================

 [ STEP 1: EDGE VECTOR EXTRACTION ]
  • OmniFlow streams high-rate NetFlow traffic into edge appliances.
  • libxinfer identifies flows with uncertainty [0.40 <= p <= 0.60] or high autoencoder reconstruction loss.
  • Edge appliances stream candidate vectors to Nexus via gRPC.

 [ STEP 2: DATASET CURATION ]
  • Nexus buffers vectors and flushes them into /shared/datasets/forge_dataset_*.csv.

 [ STEP 3: FORGE SELF-SUPERVISED ADAPTATION ]
  • matrix-forge detects the new dataset.
  • Trains a Masked Autoencoder (MAE) on the new representations for 50 epochs.
  • Evaluates weights against configs/safety/golden_attacks.yaml (100% attack retention verified).
  • Compiles model to ONNX Opset 17: models/network_threat_v2.onnx.

 [ STEP 4: NEXUS CANARY PROGRESSION ]
  • Forge stages candidate model to Nexus via REST API.
  • Nexus transitions model: SHADOW_MODE -> CANARY_5_PCT -> FLEET_WIDE.

 [ STEP 5: EDGE HOT-RELOAD ]
  • Edge appliances auto-download the new .onnx binary over HTTP.
  • Verifies the SHA-256 cryptographic checksum.
  • Executes live zero-downtime hot-reloading (< 1ms reload) without dropping network frames.
========================================================================================================
```

---

## 9. Chaos Engineering & Fault-Tolerance Verification

`sentinel-matrix` includes an automated fault-injection suite to prove system resilience:

### 1. SLA Latency Breach & Automated Rollback
```bash
make chaos-latency
```
* **Mechanism:** Simulates inference degradation on an edge appliance ($1{,}650\,\mu\text{s} > 1{,}000\,\mu\text{s}$ limit).
* **Verification:** Nexus's `RollbackGuard` detects the SLA violation, flags an emergency event, purges the candidate model, and reverts the fleet to the stable model within milliseconds.

### 2. Instant 0 ms Graceful Disconnect
```bash
make chaos-sever
```
* **Mechanism:** Kills Edge Node 01. Node 01 emits a `DeregistrationRequest` frame on `SIGINT`.
* **Verification:** In the Web UI and TUI, Node 01 transitions from **`ONLINE` (Green)** $\rightarrow$ **`OFFLINE` (Red)** in **0 milliseconds**, bypassing the standard 15-second heartbeat timeout.

---

## 10. Makefile Command Reference & Operations Runbook

```bash
# ==============================================================================
# 1. GRID LIFECYCLE & CORE OPS
# ==============================================================================
make init               # Prepare shared folders, certs, and auto-bundle host dynamic libs
make build              # Build all container images (Nexus, Sentinel, Forge, Adversary)
sudo make up            # Launch the entire 8-node simulation mesh in background
sudo make down          # Gracefully stop and dismantle the grid
sudo make restart       # Full clean restart of the mesh
make status             # Check container health and IP bindings
make logs               # Stream real-time logs from all containers
make clean              # Purge temporary datasets, models, and caches

# ==============================================================================
# 2. OBSERVABILITY & MONITORING
# ==============================================================================
make tui                # Launch live split-panel Terminal UI (Nodes + MITRE + XAI)
make adversary-logs     # Stream live terminal output from the Red-Team adversary node

# ==============================================================================
# 3. LIVE ADVERSARY ATTACKS (Container 10.240.0.99)
# ==============================================================================
make live-nmap          # Execute live nmap TCP SYN discovery scan across nodes (T1046)
make live-scada         # Execute live Modbus FC05 coil override via mbpoll (T0855)
make live-api           # Execute live high-velocity HTTP API abuse with curl (T1190)

# ==============================================================================
# 4. REAL-WORLD PCAP REPLAY STREAMS (Historical Malware)
# ==============================================================================
make download-pcaps     # Download genuine PCAPs from Nozomi, CISA, and university labs
make attack-real-triton # Replay Nozomi Networks genuine TRITON / Trisis attack capture
make attack-real-modbus # Replay University of Illinois genuine Modbus TCP SCADA capture
make attack-real-s7     # Replay genuine Siemens S7Comm PLC memory read/write capture
make attack-real-dnp3   # Replay University of Illinois genuine DNP3 substation capture
make attack-real-iec104 # Replay genuine IEC 60870-5-104 power grid telecontrol capture

# ==============================================================================
# 5. OFFLINE PCAP GENERATION & REPLAY (Zero-Network)
# ==============================================================================
make generate-pcaps     # Locally generate binary PCAP captures without internet
make attack-industroyer # Replay Industroyer IEC-104 circuit breaker trip
make attack-triton      # Replay synthetic Triton TriStation safety override
make attack-stuxnet     # Replay synthetic Stuxnet S7Comm frequency tamper

# ==============================================================================
# 6. CHAOS ENGINEERING
# ==============================================================================
make chaos-latency      # Inject SLA latency violation (>1000us) to verify rollback
make chaos-sever        # Abruptly sever Node 01 to test 0ms instant disconnect
```

---

## 11. Technical Frequently Asked Questions (FAQ)

#### Q: How does `sentinel-matrix` prevent Docker network collisions in VMware?
**A:** Standard Docker installations allocate subnets dynamically from `172.17.0.0/16` through `172.31.0.0/16`. VMware Workstation and ESXi frequently assign these identical ranges to host virtual adapters (`VMnet1`/`VMnet8`), triggering `Pool overlaps with other one on this address space`. `sentinel-matrix` explicitly specifies a private **`10.240.0.0/24`** block, eliminating routing conflicts.

#### Q: Why did the containers require `ubuntu:devel` instead of `ubuntu:24.04`?
**A:** When binaries (`sentinel-nexus`, `sentinel`) are compiled on an **Ubuntu 26.04** host, they link against **`glibc 2.43`**. Running those binaries inside stock `ubuntu:24.04` containers fails with `/lib/x86_64-linux-gnu/libm.so.6: version GLIBC_2.43 not found` because Ubuntu 24.04 only includes glibc 2.39. Using `FROM ubuntu:devel` ensures the container's glibc environment matches the host.

#### Q: How does the adversary container execute raw network scans without crashing Docker?
**A:** The adversary container runs with `cap_add: [NET_ADMIN]` on the dedicated `sentinel-grid-net` bridge. It sends raw IP packets directly across the virtual bridge to the edge containers' IP addresses (`10.240.0.101` to `10.240.0.103`), fully isolated from your host's physical network adapters.

#### Q: Can I inspect the raw PCAP files in Wireshark?
**A:** Yes. All `.pcap` files stored in `configs/pcaps/` and `configs/pcaps/downloaded/` are standard, binary Wireshark-compliant capture files (starting with magic `0xa1b2c3d4`). You can open them directly in Wireshark to inspect the exact Modbus, IEC-104, S7Comm, and TriStation packet headers.

---

## 12. Conversion Call-To-Action (CTA)

```text
+------------------------------------------------------------------------------------------------------+
|                     LAUNCH THE FULL CYBER-RANGE DIGITAL TWIN IN MINUTES                              |
|                                                                                                      |
|   Experience sub-microsecond in-kernel active defense, live adversary tool execution,                |
|   and closed-loop self-supervised neural adaptation in an encapsulated virtual environment.          |
|                                                                                                      |
|   [ Clone sentinel-matrix on GitHub ]   [ Read Operations Manual ]            [ Contact Systems Team ]|
|   github.com/kamisaberi/sentinel-matrix aryorithm.com/technology/matrix       research@aryorithm.com  |
+------------------------------------------------------------------------------------------------------+
```

---

### End of Project 7 Document
*Ready to proceed to **Project 8: `sentinel-stack` (Unified 6-Tier Meta-Installer & Master Deployment Orchestrator)** upon your confirmation.*
# Brand, Architecture & UI/UX Master Brief: Aryorithm.com

---

## 1. Brand Identity & Product Hierarchy

### The Company: Aryorithm (`aryorithm.com`)
* **Core Positioning:** Deep-Tech Cyber-Physical Security & Autonomous Edge AI Silicon Systems.
* **Tagline:** *"Deterministic Sub-Millisecond Active Defense for Sovereign Infrastructure."*
* **The Mission:** Transition enterprise, defense, and industrial cybersecurity from slow, cloud-dependent log collection (15–60s alert delays) to autonomous, air-gapped, sub-millisecond edge mitigation ($< 0.84\,\mu\text{s}$ eBPF kernel drops).

---

### The Product Portfolio Structure

```text
================================================================================================
                                ARYORITHM ECOSYSTEM PORTFOLIO
================================================================================================

 [ FLAGSHIP COMMERCIAL PRODUCTS ]
 ├── 1. SENTINEL NEXUS (Tier 6)
 │      └── Category: Central Autonomous Fleet Orchestrator & Collective Defense Grid
 │      └── Target: CISOs, SOC Teams, Enterprise Enclaves, Multi-Site Critical Infrastructure
 │
 ├── 2. BLACKBOX SENTINEL (Tier 3)
 │      └── Category: Cyber-Physical Edge XDR & SIEM Appliance (Hardware & Virtual)
 │      └── Target: Plant Managers, Substation Engineers, Datacenter Ops, Defense Vessels
 │
 ├── 3. XINFER FORGE (Tier 4)
 │      └── Category: Continuous Autonomous On-Device Retraining Daemon (MAE / InfoNCE)
 │      └── Target: ML Ops, Autonomous Systems, Closed Air-Gapped Networks

 [ OPEN-CORE & DEVELOPER ENGINES (C++20 LIBRARIES) ]
 ├── 4. xINFER ENGINE (`libxinfer.so` - Tier 1)
 │      └── Category: Universal Heterogeneous AI Runtime (15 Silicon Backends)
 │      └── Target: Embedded Developers, Silicon Vendors, Edge AI Engineers
 │
 ├── 5. BLACKBOX ENGINE (`libblackbox.so` - Tier 2)
 │      └── Category: Sub-Millisecond eBPF/XDP Mitigation & TPM 2.0 Security Core
 │      └── Target: Kernel Engineers, Systems Programmers, Low-Latency Defense

 [ ACADEMIC RESEARCH & BENCHMARKS ]
 └── 6. SENTINEL-LAB (Tier 5)
        └── Category: Open Research Testbed, SLAB Protocol & Thesis Platform
        └── Target: Researchers, PhD/Master Candidates, Reproducible Benchmark Community
================================================================================================
```

---

## 2. Design System & Visual Language (UI Foundations)

### Design Philosophy: *Industrial Sovereign Cyber-Physical*
The aesthetic merges high-performance Linux kernel systems (dark terminal aesthetics) with mission-critical defense operations. It avoids generic SaaS startup styles (no pastel cartoons, no floating bloated 3D shapes).

* **Color Palette:**
  * **Obsidian Foundation:** `#07090E` (Deep spatial background)
  * **Panel Slate:** `#0D111A` (Card & container surfaces)
  * **Border Grid:** `#1A2232` (Structural line work)
  * **Active Defense Cyan:** `#00E5FF` (Primary interactive highlights, AI inference paths)
  * **Kernel Mitigation Green:** `#00FFA3` (Zero packet loss, healthy nodes, TPM verified)
  * **Threat Alert Crimson:** `#FF3366` (eBPF packet drops, malicious signatures, attack waves)
  * **Telemetry Gold:** `#FFB800` (Continuous retraining, active learning uncertainty window)
  * **Text Primary:** `#F0F4F8` / **Text Secondary:** `#8A99AD`

* **Typography:**
  * **Headings:** Modern, technical geometric sans-serif (*Space Grotesk* or *Söhne Breit*).
  * **Body:** Clean, legible neo-grotesque (*Inter* or *Geist*).
  * **Data / Code / Latency Metrics:** Monospace engineered for high read-speed (*JetBrains Mono* or *Fira Code*).

* **Visual Assets & Accents:**
  * Technical grid backdrops with subtle dynamic radial glows.
  * Live-rendered interactive canvas nodes (replicating the Nexus radial topology).
  * Real-time code execution windows and live benchmark toggle sliders.

---

## 3. Global Information Architecture & Navigation

```text
[ ARYORITHM ]    Platform ▼    Technology ▼    Research    Benchmarks    Company    [ Schedule Briefing ]
```

1. **Platform (Enterprise Solutions):**
   * **Sentinel Nexus:** Central Collective Defense Grid & Fleet Command.
   * **Blackbox Sentinel:** Cyber-Physical Edge XDR/SIEM Appliance (26 Modules, 30 Plugins).
   * **xInfer Forge:** Air-gapped continuous learning & safety-gated retraining.
2. **Technology (Core Engines & Open-Core):**
   * **xInfer Engine:** Universal zero-copy C++20 AI runtime across 15 silicon platforms.
   * **Blackbox Core:** eBPF/XDP nanosecond packet dropper & lock-free SPMC engine.
3. **Research:**
   * **Sentinel-Lab:** Academic preprint, SLAB binary wire protocol, reproducible CIC-IDS-2017 evaluation.
4. **Benchmarks:**
   * Interactive benchmark comparison against Splunk, Elastic, QRadar, and Suricata.
5. **Company:**
   * Mission, Hardware Attestation (TPM 2.0 / Sovereignty), Contact / Defense Procurement.

---

## 4. Page-by-Page Layout & Content Blueprint

---

### Page 1: Homepage (`/`) — The Flagship Hook

#### Section 1: The Hero Section (Above the Fold)
* **Headline:**  
  *“From 60-Second Cloud Detection to 0.84-Microsecond Kernel Mitigation.”*
* **Subheadline:**  
  *“Aryorithm delivers the world’s first autonomous cyber-physical active defense ecosystem. Powered by `libxinfer` (15 silicon backends) and native Linux eBPF/XDP, Sentinel Nexus turns distributed edge appliances into an air-gapped, collective immunity grid.”*
* **Live KPI Badge Row:**
  * `0.84 µs` — Wire-Speed Mitigation Latency
  * `1,250,000 EPS` — Sustained Single-Node Throughput
  * `15 Targets` — Silicon AI Backends (OpenVINO, TensorRT, RKNN, etc.)
  * `$0 Cloud Egress` — 100% Air-Gapped Operation
* **Hero Visual:**
  * Interactive 3D/Canvas rendering of **Sentinel Nexus** at the center with glowing pulse-lines connecting 5 edge nodes (Intel NUC, NVIDIA Jetson, Rockchip OT gateway, SCADA PLC, and hospital PACS server).
* **CTAs:**
  * `[ Explore Sentinel Nexus Platform ]` (Primary Cyan Button)
  * `[ View Academic Paper & Benchmarks ]` (Secondary Ghost Button)

---

#### Section 2: The Core Problem vs. The Aryorithm Paradigm
A visual comparison table or side-by-side interactive split:

| Traditional Cloud SIEM / XDR | Aryorithm Autonomous Active Defense |
| :--- | :--- |
| **Passive & Retrospective:** Queries logs 15 to 60 seconds *after* an attack completes. | **Inline & Predictive:** Intercepts traffic at driver rings in $< 1.0\,\mu\text{s}$ before host execution. |
| **Massive Bandwidth & Cloud Egress:** Streams terabytes of raw logs to AWS/Azure. | **Edge Latent Processing:** Keeps 100% of data local; streams only high-uncertainty embeddings. |
| **Fragile Software Identity:** Easily spoofed VM MAC addresses and software tokens. | **Cryptographic Silicon Trust:** Bound to physical TPM 2.0 PCR quotes and DMI UUID hardware. |
| **Fragmented Stacks:** Relies on third-party python agents, bloated runtimes, and external CDNs. | **Native Native C++20:** Zero Python or Java in the mitigation path. Air-gapped SPA with zero CDNs. |

---

#### Section 3: The Three-Pillar Platform Architecture
Three interactive card modules with terminal previews:

1. **The Grid Command: Sentinel Nexus**
   * *“Collective Immunity across 10,000 Appliances.”*
   * Attacked once at an edge substation; immune everywhere in $<50\,\text{ms}$ via bidirectional gRPC kernel sync.
   * Automated Over-The-Air canary rollouts with SLA latency guards ($>1{,}000\,\mu\text{s}$ auto-rollback).

2. **The Edge Guardian: Blackbox Sentinel**
   * *“Cyber-Physical XDR & SIEM in a Single Appliance.”*
   * 26 decoupled native modules (`siem_core`, `ueba`, `cwpp`, `bad`, `iot_sec`, `cps_sec`).
   * 30 industrial plugins for SCADA/OT (Modbus, DNP3, PROFINET, S7Comm, MAVLink, DICOM).

3. **The Intelligence Core: xInfer & Forge**
   * *“Continuous Silicon-Agnostic Learning without Cloud Access.”*
   * Universal runtime executing `.onnx` models across 15 architectures.
   * Self-supervised MAE/InfoNCE continuous adaptation protected by non-negotiable golden attack safety gates.

---

#### Section 4: Live Interactive Benchmark Module
An interactive slider where visitors adjust network traffic from `10,000 EPS` to `1,000,000 EPS`:
* Shows live comparison bars for **Mitigation Latency**, **CPU Utilization**, and **RAM Footprint** between **Aryorithm Sentinel**, **Splunk**, **Elastic**, **QRadar**, and **Suricata**.
* Displays verified industrial bare-metal hardware specs (Intel Core i9-14900K, 192GB DDR5, Intel X520 10GbE SFP+).

---

#### Section 5: Built for High-Assurance Enclaves (Trust & Compliance)
Badges with architectural proofs:
* **CMMC 2.0 (Level 2) & NIST SP 800-171:** Sub-millisecond incident response and TPM 2.0 attestation proofs.
* **IEC 62443-3-3 / 4-2:** Industrial automated protocol constraint enforcement for OT.
* **100% Air-Gapped Guarantee:** Operates with zero internet connectivity and zero external CDN script tags.

---

### Page 2: The Enterprise Platform (`/platform/nexus`)

Dedicated to the flagship product: **Sentinel Nexus**.

* **Interactive Hero:** Live interactive preview of the Web Command Center UI (Fleet Topology, MITRE ATT&CK Matrix, eBPF Drop Table, and Canary Staging).
* **Key Feature Grid:**
  1. **Collective Defense Engine (`IocBroadcaster`):** Technical breakdown of how a zero-day payload on Node 1 fans out to the `blocked_ip_map` of all other appliances.
  2. **Active Learning Bridge (`ForgeBridge` & `DatasetCurator`):** How high-uncertainty NetFlow vectors ($[0.40, 0.60]$) are sampled without choking WAN links.
  3. **Automated Canary Rollout Engine:** Visualizing the three-phase staged deployment (`Shadow Mode` $\rightarrow$ `5% Canary` $\rightarrow$ `Fleet-Wide`) with the `RollbackGuard` SLA monitor.
  4. **TPM 2.0 Remote Attestation:** Cryptographic verification of node identity before admission into the defense grid.
  5. **Air-Gapped Sneakernet Suite:** How air-gapped nuclear/defense facilities export `.snbundle` encrypted packages for offline retraining.
* **Management Interface:** Showcase of `nexus-ctl` (the terminal operations CLI) with interactive dark-mode copyable command snippets.

---

### Page 3: The Edge Appliance (`/products/sentinel`)

Dedicated to **Blackbox Sentinel**.

* **Appliance Form Factors:**
  * **Industrial Rugged Hardware Appliance (1U / DIN-Rail):** For substations, oil rigs, naval vessels, and remote plants.
  * **Enterprise Virtual Appliance (vSphere / KVM / Proxmox):** For enterprise DMZs, private clouds, and datacenter enclaves.
* **The 26 Subsystem Deep-Dive:** An interactive accordion or filterable matrix displaying each module:
  * IT/Cloud: `01_siem_core`, `02_ueba`, `05_waf`, `09_cwpp`, `14_ato`.
  * Physical & OT: `17_iot_sec` (DICOM PACS), `18_cps_sec` (SCADA Modbus/DNP3), `21_side_channel` (Power/EM analysis).
* **The 30 Industrial Protocol Plugins:** Showcase of custom C++ dissectors for critical infrastructure: Modbus, DNP3, PROFINET, S7Comm, MAVLink, and AIS Maritime.

---

### Page 4: Developer & Open-Core Silicon Engines (`/technology`)

Targeted at kernel engineers, AI researchers, and embedded developers.

#### Tab A: `xInfer Engine` (`libxinfer.so`)
* **Core Value:** *"Write once, infer natively across 15 silicon targets."*
* **15-Platform Matrix Table:** Detailed interactive grid showing backend loader, supported formats (`.engine`, `.xml`, `.rknn`, `.hef`), and zero-copy memory paths (`DMA-BUF`, `NVMM`, host-pinned).
* **C++20 API Walkthrough:** Clean code snippet showcasing how easy it is to initialize `xinfer::InferenceEngine`, load dynamic `.so` plugins via `dlopen`, and run model execution without Python overhead.

#### Tab B: `Blackbox Core` (`libblackbox.so`)
* **Core Value:** *"Kernel-space defense at hardware wire-speed."*
* **eBPF/XDP Deep Dive:** Architectural breakdown of `xdp_filter.c` running at driver hooks, executing nanosecond packet drops via BPF hash maps before network packets reach the Linux TCP/IP stack.
* **SPMC Lock-Free Ring Buffer:** How `EventRingBuffer` transfers millions of events to userspace inference without mutex contention or thread blocking.

---

### Page 5: Research & Academic Testbed (`/research/sentinel-lab`)

Positions Aryorithm as an authoritative deep-tech scientific pioneer.

* **Academic Preprint Paper:**
  * Direct PDF download of `paper.tex` preprint: *"Autonomous Cyber-Physical Threat Mitigation: A Sub-Millisecond Active Defense Architecture on Heterogeneous Silicon"*.
* **SLAB Binary Wire Protocol Specification:**
  * Interactive byte-packet dissector: `[Magic (SLAB) | EventID | GroundTruth | NumFeatures | Float32 Array]`.
* **Reproducible Evaluation Pipeline:**
  * Open benchmark suite running the CIC-IDS-2017 PortScan dataset at 60k+ EPS on dual backends (Intel OpenVINO CPU/NPU vs. NVIDIA TensorRT GPU).
  * Direct GitHub link to `sentinel-lab`.

---

### Page 6: Company & Mission (`/about` & `/contact`)

* **Executive Mission:** Built by engineers who refused to accept that 60 seconds of data egress delay is acceptable when modern physical attacks destroy machinery in milliseconds.
* **Defense & Enterprise Enquiries Form:**
  * Interactive hardware qualification selector (e.g., Selecting deployment environment: Substation SCADA, Naval/Aviation Enclave, Healthcare Datacenter, Enterprise Corporate Network).
  * Request a Hardware Evaluation Unit or Pilot Appliance Deployment.

---

## 5. High-Converting Interactive UX Modules (Key Differentiators)

To make `aryorithm.com` immediately memorable to CISOs and engineers, incorporate these three interactive widgets:

### 1. Interactive Sub-Millisecond Latency Visualizer
* A split-screen real-time timer animation.
* On the left: **Aryorithm Sentinel** triggers an eBPF drop in **`0.00084 milliseconds`** (green flash: *Mitigated at Kernel Hook*).
* On the right: **Traditional Cloud SIEM** clock ticks continuously for **`24.3 seconds`** (gray text: *Packet buffered $\rightarrow$ Log ingested $\rightarrow$ Cloud forwarded $\rightarrow$ Index query evaluated $\rightarrow$ Ticket created*).

### 2. The 15-Silicon Hardware Backend Selector
* An interactive grid of logos: Intel, NVIDIA, Qualcomm, Rockchip, AMD/Xilinx, Apple, Hailo, Google Coral, etc.
* Clicking any chip reveals:
  * Supported runtime file format (`.xml`, `.engine`, `.rknn`, `.hef`).
  * Execution latency on MobileNet/ResNet models.
  * Memory copy path (Zero-copy DMA-BUF vs. Host Pinned).

### 3. Interactive Web Terminal (Try `nexus-ctl` in Browser)
* An embedded dark terminal emulator where visitors can type real commands:
  * `nexus-ctl fleet list` $\rightarrow$ Displays a simulated table of 5 active edge nodes.
  * `nexus-ctl threat drop 198.51.100.77` $\rightarrow$ Animates sub-50ms fanout across nodes.
  * `nexus-ctl report cmmc` $\rightarrow$ Displays the real CMMC 2.0 Level 2 compliance evaluation score.

---

## 6. Recommended Frontend Tech Stack & Implementation

To mirror the air-gapped, zero-bloat philosophy of the products:

* **Framework:** **Next.js 14+ (App Router)** or **Astro 4+** (for maximum performance and pure static export capability).
* **Styling:** **Tailwind CSS** with custom cyberpunk/industrial dark themes (`bg-[#07090E]`, cyan glow accents).
* **Animations:** **Framer Motion** for smooth hardware telemetry graphs and canvas rendering.
* **Syntax Highlighting:** **Shiki** (ultra-fast, zero-runtime overhead code highlighter for C++20 and eBPF code snippets).
* **Icons:** **Lucide Icons** (clean, crisp, technical SVG icons).
* **Static Deployment:** Can be exported via `next export` into pure static HTML/CSS/JS, allowing the marketing site itself to be hosted directly on an air-gapped Sentinel appliance!
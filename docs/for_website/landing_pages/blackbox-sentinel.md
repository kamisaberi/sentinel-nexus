# Project 3 of 8: `blackbox-sentinel` (`sentinel` daemon)
## Master Landing Page & Technical Architecture Document
**Target URL:** `aryorithm.com/products/sentinel`  
**Repository:** `https://github.com/kamisaberi/blackbox-sentinel`  
**Artifact:** `sentinel` daemon (Standalone Commercial Cyber-Physical XDR & SIEM Appliance)

---

```text
========================================================================================================
                                     PAGE STRUCTURE OUTLINE
========================================================================================================
 1. Hero Section (Headline, Value Proposition, Real-Time Latency KPI Strip)
 2. The Cyber-Physical Challenge: Why Cloud XDR Fails in Operational Technology (OT)
 3. Appliance Form Factors (Turnkey 1U Rackmount, Rugged DIN-Rail, Virtual Hardened VM)
 4. Deep Dive: The 26 Decoupled Native C++20 Subsystem Modules (01_siem_core to 26_ddp)
 5. The 30 Industrial Protocol Dissector Plugins (Modbus, DNP3, PROFINET, S7Comm, DICOM)
 6. Nexus Uplink & Autonomous Collective Defense Synchronization (NexusUplink & KernelDropInjector)
 7. Embedded Air-Gapped Web Command Center (Zero-CDN SPA Architecture on Port 8443)
 8. Deployment & Configuration Blueprint (sentinel.yaml & Systemd Integration)
 9. Sovereign Regulatory Compliance (IEC 62443, CMMC 2.0 Level 2, NIST SP 800-171, EU NIS2)
 10. Technical Frequently Asked Questions (FAQ)
 11. Conversion Call-To-Action (CTA) & Enterprise Pilot Evaluation
========================================================================================================
```

---

## 1. Hero Section

### Badge
`TIER 3 COMMERCIAL APPLIANCE` `CYBER-PHYSICAL XDR & SIEM` `26 NATIVE MODULES` `30 INDUSTRIAL PLUGINS`

### Headline
# Autonomous Cyber-Physical Active Defense. 26 Decoupled Subsystems. Zero Cloud Dependencies.

### Subheadline
**Blackbox Sentinel** is a turnkey cyber-physical active defense appliance engineered for high-assurance critical infrastructure, electrical substations, healthcare diagnostic networks, and naval defense vessels. Combining **26 native C++20 security subsystems** and **30 industrial protocol dissectors** with driver-level **eBPF/XDP kernel drops ($0.84\,\mu\text{s}$)** and **heterogeneous AI inference (`libxinfer`)**, Sentinel stops physical sabotage inline while preserving $0 cloud data egress.

### Primary CTA Group
* `[ Request 14-Day Shadow Mode Evaluation ]` $\rightarrow$ `https://aryorithm.com/contact`
* `[ View 26 Subsystem Architecture ]` $\rightarrow$ `#subsystem-deep-dive`
* `[ Explore Industrial Protocol Plugins ]` $\rightarrow$ `#protocol-plugins`

### Live KPI Strip (Metrics Display Grid)
```text
+---------------------+---------------------+---------------------+---------------------+
|    26 Subsystems    |     30 Plugins      |       0.84 µs       |       $0.00         |
| SIEM, WAF, CWPP,    | Modbus, DNP3, S7,   | In-Kernel eBPF      | Zero Cloud Egress   |
| SCADA CPS, EDR, BAD | PROFINET, DICOM     | Wire-Speed Drop SLA | 100% Air-Gapped     |
+---------------------+---------------------+---------------------+---------------------+
```

---

## 2. The Cyber-Physical Challenge: Why Cloud XDR Fails in Operational Technology (OT)

```text
  ENTERPRISE IT CLOUD XDR APPROACH (CrowdStrike / SentinelOne / Microsoft Defender)
  ┌──────────────────────────────────────────────────────────────────────────────────┐
  │ Physical Machine ──> Python/User Agent ──> JSON Event Extraction ──> Cloud WAN   │
  │ ──> Cloud Ingestion Queue ──> Correlation Logic ──> Operator Alert (15 - 60 sec) │
  │ [ VULNERABLE TO WAN DISRUPTIONS • HIGH EGRESS COSTS • LATENCY DESTROYS ACTUATORS ]│
  └──────────────────────────────────────────────────────────────────────────────────┘
                                          VS.
  BLACKBOX SENTINEL AUTONOMOUS EDGE APPLIANCE
  ┌──────────────────────────────────────────────────────────────────────────────────┐
  │ Industrial Wire (Modbus/DNP3) ──> Physical Interface ──> eBPF/XDP Driver Hook    │
  │ ──> libblackbox SPMC Ring Buffer ──> libxinfer Heterogeneous Neural Scoring      │
  │ ──> Subsystem Physical Constraint Verification ──> Kernel Drop in < 0.84 µs      │
  │ [ 100% ON-PREMISES • HARD REAL-TIME DETERMINISM • MATHEMATICALLY VERIFIED AUDIT ]│
  └──────────────────────────────────────────────────────────────────────────────────┘
```

Modern industrial infrastructure is under constant cyber-physical threat. Attackers no longer focus purely on data exfiltration; they target physical processes:
* **The High-Voltage Breaker Trip:** Altering IEC 60870-5-104 or Modbus commands to disengage protection relays in an electrical substation.
* **The Industrial Actuator Manipulation:** Overriding pressure or temperature safety thresholds in a petrochemical refinery using forced coil commands.
* **The Medical Device Extortion:** Disrupting DICOM PACS radiology archives during active clinical procedures.

Traditional enterprise Extended Detection and Response (XDR) tools cannot protect these environments. They rely on cloud data lakes that introduce **15 to 60 seconds of detection latency**, consume thousands of dollars in cloud egress fees, and crash when WAN connections fail. Furthermore, third-party agents running Python or Java runtimes cannot run on resource-constrained industrial controllers without introducing latency jitter into real-time programmable logic controller (PLC) communication cycles.

Blackbox Sentinel treats the physical edge as an autonomous, self-defending fortress. It processes every packet, flow, and system call **locally in native C++20 memory**, mitigating attacks before network frames reach the operating system's socket buffer.

---

## 3. Appliance Form Factors & Hardware Specifications

Blackbox Sentinel is delivered in three deployment form factors tailored to harsh field conditions, enterprise datacenters, or existing virtualized server clusters:

```text
========================================================================================================
                                    SENTINEL HARDWARE MATRIX
========================================================================================================
 Specification       Model S-1000 (DIN-Rail)        Model S-5000 (1U Rackmount)    Model V-Edge (Virtual VM)
 ──────────────────  ─────────────────────────────  ─────────────────────────────  ─────────────────────────
 Deployment Target   Substations, PLCs, Oil Rigs    Datacenters, Enterprise DMZ    VMware vSphere, KVM, Prox
 Ingress Throughput  1.0 Gbps (Line Rate)           10 / 25 / 40 Gbps (Line Rate)  Depends on vCPU Alloc
 Mitigation SLA      < 1.2 µs (XDP Driver Drop)     < 0.84 µs (XDP Driver Drop)    < 2.5 µs (XDP SKB Mode)
 Target Silicon      Rockchip RK3588 (RKNPU2) or    Dual Intel Xeon / AMD EPYC +   Intel Core / Xeon vCPU
                     Intel Atom (OpenVINO NPU)      NVIDIA L4 TensorRT GPU         (OpenVINO / Host Pinned)
 Network Interfaces  4x 1GbE RJ45 (Bypass Relay)    4x 10GbE SFP+ (Intel X520/810) 2x - 4x Virtual vNICs
 Physical Security   Physical TPM 2.0 (/dev/tpmrm0) Dual Physical TPM 2.0 (TSS2)   vTPM 2.0 / DMI UUID
 Power & Temp        Dual 24V DC (-40°C to +85°C)   Dual Redundant 110/240V AC     Software Defined
 Operating System    Hardened Sovereign Linux 6.8   Hardened Sovereign Linux 6.8   OVA / QCOW2 Pre-built
========================================================================================================
```

---

## 4. Deep Dive: The 26 Decoupled Native C++20 Subsystem Modules

Blackbox Sentinel does not rely on a monolithic codebase. It is constructed from **26 decoupled native C++20 subsystems** (`src/modules/01_siem_core` to `26_ddp`), communicating via unified lock-free memory interfaces. Each module can be independently toggled, tuned, or updated via `sentinel.yaml`:

```text
========================================================================================================
                            BLACKBOX SENTINEL 26-MODULE SUBSYSTEM HIERARCHY
========================================================================================================
 Domain                Subsystem Module Name                  Core Functional Responsibility
 ────────────────────  ─────────────────────────────────────  ──────────────────────────────────────────
 Enterprise IT / SIEM  01_siem_core (SIEM Core)               In-memory log correlation & indexer
                       02_ueba (User Behavior Analytics)      100k+ in-memory entity behavioral matrix
                       03_ndr (Network Detection & Response)  Encrypted traffic analysis via JA3/JA4 TLS
                       04_ids_ips (Inline Packet Filter)      Signature matching with eBPF kernel drops
                       15_ngfw (Next-Gen Firewall DPI)        Deep packet inspection & connection tracking

 Web & Application     05_waf (Web Application Firewall)      API protection against SQLi, XSS, BOLA/IDOR
                       10_bad (Bot & Automated Abuse)         Kinematic mouse/keystroke curve classifier
                       11_rasp (Runtime Self-Protection)      In-memory function hook execution guard

 Host & Endpoint       06_edr (Endpoint Detection & Response) Process tree analyzer & memory hunter
                       07_epp_ngav (Next-Gen Antivirus)       Real-time file Shannon entropy calculator
                       09_cwpp (Container Workload Guard)     eBPF syscall breakout interceptor at sys_enter
                       16_cdr (Content Reconstruction)        Active macro and script stripper for PDF/DOCX
                       20_fse (Firmware Security Evaluation)  UEFI/BIOS binary dissector & CVE scanner

 Identity & Access     08_nac (Network Access Control)        802.1X dynamic VLAN quarantine controller
                       12_itdr (Identity Threat Detection)    Active Directory abuse & Kerberoasting
                       14_ato (Account Takeover Guard)        Impossible travel velocity calculator
                       24_ztna (Zero Trust Regressor)         Dynamic session risk scorer (0.0 to 1.0)

 Industrial & IoT      17_iot_sec (Medical & IoT Security)    DICOM PACS parser & HL7 structure verifier
                       18_cps_sec (Cyber-Physical Security)   SCADA Modbus & DNP3 physical constraint guard
                       21_side_channel (Hardware Defense)     Acoustic, power & EM emission analyzer

 Forensics & Advanced  13_ddos (Hardware Flood Shaper)        Line-rate SYN cookie guard & packet shaper
                       19_swg (Secure Web Gateway)            Sovereign outbound egress proxy
                       22_dfir (Digital Forensics IR)         Ring-buffer PCAP evidence carver with SHA-256
                       23_ai_trism (AI Safety Firewall)       LLM prompt injection & token anomaly filter
                       25_fdp (Fraud Detection Platform)      Financial transaction graph anomaly analyzer
                       26_ddp (Distributed Deception)         Decoy PLCs and synthetic honeypot ports
========================================================================================================
```

### Technical Examination of Mission-Critical Modules

#### Module 18: `18_cps_sec` (Cyber-Physical SCADA Constraint Validator)
Unlike traditional firewalls that only check if port `502` is open, `18_cps_sec` executes stateful semantic dissection on Modbus TCP and DNP3 industrial streams:
* Validates Function Codes against a physical security matrix (e.g., blocking `FC05 Force Single Coil` or `FC15 Write Multiple Coils` during active generation cycles).
* Validates Register Addresses against physical boundaries (e.g., disallowing writes to memory offsets $> 100$ mapped to critical valve actuators).
* Enforces **Command Velocity Limits**: Flags command frequencies exceeding $50\,\text{Hz}$ that attempt to induce physical vibration or mechanical wear on turbines.

#### Module 09: `09_cwpp` (Container eBPF Syscall Guard)
Operating inside the Linux kernel at the `sys_enter` tracepoint:
* Intercepts container breakouts attempting to execute `ptrace`, mount sensitive `/proc` filesystems, or modify host namespaces.
* Terminates offending processes in nanoseconds without relying on slow userspace container security daemons.

#### Module 10: `10_bad` (Kinematic Bot & Automated Abuse Defense)
Analyzes user interaction telemetry to distinguish human operators from automated attack scripts:
* Evaluates mouse trajectory acceleration vectors, touch contact surfaces, and keystroke inter-arrival jitter.
* Humans produce non-linear acceleration curves with variable micro-hesitations. Automated scripts exhibit linear velocity vectors ($\text{Jitter} < 0.1\,\text{ms}$), allowing Sentinel to drop automated credential stuffing without annoying CAPTCHAs.

#### Module 26: `26_ddp` (Distributed Deception Platform)
Deploys synthetic industrial honeypots directly on edge networks:
* Binds virtual decoy PLCs (emulating Siemens S7-300 or Schneider Modbus controllers) strictly to secondary Virtual IPs (VIPs).
* Bypasses port collisions with passive production sniffers. Any connection attempt to a decoy VIP generates an immediate high-fidelity alert, proving malicious reconnaissance.

---

## 5. The 30 Industrial Protocol & Dissector Plugins

Blackbox Sentinel features a modular dynamic plugin architecture. Custom protocol parsers are compiled as independent shared objects (`.so`) located in `src/plugins/` and dynamically loaded at runtime using `dlopen(..., RTLD_LAZY | RTLD_LOCAL)`:

```text
+------------------------------------------------------------------------------------------------------+
|                                   BLACKBOX SENTINEL DAEMON (C++20)                                   |
|                                                                                                      |
|   PluginManager::load_plugin("/usr/local/lib/sentinel/plugins/libmodbus_dissector.so")               |
|   └── Enforces strict symbol isolation (-fvisibility=hidden) to prevent linker collisions            |
+------------------------------------------------------------------------------------------------------+
         │                                       │                                      │
         ▼                                       ▼                                      ▼
 ┌─────────────────────────┐           ┌─────────────────────────┐            ┌─────────────────────────┐
 │ PLUGIN 01: Modbus TCP   │           │ PLUGIN 02: DNP3 Substa  │            │ PLUGIN 03: Siemens S7   │
 │ • APDU Field Parser     │           │ • Class 0/1/2/3 Polls   │            │ • TPKT / COTP Decapsule │
 │ • Coil Override Guard   │           │ • Outstation Trip Check │            │ • Block Read/Write Trap │
 └─────────────────────────┘           └─────────────────────────┘            └─────────────────────────┘
```

### Full Protocol Dissector Directory

```text
========================================================================================================
                          30 DYNAMIC C++ INDUSTRIAL DISSECTOR PLUGINS
========================================================================================================
 Industrial OT / SCADA    Energy & Utilities       Aviation & Maritime      Healthcare & Enterprise
 ──────────────────────   ──────────────────────   ──────────────────────   ───────────────────────
 • libmodbus_dissector    • libiec104_dissector    • libmavlink_uav         • libdicom_pacs
 • libdnp3_dissector      • libiec61850_goose      • libais_maritime        • libhl7_v2
 • libs7comm_dissector    • libiec61850_mms        • libnmea_gps            • libcef_forwarder
 • libprofinet_dissector  • libopc_ua_dissector    • libadsb_avionics       • libleef_forwarder
 • libethernet_ip         • libbacnet_building     • libstanag_4586         • libsyslog_rfc5424
 • libhart_ip             • libmodbus_rtu_serial   • libmil_std_1553        • libkafka_producer
 • libmitsubishi_melsec   • libenip_cip            • libcanbus_automotive   • libsnmp_v3_trap
 • libomron_fins          • libfieldbus_h1                                  • libnetflow_v9_ipfix
========================================================================================================
```

---

## 6. Nexus Uplink & Collective Defense Synchronization

When connected to an enterprise grid, Blackbox Sentinel activates **`NexusUplink`** (`src/nexus/NexusUplink.cpp`), turning individual edge sensors into a synchronized collective defense mesh:

```text
========================================================================================================
                      COLLECTIVE DEFENSE: "ATTACKED ONCE, IMMUNE EVERYWHERE"
========================================================================================================

 [ ADVERSARY STRIKES SUBSTATION 01 ]
  • Exploit payload hits Edge Node 01 interface (10.240.0.101).
  • Substation 01 executes local in-kernel eBPF drop (< 0.84µs).
                 │
                 ▼ (gRPC Stream: ThreatIndicator emitted)
 [ TIER 6: SENTINEL NEXUS COMMAND PLANE (10.240.0.10) ]
  • Ingests ThreatIndicator; updates MITRE ATT&CK cache.
  • Instantly generates FleetDefenseRule.
                 │
                 ▼ (Parallel gRPC Fanout in < 50ms)
 ┌───────────────┴───────────────────────────────┐
 │                                               │
 ▼                                               ▼
 [ REFINERY 03 (10.240.0.103) ]                  [ HOSPITAL PACS 02 (10.240.0.102) ]
 • Ingests FleetDefenseRule.                     • Ingests FleetDefenseRule.
 • KernelDropInjector calls:                     • KernelDropInjector calls:
   bpf_map_update_elem(blocked_ip_map)             bpf_map_update_elem(blocked_ip_map)
                 │                                               │
                 ▼                                               ▼
 [ Attacker Blocked at Kernel Driver Ring ]      [ Attacker Blocked at Kernel Driver Ring ]
========================================================================================================
```

### Key Capabilities of `NexusUplink`:
1. **Automated Hardware Enrollment:** At boot, queries physical TPM 2.0 or DMI machine registers, signs an attestation payload, and enrolls with Nexus via `RegisterAppliance`.
2. **Instant 0 ms Graceful Disconnect:** When Sentinel receives `SIGINT` (`Ctrl+C`) or shuts down, it transmits a `DeregistrationRequest` frame. Nexus marks the appliance `OFFLINE` in **0 milliseconds**, bypassing the standard 15-second heartbeat timeout.
3. **Automated Canary OTA Model Pull & Live Hot-Reload:**  
   Every 15 seconds, Sentinel polls `ModelOtaService`. When a candidate model is promoted to `STAGE_FLEET_WIDE`:
   * Sentinel automatically downloads the `.onnx` binary from Nexus over HTTP.
   * Computes the cryptographic SHA-256 hash using OpenSSL and verifies it against the signed manifest.
   * Invokes Sentinel's internal control API (`POST /api/v1/control/reload-model`), executing a **zero-downtime model hot-reload** without dropping in-flight network packets.

---

## 7. Embedded Air-Gapped Web Command Center (Port 8443)

Each Sentinel appliance embeds a standalone, high-performance web command center operating on port **`8443`** (HTTPS/TLS):

```text
+------------------------------------------------------------------------------------------------------+
|  BLACKBOX SENTINEL [NODE: Edge-Substation-01] [SITE: PowerGrid-North] [UPTIME: 42d 14h] [eBPF: ACTIVE]|
+------------------------------------------------------------------------------------------------------+
|  ACTIVE TELEMETRY:                                                                                   |
|  • CPU Utilization: 12.5%          • Memory Usage: 240 MB          • NPU Temperature: 48.5°C         |
|  • Wire Packets Inspected: 150,000 • In-Kernel Drops: 42            • Mean Mitigation SLA: 0.84 µs   |
+------------------------------------------------------------------------------------------------------+
|  ACTIVE eBPF KERNEL BLOCKED IP TABLE (blocked_ip_map):                                               |
|  IP Address        Target Port   TTL Remaining   Rule Provenance           Action                    |
|  198.51.100.45     502 (Modbus)  23h 14m 02s     Local Subsystem 18_cps    [ 1-Click Unblock ]       |
|  203.0.113.88      443 (HTTPS)   18h 02m 11s     Nexus Collective Defense  [ 1-Click Unblock ]       |
|  192.0.2.144       102 (S7Comm)  12h 45m 50s     Nexus Collective Defense  [ 1-Click Unblock ]       |
+------------------------------------------------------------------------------------------------------+
|  LIVE THREAT CONSOLE:                                                                                |
|  [14:02:11] ALERT: Modbus Function 0x05 override blocked on Register 105 (Turbine Cooling Relief)   |
|  [14:02:11] XAI DECOMPOSITION: 1. SCADA_FC=5 (54.2%) | 2. Rate=184.2Hz (28.1%) | 3. Reg=105 (14.8%)   |
+------------------------------------------------------------------------------------------------------+
```

### Zero-CDN Air-Gapped Guarantee
The Web Command Center contains **zero external script tags, zero Google Fonts, and zero CDN links**. All CSS styling, JavaScript engines, and SVG icons are stored locally on the appliance flash filesystem, ensuring that opening the management dashboard inside an air-gapped nuclear facility or naval vessel will never leak a single DNS request to the public internet.

---

## 8. Deployment & Configuration Blueprint

### Configuration Reference: `sentinel.yaml`
Saved at `/etc/sentinel/sentinel.yaml`:

```yaml
# ==============================================================================
# BLACKBOX SENTINEL APPLIANCE CONFIGURATION
# ==============================================================================

appliance:
  identifier: "Edge-Substation-01"
  site: "PowerGrid-North-01"
  mode: "ACTIVE_MITIGATION"          # Options: ACTIVE_MITIGATION or STAGE_SHADOW_MODE
  primary_interface: "eth0"
  xdp_driver_mode: "SKB"             # Options: DRV (Physical 10GbE) or SKB (Virtual/VMware)

nexus_uplink:
  enabled: true                      # Connects to Sentinel Nexus fleet grid
  host: "10.240.0.10"
  port: 50051
  nexus_http_port: 9443
  sentinel_local_api_port: 8443
  heartbeat_interval_sec: 5
  active_model_name: "network_threat_v1.onnx"
  local_models_dir: "/etc/sentinel/models"

subsystems:
  siem_core: true
  scada_constraint_validator: true
  web_application_firewall: true
  container_syscall_guard: true
  bot_abuse_defense: true
  ransomware_entropy_guard: true

scada_policy:
  enforce_strict_function_codes: true
  allowed_modbus_functions: [1, 2, 3, 4] # Read-only baseline; writes require explicit authorization
  max_command_rate_hz: 50.0
```

### Systemd Service Deployment
Saved at `/etc/systemd/system/blackbox-sentinel.service`:

```ini
[Unit]
Description=Blackbox Sentinel - Autonomous Edge Cyber-Physical XDR Engine (Tier 3)
After=network.target network-online.target
Wants=network-online.target

[Service]
Type=simple
User=root
WorkingDirectory=/etc/sentinel
ExecStart=/usr/local/bin/sentinel /etc/sentinel/sentinel.yaml
Restart=always
RestartSec=3s

# Real-Time Capabilities & Resource Allocation
LimitNOFILE=65536
TasksMax=4096
AmbientCapabilities=CAP_NET_ADMIN CAP_SYS_ADMIN CAP_BPF
Nice=-10
CPUSchedulingPolicy=rr
CPUSchedulingPriority=80

[Install]
WantedBy=multi-user.target
```

Launch the daemon:
```bash
sudo systemctl daemon-reload
sudo systemctl enable --now blackbox-sentinel
sudo systemctl status blackbox-sentinel
```

---

## 9. Sovereign Regulatory Compliance

```text
========================================================================================================
                                REGULATORY COMPLIANCE ATTESTATION
========================================================================================================

 [ IEC 62443-3-3 & IEC 62443-4-2 (INDUSTRIAL AUTOMATION AND CONTROL SYSTEMS) ]
  • Zone Boundary Enforcement (FR 5): Enforces deterministic deep packet inspection and line-rate
    filtering between Safety Instrumented Systems (SIS) and Basic Process Control Systems (BPCS).
  • System Integrity (FR 3): Validates command semantics for Modbus, DNP3, and PROFINET, preventing
    unauthorized physical actuator movement.

 [ CMMC 2.0 (LEVEL 2) & NIST SP 800-171 (DEFENSE INDUSTRIAL BASE) ]
  • Incident Response (SI.L2-3.14.1): Automatically mitigates malicious indicators in < 0.84µs,
    providing mathematical log proof of threat containment prior to host execution.
  • Identification & Authentication (IA.L2-3.5.1): Anchors appliance identity to physical TPM 2.0
    cryptoprocessors via Endorsement Keys and signed PCR 0/4 quotes.

 [ EU NIS2 DIRECTIVE (HIGHLY CRITICAL ENTITIES) ]
  • Article 21 Cybersecurity Measures: Provides sovereign, air-gapped incident handling with zero
    reliance on external third-party cloud data lakes ($0 cloud data egress).
========================================================================================================
```

---

## 10. Technical Frequently Asked Questions (FAQ)

#### Q: How does Blackbox Sentinel prevent port collisions when running deception honeypots?
**A:** Module 26 (`26_ddp`) uses **Virtual IP (VIP) binding**. When deployed on a network, Sentinel assigns secondary IP addresses to its physical interface. Production sniffers and inspection engines listen promiscuously across all traffic, while honeypot responders (decoy PLCs or fake SSH ports) bind exclusively to the secondary VIPs. Legitimate production servers never experience port or socket conflicts.

#### Q: Can Sentinel operate in a purely passive mode during customer evaluations?
**A:** Yes. By setting `mode: "STAGE_SHADOW_MODE"` in `sentinel.yaml`, Sentinel connects strictly to switch mirror/SPAN ports or optical TAPs. In Shadow Mode, the appliance performs full neural classification, protocol dissection, and XAI feature attribution, but **enforces zero packet drops and zero packet injections**. It produces complete audit reports without any risk of disrupting physical PLC communication loops.

#### Q: What happens if an appliance loses network connection to Sentinel Nexus?
**A:** Sentinel appliances are fully autonomous by design. If a WAN link drops, the appliance continues running its local eBPF kernel filter, inference engines, and 26 subsystems without degradation. When the connection to Nexus is restored, buffered telemetry, candidate vectors, and audit logs are automatically synchronized.

#### Q: How does the appliance verify firmware updates without an internet connection?
**A:** In air-gapped enclaves, updates are applied via cryptographically signed sneakernet update bundles (`.snbundle`). The appliance uses its local GPG public keys and TPM 2.0 hardware registers to verify the cryptographic signature and SHA-256 hash of the update package before unbundling binaries or hot-reloading models.

---

## 11. Conversion Call-To-Action (CTA)

```text
+------------------------------------------------------------------------------------------------------+
|                     DEPLOY AUTONOMOUS ACTIVE DEFENSE ON YOUR INDUSTRIAL EDGE                         |
|                                                                                                      |
|   Stop relying on 60-second cloud alerts to protect physical machinery. Deploy Blackbox Sentinel     |
|   for deterministic, sub-microsecond in-kernel active defense with zero cloud data egress.           |
|                                                                                                      |
|   [ Request 14-Day Shadow Pilot ]       [ View Technical Blueprint ]          [ Contact OT Security ]|
|   aryorithm.com/contact                 aryorithm.com/products/sentinel       research@aryorithm.com |
+------------------------------------------------------------------------------------------------------+
```

---

### End of Project 3 Document
*Ready to proceed to **Project 4: `xinfer-forge` (Tier 4 Continuous Active Learning Service)** upon your confirmation.*
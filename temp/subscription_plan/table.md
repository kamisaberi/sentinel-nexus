Here is the master commercial pricing and subscription matrix for your website team. 

It provides an itemized checklist covering:
* **All 8 Software Tiers**
* **All 26 Native Subsystems**
* **All 30 Industrial Protocol Plugins**
* **All 28 Cloud SaaS Services**

---

# Aryorithm Commercial Subscription Matrix (`aryorithm.com/pricing`)

```text
========================================================================================================================
                                             SUBSCRIPTION TIERS AT A GLANCE
========================================================================================================================
 PLAN NAME:          COMMUNITY / RESEARCH      ENTERPRISE IT              CRITICAL INFRASTRUCTURE (OT)  SOVEREIGN DEFENSE
 PRICE:              €0 (Free Forever)         €299 / node / month        €699 / node / month           Custom Enterprise / Year
 TARGET:             Developers, Academia      Corporate IT, Cloud, FinTech Substation, Factory, Hospital Defense, Nuclear, Classified
 DEPLOYMENT:         Self-Hosted (Local Only)  Cloud Hybrid (SaaS + Edge) Hybrid or 100% Air-Gapped     100% Air-Gapped Sovereign
 NODE CAPACITY:      Max 1 Node (Lab: 3 Nodes) Unlimited                  Unlimited                     Unlimited
 LICENSING:          FOSS (Apache/MIT)         SaaS JWT / Commercial EULA Commercial Air-Gapped (.lic)  Air-Gapped Hardware Dongle
 SUPPORT:            Community / Discord       99.9% Cloud SLA, Standard  24/7/365 L3, 15-min Critical   Dedicated On-Site Team
========================================================================================================================
```

---

## Table 1: The 8 Software Tiers & Core Runtimes

| Software Tier & Component | Community (Free) | Enterprise IT (€299) | Critical Infrastructure (€699) | Sovereign Defense (Custom) | Technical Gating Mechanism |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **Tier 1: `xinfer-essential` (`libxinfer.so`)** | **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | Apache 2.0 (Open Source) |
| **Tier 2: `blackbox-essential` (`libblackbox.so`)** | **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | Apache 2.0 / GPLv2 (Open Source) |
| **Tier 3: `blackbox-sentinel` (Appliance Daemon)** | Limited (5 Modules) | **✔ Included** (IT Tier) | **✔ Included** (Full OT) | **✔ Included** (Custom Silicon) | C++ `LicenseManager` Ed25519 Key |
| **Tier 4: `xinfer-forge` (`forge-cli`)** | Limited (Manual CLI) | **✔ Included** | **✔ Included** | **✔ Included** | Automated Safety Gate in `forge-cli` |
| **Tier 5: `sentinel-lab` (Academic Testbed)** | **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | MIT License (Open Access / Zenodo) |
| **Tier 6: `sentinel-nexus` (Command Plane)** | Limited (Max 3 Nodes) | **✔ Included** (Cloud C2) | **✔ Included** (On-Prem/Cloud)| **✔ Included** (Multi-Cluster HA) | Node quota check in C++ daemon |
| **Tier 7: `sentinel-matrix` (Cyber-Range)** | Limited (Basic Traffic) | **✔ Included** (API/Web) | **✔ Included** (Full OT Mesh)| **✔ Included** (Full Digital Twin) | Docker Compose profile entitlement |
| **Tier 8: `sentinel-stack` (Meta-Installer)** | **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | Apache 2.0 (Open Source) |

---

## Table 2: The 26 Decoupled Native Subsystems

| Module Identifier & Subsystem Name | Community (Free) | Enterprise IT (€299) | Critical Infrastructure (€699) | Sovereign Defense (Custom) | Primary Operational Role |
| :--- | :---: | :---: | :---: | :---: | :--- |
| `01_siem_core` (SIEM Core) | **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | In-memory log correlation & indexer |
| `02_ueba` (User Entity Behavior Analytics) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | 100k+ in-memory entity behavioral matrix |
| `03_ndr` (Network Detection & Response) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Encrypted Traffic Analysis (JA3/JA4 TLS) |
| `04_ids_ips` (Inline Packet Filter) | **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | Signature matching & local eBPF drops |
| `05_waf` (Web App & API Protection) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | In-kernel SQLi, XSS, BOLA/IDOR blocker |
| `06_edr` (Endpoint Detection & Response) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Process tree analyzer & memory injection hunter |
| `07_epp_ngav` (Next-Gen Antivirus) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Real-time file Shannon entropy calculator |
| `08_nac` (Network Access Control) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | 802.1X dynamic VLAN quarantine controller |
| `09_cwpp` (Container Workload Guard) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | eBPF container breakout guard at `sys_enter` |
| `10_bad` (Bot & Automated Abuse Defense) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Kinematic mouse/keystroke curve classifier |
| `11_rasp` (Runtime Self-Protection) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | In-memory hook execution guard |
| `12_itdr` (Identity Threat Detection) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Kerberoasting & Active Directory abuse |
| `13_ddos` (Hardware Line-Rate Flood Shaper) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Hardware line-rate flood shaper & SYN cookies |
| `14_ato` (Account Takeover Guard) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Impossible travel geo-velocity evaluator |
| `15_ngfw` (Next-Gen Firewall DPI) | **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | Deep packet inspection & connection tracking |
| `16_cdr` (Content Disarm & Reconstruction) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Active macro/script stripper for PDF/DOCX |
| `17_iot_sec` (Medical & IoT Security) | ✖ | ✖ | **✔ Included** | **✔ Included** | DICOM PACS and HL7 v2 medical parser |
| `18_cps_sec` (SCADA Constraint Validator) | ✖ | ✖ | **✔ Included** | **✔ Included** | Modbus & DNP3 physical constraint guard |
| `19_swg` (Secure Web Gateway) | **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | Sovereign outbound egress proxy |
| `20_fse` (Firmware Security Evaluation) | ✖ | ✖ | **✔ Included** | **✔ Included** | UEFI/BIOS binary flash memory dissector |
| `21_side_channel` (Hardware Defense) | ✖ | ✖ | **✔ Included** | **✔ Included** | Acoustic, power & EM emission anomaly analyzer |
| `22_dfir` (Digital Forensics IR) | **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | Ring-buffer PCAP evidence carver with SHA-256 |
| `23_ai_trism` (AI Safety Firewall) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | LLM prompt injection & token anomaly filter |
| `24_ztna` (Zero Trust Regressor) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Dynamic session risk regressor ($0.0 - 1.0$) |
| `25_fdp` (Fraud Detection Platform) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Financial transaction graph anomaly analyzer |
| `26_ddp` (Distributed Deception) | ✖ | ✖ | **✔ Included** | **✔ Included** | Decoy PLCs and honeypot ports on secondary VIPs |

---

## Table 3: The 30 Dynamic Industrial Protocol Plugins

*(Loaded dynamically via `IInferencePlugin` with `-fvisibility=hidden`)*

| Plugin Library (`.so`) | Protocol & Modality | Community (Free) | Enterprise IT (€299) | Critical Infrastructure (€699) | Sovereign Defense (Custom) | Target Vertical |
| :--- | :--- | :---: | :---: | :---: | :---: | :--- |
| `libmodbus_dissector.so` | Modbus TCP (Port 502) | ✖ | ✖ | **✔ Included** | **✔ Included** | Industrial Automation, Power |
| `libdnp3_dissector.so` | DNP3 Protocol (Port 20000) | ✖ | ✖ | **✔ Included** | **✔ Included** | Electrical Substations, Water |
| `libs7comm_dissector.so` | Siemens S7Comm (Port 102) | ✖ | ✖ | **✔ Included** | **✔ Included** | Manufacturing, Automotive |
| `libprofinet_dissector.so`| PROFINET Real-Time | ✖ | ✖ | **✔ Included** | **✔ Included** | Factory Automation Loops |
| `libethernet_ip.so` | EtherNet/IP (CIP) | ✖ | ✖ | **✔ Included** | **✔ Included** | Rockwell Automation / Allen-Bradley |
| `libhart_ip.so` | WirelessHART / HART-IP | ✖ | ✖ | **✔ Included** | **✔ Included** | Refineries, Chemical Sensors |
| `libmitsubishi_melsec.so` | MELSEC-Q Protocol | ✖ | ✖ | **✔ Included** | **✔ Included** | Semiconductor Manufacturing |
| `libomron_fins.so` | Omron FINS Ethernet | ✖ | ✖ | **✔ Included** | **✔ Included** | Packaging & Conveyor Logistics |
| `libiec104_dissector.so` | IEC 60870-5-104 (Port 2404) | ✖ | ✖ | **✔ Included** | **✔ Included** | Power Transmission & Grids |
| `libiec61850_goose.so` | IEC 61850 GOOSE (Layer 2) | ✖ | ✖ | **✔ Included** | **✔ Included** | High-Voltage Substation Bus |
| `libiec61850_mms.so` | IEC 61850 MMS Protocol | ✖ | ✖ | **✔ Included** | **✔ Included** | Substation SCADA Gateway |
| `libopc_ua_dissector.so` | OPC UA Binary (Port 4840) | ✖ | ✖ | **✔ Included** | **✔ Included** | Industry 4.0 Telemetry Hub |
| `libbacnet_building.so` | BACnet/IP (Port 47808) | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Smart Building Automation |
| `libmodbus_rtu_serial.so`| Serial Modbus RTU / RS-485 | ✖ | ✖ | **✔ Included** | **✔ Included** | Legacy Fieldbus Controllers |
| `libenip_cip.so` | Common Industrial Protocol | ✖ | ✖ | **✔ Included** | **✔ Included** | Industrial Robotic Assembly |
| `libfieldbus_h1.so` | Foundation Fieldbus H1 | ✖ | ✖ | **✔ Included** | **✔ Included** | Process Instrumentation |
| `libmavlink_uav.so` | MAVLink UAV Protocol | ✖ | ✖ | **✔ Included** | **✔ Included** | Drone & Autonomous Robotics |
| `libais_maritime.so` | AIS Maritime Transponder | ✖ | ✖ | **✔ Included** | **✔ Included** | Commercial Shipping, Ports |
| `libnmea_gps.so` | NMEA-0183 / 2000 GPS | ✖ | ✖ | **✔ Included** | **✔ Included** | Navigation Sensor Spoofing |
| `libadsb_avionics.so` | ADS-B Mode-S Transponder | ✖ | ✖ | **✔ Included** | **✔ Included** | Air Traffic Management |
| `libstanag_4586.so` | NATO STANAG 4586 UAV | ✖ | ✖ | ✖ | **✔ Included** | Military Drone Datalinks |
| `libmil_std_1553.so` | MIL-STD-1553 Avionics | ✖ | ✖ | ✖ | **✔ Included** | Fighter Aircraft, Armored Vehicles |
| `libcanbus_automotive.so`| CAN 2.0B / CAN-FD Bus | ✖ | ✖ | **✔ Included** | **✔ Included** | Connected Vehicle ECUs |
| `libdicom_pacs.so` | DICOM C-STORE Protocol | ✖ | ✖ | **✔ Included** | **✔ Included** | Hospital Radiology / MRI |
| `libhl7_v2.so` | HL7 v2 Clinical Messaging | ✖ | ✖ | **✔ Included** | **✔ Included** | Hospital Patient Diagnostics |
| `libcef_forwarder.so` | Common Event Format (CEF) | **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | Enterprise SIEM Integration |
| `libleef_forwarder.so` | IBM QRadar LEEF Forwarder | **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | Enterprise SIEM Integration |
| `libsyslog_rfc5424.so` | Syslog RFC 5424 Streamer | **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | Universal Log Ingestion |
| `libkafka_producer.so` | Apache Kafka Zero-Copy | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Enterprise Streaming Lakes |
| `libsnmp_v3_trap.so` | SNMPv3 Encrypted Traps | **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | Network Device Telemetry |
| `libnetflow_v9_ipfix.so`| IPFIX / NetFlow v9 Blaster| **✔ Included** | **✔ Included** | **✔ Included** | **✔ Included** | Wire-Speed Flow Extraction |

---

## Table 4: The 28 Cloud SaaS Services (`app.aryorithm.com`)

| # | SaaS Service Offering | Domain | Community (Free) | Enterprise IT (€299) | Critical Infrastructure (€699) | Sovereign Defense (Custom) | Primary Cloud Capability |
| :---: | :--- | :--- | :---: | :---: | :---: | :---: | :--- |
| **1** | **Sentinel Nexus Cloud** | Fleet C2 | ✖ *(Local only, $\le$ 3)* | **✔ Included** | **✔ Included** | **✔ Included** | Multi-tenant cloud command center |
| **2** | **Zero-Touch Provisioning (ZTP)** | Fleet C2 | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Automated cloud enrollment via TPM 2.0 |
| **3** | **Multi-Tenant MSSP Portal** | Fleet C2 | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Hierarchical multi-client management |
| **4** | **xInfer Compiler Cloud** | AI & Silicon | ✖ | 50 builds/mo | 250 builds/mo | **Unlimited Dedicated** | Cross-compile models for 15 silicon targets |
| **5** | **Federated Continual Retraining** | AI & Silicon | ✖ | ✖ | **✔ Included** | **✔ Included** | Cross-fleet privacy-preserving model tuning |
| **6** | **AI TRiSM Gateway** | AI & Silicon | ✖ | 50k tokens/mo | 1M tokens/mo | **Unlimited Dedicated** | LLM prompt injection firewall proxy |
| **7** | **Global Collective Defense Feed** | CTI | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Sub-50ms crowdsourced zero-day feed |
| **8** | **OT/SCADA Exploit Warning Feed** | CTI | ✖ | ✖ | **✔ Included** | **✔ Included** | Early-warning feed for weaponized PLC exploits |
| **9** | **Ransomware IOC Clearinghouse** | CTI | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Real-time high-entropy process hash registry |
| **10**| **Automated EU NIS2 / DORA Vault**| Compliance | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Timestamped incident mitigation SLA proofs |
| **11**| **CMMC 2.0 / NIST 800-171 Vault** | Compliance | ✖ | ✖ | **✔ Included** | **✔ Included** | Assessor-ready packages with TPM quotes |
| **12**| **Cyber Insurance Risk Verifier** | Compliance | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Cryptographic proof for 20%–40% premium cuts |
| **13**| **Dynamic SBOM & VEX Tracker** | Compliance | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Cloud CycloneDX/SPDX vulnerability indexer |
| **14**| **SCADA Actuator Analytics** | CPS Security | ✖ | ✖ | **✔ Included** | **✔ Included** | Long-term PLC command chatter & wear analysis |
| **15**| **Medical IoMT & DICOM Cloud** | CPS Security | ✖ | ✖ | **✔ Included** | **✔ Included** | Hospital radiology network security hub |
| **16**| **Maritime & Vessel Fleet Portal**| CPS Security | ✖ | ✖ | **✔ Included** | **✔ Included** | Low-bandwidth satellite telemetry tracking |
| **17**| **Identity Threat (ITDR) Cloud** | Identity | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Entra ID & Active Directory abuse monitoring |
| **18**| **Kinematic Bot Defense API** | Identity | ✖ | 100k req/mo | 1M req/mo | **Unlimited Dedicated** | Web API scoring mouse/touch trajectory curves |
| **19**| **Deception & Honeypot Cloud** | Identity | ✖ | ✖ | **✔ Included** | **✔ Included** | Remote VIP rotation of decoy industrial PLCs |
| **20**| **Dynamic Zero Trust Scorer** | Identity | ✖ | **✔ Included** | **✔ Included** | **✔ Included** | Real-time session risk scoring ($0.0 - 1.0$) |
| **21**| **Cloud Evidence Vault (PCAP)** | Forensics | ✖ | 10 GB Storage | 100 GB Storage | **Unlimited Storage** | Signed PCAP evidence with legal chain-of-custody |
| **22**| **Cloud CDR Document Sanitizer** | Forensics | ✖ | 500 docs/mo | 5,000 docs/mo | **Unlimited Dedicated** | Cloud API stripping macros from PDF/DOCX |
| **23**| **Firmware Backdoor Scanner** | Forensics | ✖ | ✖ | 10 scans/mo | **Unlimited Dedicated** | Static binary analysis for router/PLC firmware |
| **24**| **Matrix Digital Twin Cyber-Range**| Simulation | ✖ | 10 hours/mo | 50 hours/mo | **Unlimited Dedicated** | Cloud-hosted virtual replica of customer plants |
| **25**| **Automated Breach Simulation (BAS)**| Simulation | ✖ | 1 run/mo | 10 runs/mo | **Continuous Daily** | Scheduled nmap/Modbus attack simulations |
| **26**| **Resilience & MTTR Certification** | Simulation | ✖ | Annual Audit | Semi-Annual Audit| **Continuous Audit** | Certified Mean Time to Fleet Immunity proof |
| **27**| **Co-Managed CPS SOC (MDR)** | Managed | ✖ | Optional Add-on| **✔ Included** | **Dedicated Analysts** | 24/7 human-in-the-loop alert triage |
| **28**| **Incident Response Guarantee** | Managed | ✖ | 4-Hour SLA | 1-Hour SLA | **15-Minute Critical SLA** | Emergency on-demand kernel & SCADA response |

---

## 5. Technical Implementation Notes for the Website Team

When building the pricing UI (`aryorithm.com/pricing`):
1. **Interactive Toggle:** Implement a prominent toggle between **"Annual Billing (20% Discount)"** and **"Monthly Billing"**.
2. **Feature Filtering Tabs:** Enable tabbed filters above the comparison table:
   * `[ All Features (92) ]`
   * `[ Core Software Tiers (8) ]`
   * `[ Security Subsystems (26) ]`
   * `[ Industrial Plugins (30) ]`
   * `[ Cloud SaaS Services (28) ]`
3. **CTA Buttons per Column:**
   * Community Tier: `[ Clone on GitHub ]` $\rightarrow$ `https://github.com/kamisaberi/sentinel-stack`
   * Enterprise IT Tier: `[ Start 14-Day Cloud Trial ]` $\rightarrow$ `https://app.aryorithm.com/portal/register`
   * Critical Infrastructure OT Tier: `[ Request 14-Day Shadow Pilot ]` $\rightarrow$ `/contact`
   * Sovereign Defense Tier: `[ Request Sovereign Consultation ]` $\rightarrow$ `/contact?tier=defense`
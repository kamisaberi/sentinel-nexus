# Project 3 of 8: `blackbox-sentinel` (`sentinel` daemon)
## Complete Documentation & Help Desk System File Structure (`docs/`)

This is the complete, full-version documentation tree for **`blackbox-sentinel`**. It is formatted for enterprise documentation platforms (**MkDocs Material**, **Docusaurus**, or **Starlight**) and covers all **26 native subsystems**, **30 industrial protocol plugins**, **Nexus fleet uplink synchronization**, **air-gapped web management**, and **regulatory compliance auditing**.

---

```text
blackbox-sentinel/docs/
├── mkdocs.yml                                 # Documentation site configuration (navigation, theme, search)
├── index.md                                   # Documentation home & executive architectural overview
│
├── getting-started/                           # Onboarding & Initial Setup
│   ├── overview.md                            # Autonomous edge active defense & SIEM appliance introduction
│   ├── hardware-specifications.md             # Model S-1000 (DIN-Rail), S-5000 (1U), and V-Edge (Virtual VM)
│   ├── system-requirements.md                 # Kernel 6.8+, physical/vTPM, NIC driver requirements, and RAM
│   ├── installation.md                        # Bare-metal installation, systemd daemon deployment, and OVA/QCOW2
│   ├── initial-configuration.md               # Setting up /etc/sentinel/sentinel.yaml from scratch
│   ├── shadow-mode-evaluation.md              # 14-day zero-risk passive SPAN evaluation setup (STAGE_SHADOW_MODE)
│   ├── first-threat-mitigation.md             # Verifying your first live in-kernel eBPF packet drop (< 0.84µs)
│   └── verifying-appliance-health.md          # Health inspection via CLI, systemd, and local web UI
│
├── architecture/                              # Deep Systems Design
│   ├── appliance-architecture.md              # Decoupled C++20 engine design & multi-subsystem orchestration
│   ├── unidirectional-pipeline.md             # Ingest -> Tensor Extraction -> Inference -> Kernel Mitigation -> Egress
│   ├── memory-safety-invariants.md            # Guaranteed backing buffer allocations & zero heap fragmentation
│   ├── dynamic-plugin-loader.md               # dlopen(RTLD_LAZY | RTLD_LOCAL) mechanics and symbol isolation
│   ├── port-arbitration-model.md              # Promiscuous raw sockets vs. secondary VIPs for zero port collisions
│   └── under-the-hood-bindings.md             # In-process bindings to Tier 1 libxinfer and Tier 2 libblackbox
│
├── subsystems-26/                             # Complete Guides for the 26 Native C++20 Subsystems
│   ├── index.md                               # Subsystem overview & runtime dependency matrix
│   ├── enterprise-it/                         # Enterprise IT & Detection Subsystems
│   │   ├── 01-siem-core.md                    # 01_siem_core: In-memory log correlation & indexer
│   │   ├── 02-ueba.md                         # 02_ueba: User & Entity Behavior Analytics (100k state matrix)
│   │   ├── 03-ndr.md                          # 03_ndr: Network Detection & Response (TLS JA3/JA4 analysis)
│   │   ├── 04-ids-ips.md                      # 04_ids_ips: Inline signature matching & eBPF driver drops
│   │   └── 15-ngfw.md                         # 15_ngfw: Next-Gen Firewall Deep Packet Inspection (DPI)
│   ├── web-application/                       # Web & Application Layer Protection
│   │   ├── 05-waf.md                          # 05_waf: Web Application & API Protection (SQLi, XSS, BOLA/IDOR)
│   │   ├── 10-bad.md                          # 10_bad: Bot & Automated Abuse Defense (Kinematic curve classifier)
│   │   └── 11-rasp.md                         # 11_rasp: Runtime Application Self-Protection (In-memory hook guard)
│   ├── host-endpoint/                         # Host, Workload & Binary Security
│   │   ├── 06-edr.md                          # 06_edr: Endpoint process tree analyzer & memory injection hunter
│   │   ├── 07-epp-ngav.md                     # 07_epp_ngav: Real-time file Shannon entropy calculator & IOPS blocker
│   │   ├── 09-cwpp.md                         # 09_cwpp: Container eBPF syscall breakout guard at sys_enter
│   │   ├── 16-cdr.md                          # 16_cdr: Content Disarm & Reconstruction (Macro stripper)
│   │   └── 20-fse.md                          # 20_fse: Firmware Security Evaluation (UEFI/BIOS binary dissector)
│   ├── identity-access/                       # Identity & Access Governance
│   │   ├── 08-nac.md                          # 08_nac: 802.1X dynamic VLAN quarantine controller
│   │   ├── 12-itdr.md                         # 12_itdr: Identity threat detection (Kerberoasting & AD abuse)
│   │   ├── 14-ato.md                          # 14_ato: Account takeover & impossible travel geo-velocity check
│   │   └── 24-ztna.md                         # 24_ztna: Dynamic Zero Trust session risk regressor (0.0 to 1.0)
│   ├── cyber-physical-ot/                     # Critical Infrastructure & IoT Protection
│   │   ├── 17-iot-sec.md                      # 17_iot_sec: Medical DICOM PACS & HL7 clinical protocol security
│   │   ├── 18-cps-sec.md                      # 18_cps_sec: SCADA OT physical constraint validator (Modbus/DNP3)
│   │   └── 21-side-channel.md                 # 21_side_channel: Hardware power & EM emission anomaly analyzer
│   └── forensics-advanced/                    # Forensics, Traffic Control & Deception
│       ├── 13-ddos.md                         # 13_ddos: Line-rate flood shaper & SYN cookie guard
│       ├── 19-swg.md                          # 19_swg: Sovereign outbound egress proxy & URL filtering
│       ├── 22-dfir.md                         # 22_dfir: Ring-buffer PCAP evidence carver with SHA-256
│       ├── 23-ai-trism.md                     # 23_ai_trism: AI safety firewall & LLM prompt injection barrier
│       ├── 25-fdp.md                          # 25_fdp: Financial transaction graph anomaly analyzer
│       └── 26-ddp.md                          # 26_ddp: Distributed deception decoy PLCs on secondary VIPs
│
├── plugins-30/                                # Complete Guides for the 30 Protocol Dissectors
│   ├── plugin-architecture.md                 # Dynamic loading, ABI versioning, and zero-allocation parsing
│   ├── industrial-ot/                         # Industrial Control & Manufacturing Plugins
│   │   ├── modbus-tcp.md                      # libmodbus_dissector: APDU decoding & coil override checks
│   │   ├── dnp3-substation.md                 # libdnp3_dissector: Class 0/1/2/3 polls & outstation protection
│   │   ├── siemens-s7comm.md                  # libs7comm_dissector: TPKT/COTP parsing & block memory trap
│   │   ├── profinet-rt.md                     # libprofinet_dissector: Real-time factory automation loop guard
│   │   ├── ethernet-ip-cip.md                 # libethernet_ip: Common Industrial Protocol (CIP) verification
│   │   ├── hart-ip.md                         # libhart_ip: WirelessHART & refinery instrument validation
│   │   ├── mitsubishi-melsec.md               # libmitsubishi_melsec: Semiconductor fabrication protocol guard
│   │   └── omron-fins.md                      # libomron_fins: Packaging & conveyor network parser
│   ├── energy-utilities/                      # Power Grid & Smart Building Plugins
│   │   ├── iec-60870-5-104.md                 # libiec104_dissector: High-voltage grid telecontrol APDU guard
│   │   ├── iec-61850-goose.md                 # libiec61850_goose: Substation protection relay multicast guard
│   │   ├── iec-61850-mms.md                   # libiec61850_mms: Client-server SCADA telecontrol parser
│   │   ├── opc-ua-binary.md                   # libopc_ua_dissector: Industry 4.0 binary communication filter
│   │   ├── bacnet-ip.md                       # libbacnet_building: Commercial HVAC & facility automation guard
│   │   ├── modbus-rtu-serial.md               # libmodbus_rtu_serial: RS-485 legacy serial fieldbus inspector
│   │   ├── enip-cip.md                        # libenip_cip: Industrial robotics & assembly line protocol guard
│   │   └── foundation-fieldbus.md             # libfieldbus_h1: Chemical & process instrumentation filter
│   ├── aviation-maritime-defense/             # Transport & Sovereign Defense Plugins
│   │   ├── mavlink-uav.md                     # libmavlink_uav: Autonomous drone telemetry & GPS spoofing guard
│   │   ├── ais-maritime.md                    # libais_maritime: Commercial shipping transponder collision guard
│   │   ├── nmea-gps.md                        # libnmea_gps: Navigation sensor sentence integrity verifier
│   │   ├── ads-b-avionics.md                  # libadsb_avionics: Air traffic surveillance broadcast validator
│   │   ├── stanag-4586.md                     # libstanag_4586: Military UAV interoperable datalink parser
│   │   ├── mil-std-1553.md                    # libmil_std_1553: Avionics dual-redundant multiplex data bus
│   │   └── canbus-automotive.md               # libcanbus_automotive: CAN 2.0B / CAN-FD vehicle ECU frame guard
│   └── healthcare-and-siem/                   # Medical Diagnostic & SIEM Egress Forwarders
│       ├── dicom-pacs.md                      # libdicom_pacs: 16-bit radiology imaging & C-STORE payload guard
│       ├── hl7-v2.md                          # libhl7_v2: Clinical patient diagnostic message structure verifier
│       ├── cef-forwarder.md                   # libcef_forwarder: Common Event Format SIEM stream exporter
│       ├── leef-forwarder.md                  # libleef_forwarder: IBM QRadar Log Event Extended Format exporter
│       ├── syslog-rfc5424.md                  # libsyslog_rfc5424: Structured cryptographic syslog forwarder
│       ├── kafka-producer.md                  # libkafka_producer: Zero-copy high-throughput enterprise streaming
│       ├── snmp-v3-trap.md                    # libsnmp_v3_trap: Encrypted SNMP operational alert forwarder
│       └── netflow-v9-ipfix.md                # libnetflow_v9_ipfix: Line-rate NetFlow telemetry export engine
│
├── nexus-uplink/                              # Fleet Orchestration Client Subsystem
│   ├── uplink-architecture.md                 # Background gRPC agent architecture (src/nexus/NexusUplink.cpp)
│   ├── hardware-enrollment-flow.md            # Hardware identity probe & registration handshake
│   ├── telemetry-and-heartbeats.md            # Ingesting CPU, RAM, NPU temp, drops, and sensor inventories
│   ├── collective-defense-sync.md             # Receiving FleetDefenseRule and injecting eBPF maps in < 50ms
│   ├── kernel-drop-injector.md                # Direct kernel BPF syscall mapping (KernelDropInjector.cpp)
│   ├── ota-model-updates.md                   # Polling ModelOtaService, SHA-256 checks, and zero-downtime reloads
│   └── instant-graceful-disconnect.md         # 0ms DeregistrationRequest dispatch upon SIGINT/Ctrl+C
│
├── web-command-center/                        # Local Web Management (Port 8443)
│   ├── web-console-overview.md                # Standalone air-gapped embedded SPA architecture
│   ├── zero-cdn-guarantee.md                  # Proof of zero external scripts, fonts, or network leakage
│   ├── active-telemetry-monitoring.md         # Live hardware load, NPU temperatures, and packet counters
│   ├── ebpf-drop-table-management.md          # Inspecting blocked_ip_map with 1-click manual unblock
│   ├── xai-audit-proof-viewer.md              # Real-time inspection of top-3 physical feature deviations
│   └── compliance-report-viewer.md            # On-appliance CMMC Level 2 and IEC 62443 audit scorecard exports
│
├── licensing-and-entitlements/                # Cryptographic Air-Gapped Licensing
│   ├── licensing-architecture.md              # Asymmetric cryptographic license verification model
│   ├── license-manager-engine.md              # In-process validation via LicenseManager.hpp
│   ├── community-vs-enterprise.md             # Module entitlement matrix (5 Free vs. 26 Enterprise OT)
│   ├── generating-hardware-tokens.md          # Using sentinel --generate-hardware-token for offline licensing
│   ├── applying-license-envelopes.md          # Installing license.lic to /etc/sentinel/ without phoning home
│   └── hardware-pcr-silicon-locking.md        # Binding licenses to physical TPM 2.0 PCR 0 measurements
│
├── configuration-reference/                   # Declarative YAML & Runtime Tuning
│   ├── sentinel-yaml-specification.md         # Exhaustive specification of all configuration keys
│   ├── module-tuning-parameters.md            # Per-module thresholds, timeouts, and state matrix sizing
│   ├── scada-safety-thresholds.md             # Defining allowed Modbus coils, registers, and velocity limits
│   ├── environment-variables.md               # NEXUS_HOST, NEXUS_PORT, NODE_IDENTIFIER, and XDP_MODE
│   └── systemd-service-tuning.md              # Real-time priorities (SCHED_RR), nice levels, and capabilities
│
├── tutorials/                                 # Practical Step-by-Step Deployment Guides
│   ├── deploying-shadow-mode-audit.md         # Setting up non-intrusive SPAN monitoring in 15 minutes
│   ├── protecting-modbus-substation-plc.md    # Inline physical protection for Siemens & Schneider controllers
│   ├── securing-hospital-pacs-enclave.md      # Mitigating DICOM exfiltration in radiology diagnostic suites
│   ├── container-breakout-prevention.md       # Stopping Docker & Kubernetes container escapes with CWPP
│   └── deploying-synthetic-decoy-plcs.md      # Configuring honeypot secondary VIPs without port conflicts
│
├── compliance/                                # Regulatory Alignment & Verification
│   ├── iec-62443-industrial-audit.md          # Proving FR 3 (System Integrity) & FR 5 (Zone Segmentation)
│   ├── cmmc-2.0-level-2-audit.md              # Fulfilling NIST SP 800-171 Control SI.L2-3.14.1 (< 1ms drop proof)
│   ├── eu-nis2-directive.md                   # Satisfying Article 21 incident response mandates ($0 cloud egress)
│   └── tamper-evident-evidence-logging.md     # Legally admissible PCAP carving and cryptographic signing
│
└── troubleshooting/                           # Help Desk & Diagnostics
    ├── startup-and-config-errors.md           # Fixing YAML parse failures, missing keys, and invalid paths
    ├── ebpf-driver-attachment-issues.md       # Resolving interface binding errors on ens33, eth0, and vmxnet3
    ├── plugin-loading-failures.md             # Debugging dlopen failures, missing .so files, and ABI mismatches
    ├── nexus-uplink-disconnections.md         # Diagnosing gRPC timeouts, mTLS handshake errors, and DNS
    ├── faq.md                                 # Technical Frequently Asked Questions
    └── support.md                             # Issue tracking, emergency support SLAs, and vulnerability reporting
```


# Project 7 of 8: `sentinel-matrix` (`sentinel-matrix`)
## Complete Documentation & Help Desk System File Structure (`docs/`)

This is the complete, full-version documentation tree for **`sentinel-matrix`**. It is formatted for enterprise documentation platforms (**MkDocs Material**, **Docusaurus**, or **Starlight**) and covers VMware hypervisor encapsulation, the collision-free **`10.240.0.0/24`** subnet, the **OmniFlow 7-channel multi-modal traffic engine**, **authentic malware PCAP replays (Industroyer, Triton, S7Comm)**, the **live Red-Team adversary node (`10.240.0.99`)**, and the **real-time terminal TUI dashboard**.

---

```text
sentinel-matrix/docs/
├── mkdocs.yml                                 # Documentation site configuration (navigation, theme, search)
├── index.md                                   # Documentation home & cyber-range digital twin overview
│
├── getting-started/                           # Onboarding & Setup
│   ├── overview.md                            # Autonomous multi-tier cyber-physical range introduction
│   ├── system-requirements.md                 # VMware Workstation/ESXi, Docker, Compose v2, CPU/RAM sizing
│   ├── quickstart-one-command-launch.md       # 3-minute launch: make init -> make build -> make up
│   ├── verifying-container-grid.md            # Checking 7+ container health, IPs, and port bindings
│   └── architecture-at-a-glance.md            # High-level diagram: Mesh Topology, Subnets, and Data Paths
│
├── architecture/                              # Deep Systems Design
│   ├── simulation-mesh-architecture.md        # Multi-container digital twin architecture and lifecycle
│   ├── the-infinite-flywheel.md               # Continuous loop: Ambient -> Exploit -> Drop -> Retrain -> Hot-Reload
│   ├── container-topology-matrix.md           # Master service inventory: Nexus, Forge, Nodes, Traffic, Adversary
│   ├── shared-volumes-and-ipc.md              # Shared storage architecture: /shared/models, datasets, and logs
│   └── vmware-hypervisor-optimization.md      # Solving virtualization constraints inside VMware Linux guests
│
├── vmware-and-networking/                     # Hypervisor & Network Engineering
│   ├── 10-240-0-subnet-design.md              # Avoiding 172.x Docker and VMnet1/VMnet8 route collisions
│   ├── xdp-generic-skb-mode.md                # Running eBPF/XDP over virtual veth interfaces and virtual NICs
│   ├── vtpm-and-dmi-emulation.md              # Emulating physical TPM 2.0 PCRs and VMware-DMI product UUIDs
│   ├── glibc-alignment-ubuntu-devel.md        # Aligning host Ubuntu 26.04 (GLIBC 2.43) with container images
│   ├── host-dynamic-library-bundling.md       # Harvesting libabsl, libre2, and libgrpc into /shared/lib/
│   └── internal-mtls-pki.md                   # Generating container Root CA and internal certificates
│
├── omniflow-traffic-engine/                   # 7-Channel Multi-Modal Generation Core
│   ├── omniflow-architecture.md               # Concurrent multi-threaded engine design (omniflow_engine.py)
│   ├── channel-1-scada-ot.md                  # Modbus TCP (FC03/FC05) & DNP3 actuator override stream
│   ├── channel-2-edge-vision.md               # 30 FPS video tensor frames & YOLO perimeter intrusion stream
│   ├── channel-3-web-api-bot.md               # L7 REST API queries, SQLi, and non-human bot kinematics
│   ├── channel-4-identity-ato.md              # Impossible geographic travel velocity & Kerberos SPN abuse
│   ├── channel-5-host-syscalls.md             # Container eBPF breakouts & ransomware Shannon entropy (7.95 bits)
│   ├── channel-6-medical-iot.md               # DICOM PACS radiology images & MAVLink drone waypoint spoofing
│   ├── channel-7-netflow-blaster.md           # High-rate directional NetFlow stream with [0.40 - 0.60] uncertainty
│   └── declarative-traffic-tuning.md          # Customizing rates, targets, and distributions in omniflow.yaml
│
├── real-pcap-replay/                          # Authentic Historical Malware Pipeline
│   ├── pcap-replay-pipeline-overview.md       # Replaying genuine raw byte captures vs. synthetic mocking
│   ├── pcap-streamer-engine.md                # Binary parser and wire injection engine (src/traffic/pcap_streamer.py)
│   ├── git-lfs-downloader.md                  # Automated GitHub LFS pointer resolution (tools/download_real_pcaps.py)
│   ├── offline-binary-generator.md            # Air-gapped self-contained generator (tools/generate_real_pcaps.py)
│   ├── pcap-catalog-industroyer-iec104.md     # IEC 60870-5-104 power grid circuit breaker trip replay (T0855)
│   ├── pcap-catalog-triton-tristation.md      # Schneider Electric Triconex TriStation safety override replay (T0843)
│   ├── pcap-catalog-stuxnet-s7comm.md         # Siemens S7Comm PLC centrifuge frequency tamper replay (T0831)
│   ├── pcap-catalog-modbus-scada.md           # University of Illinois genuine Modbus TCP SCADA replay (T0855)
│   └── rate-pacing-and-wire-injection.md      # Rate-limiting replay pace (packets/sec) and flow dissection
│
├── live-adversary-node/                       # Active Wire Red-Team Container (10.240.0.99)
│   ├── adversary-node-architecture.md         # Containerized Red-Team workstation design & network routing
│   ├── live-adversary-daemon.md               # Autonomous testing loop (src/traffic/live_adversary_daemon.py)
│   ├── nmap-tcp-syn-sweeps.md                 # Executing live nmap -sS port discovery sweeps on the wire (T1046)
│   ├── mbpoll-scada-overrides.md              # Executing live Modbus FC05 coil overrides via mbpoll (T0855)
│   ├── curl-api-abuse-bursts.md               # Executing high-velocity HTTP/REST endpoint fuzzing (T1190)
│   ├── observing-wire-ebpf-drops.md           # Real-time feedback: Watching nmap ports transition from open to filtered
│   └── custom-adversary-tooling.md            # Adding custom tools (hping3, scapy, hydra) to Dockerfile.adversary
│
├── observability-and-tui/                     # Real-Time Monitoring & Consoles
│   ├── observability-overview.md              # Dual-console monitoring: Terminal TUI vs. Web Command Center
│   ├── terminal-dashboard-tui.md              # High-density curses/rich dashboard architecture (live_dashboard.py)
│   ├── split-panel-layout.md                  # Screen design: Connected Appliances + MITRE Heatmap + XAI Panel
│   ├── real-time-xai-panel.md                 # Live inspection of top-3 physical deviations and audit notes
│   ├── web-command-center-integration.md      # Connecting browser to localhost:9443 (HTML5 Canvas Topology)
│   ├── streaming-sse-events.md                # Server-Sent Events multiplexing on port 9444
│   └── metrics-aggregation.md                 # Calculating live microsecond latency percentiles (p50, p95, p99)
│
├── closed-loop-active-learning/               # Flywheel Integration (xInfer-Forge & Nexus)
│   ├── flywheel-lifecycle.md                  # Ingestion -> Curate CSV -> MAE Retrain -> Canary OTA -> Hot-Reload
│   ├── forge-watcher-daemon.md                # Automated dataset detection in /shared/datasets/ (forge_watcher.py)
│   ├── staged-rollout-progression.md          # Validating model evolution: SHADOW_MODE -> CANARY_5_PCT -> FLEET_WIDE
│   ├── zero-downtime-hot-reload-validation.md # Proving edge nodes hot-reload new weights without dropping packets
│   └── continuous-drift-adaptation.md         # Maintaining > 98% accuracy under changing simulated site patterns
│
├── chaos-and-resilience/                      # Fault-Tolerance & Chaos Engineering
│   ├── chaos-engineering-overview.md          # Testing edge resilience and automated safety circuits under stress
│   ├── latency-spike-injection.md             # Injecting > 1000µs SLA latency breach via make chaos-latency
│   ├── automated-rollback-verification.md     # Proving Nexus RollbackGuard aborts candidate models in milliseconds
│   ├── node-sever-testing.md                  # Killing edge containers via make chaos-sever
│   ├── instant-0ms-disconnect-validation.md   # Proving nodes turn OFFLINE in 0ms via DeregistrationRequest
│   └── automated-recovery-testing.md          # Validating seamless appliance re-registration after network recovery
│
├── operations-and-makefile/                   # Command Reference & Execution Guide
│   ├── makefile-reference.md                  # Complete categorized command cheat sheet (make help)
│   ├── common-workflows.md                    # Daily operations: Launching, attacking, inspecting, and tearing down
│   ├── configuring-matrix-yaml.md             # Customizing timelines, tick rates, and thresholds in matrix.yaml
│   ├── configuring-node-templates.md          # Scaling from 3 to 20 nodes using custom node YAML profiles
│   ├── configuring-scenario-profiles.md       # Authoring new attack scenario YAMLs in configs/scenarios/
│   └── log-inspection-and-debugging.md        # Accessing centralized logs in /shared/logs/ (nexus, nodes, forge)
│
├── tutorials/                                 # Hands-on Simulation Walkthroughs
│   ├── full-closed-loop-walkthrough.md        # End-to-end demonstration: Attack -> Mitigation -> Retrain -> Hot-Reload
│   ├── simulating-substation-blackout-attack.md# Replaying Industroyer against simulated electrical protection relays
│   ├── simulating-hospital-ransomware-wave.md # Injecting DICOM exfiltration and high-entropy encryption bursts
│   ├── adding-custom-malware-pcap.md          # Importing your own Wireshark capture into the replay streamer
│   └── running-matrix-in-esxi-headless.md     # Headless deployment on enterprise VMware ESXi clusters
│
└── troubleshooting/                           # Help Desk & Diagnostics
    ├── docker-subnet-pool-overlaps.md         # Resolving "Pool overlaps with other one" via 10.240.0.0/24
    ├── glibc-version-not-found-errors.md      # Resolving GLIBC_2.43 not found via FROM ubuntu:devel
    ├── missing-host-libraries-absl-re2.md     # Resolving missing dynamic libraries via make init & shared/lib/
    ├── container-restarting-loops.md          # Debugging Python import paths and entrypoint script execution
    ├── tui-empty-appliances-debugging.md      # Fixing edge appliance registration and NEXUS_HOST routing
    ├── pcap-lfs-pointer-corruption.md         # Detecting and resolving 130-byte Git LFS text pointer files
    ├── faq.md                                 # Technical Frequently Asked Questions
    └── support.md                             # Issue tracking, community channels, and enterprise support SLAs
```

---

*This concludes the complete documentation system structure for **Project 7: `sentinel-matrix`**.*  
*Ready to proceed to the final project, **Project 8: `sentinel-stack` (`sentinel-stack` unified 6-tier meta-installer & master deployment orchestrator)**, upon your confirmation.*
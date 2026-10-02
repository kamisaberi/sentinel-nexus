# Project 8 of 8: `sentinel-stack` (`sentinel-stack`)
## Master Landing Page & Technical Architecture Document
**Target URL:** `aryorithm.com/technology/stack`  
**Repository:** `https://github.com/kamisaberi/sentinel-stack`  
**Artifact:** `install.sh` & Master Meta-Orchestrator (Unified 6-Tier Build & Deployment Engine)

---

```text
========================================================================================================
                                     PAGE STRUCTURE OUTLINE
========================================================================================================
 1. Hero Section (Headline, Value Proposition, Real-Time Automation Metric Strip)
 2. The Deep-Tech Deployment Challenge: Topological Compilation at Scale
 3. The 6-Phase Automated Installation Lifecycle (Phase 0 to Phase 5)
 4. Automated Dependency Resolution Engine (Ubuntu 24.04 / 26.04 Packaging)
 5. Strict Topological Compilation DAG (xinfer -> blackbox -> sentinel -> forge -> lab -> nexus)
 6. Systemd Production Daemonization & Real-Time Linux Capability Grants
 7. Automated Smoke-Testing & Verification Suite (05_verify_installation.sh)
 8. Bridge to Sentinel-Matrix (Automated Container & Library Bundling)
 9. Developer Quickstart & Master Makefile Command Reference
 10. Technical Frequently Asked Questions (FAQ)
 11. Conversion Call-To-Action (CTA) & Master Ecosystem Wrap-Up
========================================================================================================
```

---

## 1. Hero Section

### Badge
`TIER 8 MASTER ORCHESTRATOR` `1-CLICK ZERO-PROMPT INSTALL` `TOPOLOGICAL DAG COMPILER` `UBUNTU 24.04 / 26.04`

### Headline
# Deploy the Complete 6-Tier Active Defense Ecosystem in a Single Command.

### Subheadline
**`sentinel-stack`** is the master build automation, dependency resolver, and deployment orchestration platform for the **Aryorithm / Blackbox Sentinel** ecosystem. In a single zero-prompt execution, it resolves OS toolchains, manages modern Python virtual environments (PEP 668), compiles all six native C++20 and eBPF/XDP tiers in strict topological dependency order, installs libraries system-wide, and deploys production systemd daemons with automated health verification.

### Primary CTA Group
* `[ View on GitHub ]` $\rightarrow$ `https://github.com/kamisaberi/sentinel-stack`
* `[ Explore 6-Phase Pipeline ]` $\rightarrow$ `#lifecycle-phases`
* `[ Run 1-Click Install Guide ]` $\rightarrow$ `#quickstart-runbook`

### Live KPI Strip (Metrics Display Grid)
```text
+---------------------+---------------------+---------------------+---------------------+
|       6 Tiers       |       1-Click       |       100%          |        0 ms         |
| Sequential Build in | Zero-Prompt Master  | Automated Post-Build| Manual Dependency   |
| Strict Dependency   | Installation Script | Verification Suite  | Configuration Needed|
+---------------------+---------------------+---------------------+---------------------+
```

---

## 2. The Deep-Tech Deployment Challenge: Topological Compilation at Scale

```text
  THE FRAGMENTED MANUAL DEPLOYMENT NIGHTMARE
  ┌──────────────────────────────────────────────────────────────────────────────────┐
  │ 1. Developer clones 6 disparate repositories into different paths.               │
  │ 2. Compiling Tier 2 fails because Tier 1 (libxinfer.so) wasn't installed yet.    │
  │ 3. Compiling Tier 3 fails because eBPF xdp_filter.o bytecode wasn't built first. │
  │ 4. Python pip install breaks because Ubuntu 24.04/26.04 enforces PEP 668.        │
  │ 5. Dynamic linker throws "shared library not found" because ldconfig was missed. │
  │ 6. Systemd services crash because CAP_NET_ADMIN / CAP_BPF were not granted.      │
  │ [ RESULT: 4+ hours of frustrating dependency debugging and broken builds ]       │
  └──────────────────────────────────────────────────────────────────────────────────┘
                                          VS.
  THE SENTINEL-STACK DETERMINISTIC META-INSTALLER
  ┌──────────────────────────────────────────────────────────────────────────────────┐
  │ sudo ./install.sh                                                                │
  │ ──> Probes virtualization, CPU, RAM, and mounts /sys/fs/bpf                      │
  │ ──> Installs Clang, LLVM, CMake, gRPC, Protobuf, and isolated Python venv        │
  │ ──> Executes Topological Compilation DAG (xinfer -> blackbox -> sentinel...)     │
  │ ──> Updates dynamic linker caches & installs binaries to /usr/local/             │
  │ ──> Deploys hardened systemd units with RT priorities & capabilities             │
  │ ──> Runs 6/6 automated smoke tests confirming production readiness               │
  │ [ ZERO PROMPTS • DETERMINISTIC SUCCESS • REPRODUCIBLE IN MINUTES ]               │
  └──────────────────────────────────────────────────────────────────────────────────┘
```

Deploying a modern deep-tech cybersecurity platform involves heterogeneous layers of the Linux operating system:
* Native C++20 shared libraries with strict ABI boundaries.
* Kernel-space eBPF bytecode compiled directly via Clang/LLVM.
* Multi-threaded gRPC services with compiled Protobuf stubs.
* Python machine learning daemons running PyTorch and ONNX.
* Pinned BPF virtual filesystems and Linux real-time scheduling capabilities.

Attempting to install these components manually inevitably leads to circular dependency deadlocks and linker mismatches. `sentinel-stack` eliminates manual human error by encoding the **entire system topology into a deterministic, automated execution graph**.

---

## 3. The 6-Phase Automated Installation Lifecycle

When executing `sudo ./install.sh`, the orchestrator executes a linear, 6-phase pipeline:

```text
========================================================================================================
                          THE 6-PHASE AUTOMATED INSTALLATION PIPELINE
========================================================================================================

  [ PHASE 0: SYSTEM VALIDATION (00_check_system.sh) ]
  • Verifies Ubuntu 24.04 / 26.04 LTS & x86_64 / aarch64 architecture.
  • Probes available CPU cores & RAM (dynamically scales compilation threads).
  • Validates virtualization (VMware, KVM, Bare-metal).
  • Mounts Linux BPF virtual filesystem at /sys/fs/bpf.
                 │
                 ▼
  [ PHASE 1: TOOLCHAINS & DEPENDENCY RESOLUTION (01_install_dependencies.sh) ]
  • Installs Clang, LLVM, CMake >= 3.20, build-essential, libelf-dev, libssl-dev.
  • Installs gRPC & Protobuf development packages (libgrpc++-dev, libprotobuf-dev).
  • Configures isolated Python virtual environment at /opt/sentinel-stack/venv (PEP 668).
  • Pre-installs NumPy, PyYAML, PyTorch CPU, ONNX, and Rich.
                 │
                 ▼
  [ PHASE 2: REPOSITORY SYNCHRONIZATION (02_clone_repositories.sh) ]
  • Checks for existing local workspace repositories in /home/kami/.
  • Synchronizes source trees using rsync (excluding stale build folders).
  • Clones missing repositories from GitHub via shallow depth-1 clones.
                 │
                 ▼
  [ PHASE 3: TOPOLOGICAL COMPILATION & INSTALLATION (03_build_all_tiers.sh) ]
  • Compiles and installs Tiers 1 through 6 in strict dependency sequence.
  • Updates dynamic linker cache (ldconfig) after each library installation.
  • Deploys web assets, configuration files, and CLI wrapper binaries.
                 │
                 ▼
  [ PHASE 4: SYSTEMD SERVICE DAEMONIZATION (04_setup_systemd.sh) ]
  • Generates and deploys sentinel-nexus.service and blackbox-sentinel.service.
  • Configures real-time kernel capabilities: CAP_NET_ADMIN, CAP_SYS_ADMIN, CAP_BPF.
  • Enables services to auto-start on system boot.
                 │
                 ▼
  [ PHASE 5: SMOKE TESTING & VERIFICATION (05_verify_installation.sh) ]
  • Executes automated post-build assertions across all 6 tiers.
  • Validates shared object sonames, binary execution, and CLI responsiveness.
  • Confirms complete operational readiness.
========================================================================================================
```

---

## 4. Automated Dependency Resolution Engine

Modern Linux distributions (such as Ubuntu 24.04 and 26.04) introduce strict packaging rules that break traditional installers:
1. **64-bit `time_t` Package Transition (`t64`):**  
   Ubuntu 24.04+ renamed core runtime packages (e.g., `libprotobuf32t64`, `libgrpc++1.51t64`). `sentinel-stack` uses canonical development metapackages (`libprotobuf-dev`, `libgrpc++-dev`), ensuring version-agnostic compatibility across both LTS and rolling development releases.
2. **PEP 668 Managed Python Environments:**  
   Running `pip install` globally on modern Ubuntu triggers `error: externally-managed-environment`. `sentinel-stack` creates an isolated, managed virtual environment at:
   ```text
   /opt/sentinel-stack/venv/
   ```
   All machine learning dependencies (PyTorch, ONNX, PyYAML) are installed inside this managed boundary. CLI tools like `forge-cli` are wrapped in clean shell entrypoints placed in `/usr/local/bin/`, providing global terminal access without polluting system Python packages.
3. **Non-Interactive Debian Frontend:**  
   Forces `DEBIAN_FRONTEND=noninteractive` and applies `-o Dpkg::Options::="--force-confdef"`, ensuring that the installer never hangs waiting for user input during kernel header updates.

---

## 5. Strict Topological Compilation DAG

The ecosystem cannot be compiled in arbitrary order. Each tier provides dynamic symbols and headers consumed by the tier above it:

```text
========================================================================================================
                        TOPOLOGICAL COMPILATION DIRECTED ACYCLIC GRAPH (DAG)
========================================================================================================

  [ STEP 1: TIER 1 - xinfer-essential ]
  • Compiles libxinfer.so (C++20 heterogeneous AI runtime).
  • Installs: /usr/local/lib/libxinfer.so & /usr/local/include/xinfer/
  • Action: Runs ldconfig.
                 │
                 ▼ (libxinfer.so linked by Tier 2)
  [ STEP 2: TIER 2 - blackbox-essential ]
  • Builds eBPF bytecode: clang -O2 -target bpf -> xdp_filter.o.
  • Compiles libblackbox.so (EventRingBuffer, HardwareIdentity, BPF maps).
  • Installs: /usr/local/lib/libblackbox.so & /usr/local/include/blackbox/
  • Action: Runs ldconfig.
                 │
                 ▼ (libxinfer.so + libblackbox.so linked by Tier 3)
  [ STEP 3: TIER 3 - blackbox-sentinel ]
  • Synchronizes protobuf definitions from Nexus.
  • Compiles sentinel edge daemon (26 subsystems, 30 plugins, NexusUplink).
  • Installs: /usr/local/bin/sentinel & /etc/sentinel/sentinel.yaml.
                 │
                 ▼ (Isolated Python Runtime)
  [ STEP 4: TIER 4 - xinfer-forge ]
  • Configures PyTorch CPU continual active learning pipeline.
  • Installs: /usr/local/bin/forge-cli global wrapper.
                 │
                 ▼ (libxinfer.so + libblackbox.so linked by Tier 5)
  [ STEP 5: TIER 5 - sentinel-lab ]
  • Compiles eBPF driver hooks and native C++20 research harness.
  • Installs: /usr/local/bin/sentinel_lab.
                 │
                 ▼ (Independent Orchestrator)
  [ STEP 6: TIER 6 - sentinel-nexus ]
  • Compiles central command plane, gRPC services, and embedded HTTP server.
  • Installs: /usr/local/bin/sentinel-nexus & /usr/local/bin/nexus-ctl.
  • Deploys: Web Command Center SPA to /opt/sentinel-nexus/web/.
========================================================================================================
```

---

## 6. Systemd Production Daemonization

`sentinel-stack` does not leave applications running in loose terminal windows. It deploys production Linux systemd service units engineered with **real-time priorities and hardened security boundaries**:

### Blackbox Sentinel Service (`blackbox-sentinel.service`)
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

# High-Performance Limits & Process Scheduling
LimitNOFILE=65536
TasksMax=4096
AmbientCapabilities=CAP_NET_ADMIN CAP_SYS_ADMIN CAP_BPF
Nice=-10
CPUSchedulingPolicy=rr
CPUSchedulingPriority=80

[Install]
WantedBy=multi-user.target
```

### Sentinel Nexus Service (`sentinel-nexus.service`)
```ini
[Unit]
Description=Sentinel Nexus - Collective Fleet Command Plane (Tier 6)
After=network.target network-online.target
Wants=network-online.target

[Service]
Type=simple
User=root
WorkingDirectory=/opt/sentinel-nexus
ExecStart=/usr/local/bin/sentinel-nexus /opt/sentinel-nexus/configs/nexus.yaml
Restart=always
RestartSec=5s
LimitNOFILE=65536
TasksMax=4096

[Install]
WantedBy=multi-user.target
```

---

## 7. Automated Smoke-Testing & Verification Suite

Upon compilation, `sentinel-stack` executes **`scripts/05_verify_installation.sh`**, running strict assertions against the operating system:

```text
================================================================================
  SENTINEL-STACK: AUTOMATED POST-INSTALLATION HEALTH VERIFICATION
================================================================================
[*] Testing Tier 1: libxinfer.so in dynamic linker cache...           [PASS]
[*] Testing Tier 2: libblackbox.so in dynamic linker cache...         [PASS]
[*] Testing Tier 3: sentinel daemon binary execution...               [PASS]
[*] Testing Tier 4: forge-cli environment & CLI responsiveness...     [PASS]
[*] Testing Tier 5: sentinel_lab testbed binary execution...          [PASS]
[*] Testing Tier 6: sentinel-nexus & nexus-ctl CLI responsiveness...  [PASS]
--------------------------------------------------------------------------------
[+] Verification Summary: 6 of 6 Tiers Operating Nominally.
[+] System is fully compiled, linked, and ready for production operations.
================================================================================
```

---

## 8. Bridge to Sentinel-Matrix (Cyber-Range Handover)

`sentinel-stack` also serves as the build pipeline for **`sentinel-matrix`** (the containerized cyber range inside VMware):
1. When `sentinel-stack` compiles the native binaries on the host, they are verified against system shared objects.
2. Running `make init` inside `sentinel-matrix` immediately harvests the freshly compiled `sentinel-nexus`, `sentinel`, and `nexus-ctl` binaries from system paths into `sentinel-matrix/shared/bin/`.
3. It bundles all dynamic host libraries (`libabsl_*`, `libre2.so.11`, `libgrpc++`) into `sentinel-matrix/shared/lib/`, ensuring that the Docker containers start without missing `.so` dependencies.

---

## 9. Developer Quickstart & Master Makefile Command Reference

### Master 1-Click Install
On any fresh Ubuntu 24.04 or 26.04 machine:
```bash
git clone https://github.com/kamisaberi/sentinel-stack.git
cd sentinel-stack
chmod +x install.sh scripts/*.sh
sudo ./install.sh
```

### Operations Makefile Reference
```bash
# ------------------------------------------------------------------------------
# MASTER MAKEFILE TARGETS
# ------------------------------------------------------------------------------
sudo make install    # Executes complete 6-tier installation and compilation
sudo make verify     # Runs post-installation smoke tests across all 6 tiers
sudo make update     # Pulls latest git commits and recompiles the entire stack
sudo make status     # Displays status of Sentinel and Nexus systemd services
sudo make clean      # Purges all build directories, caches, and object files
```

---

## 10. Technical Frequently Asked Questions (FAQ)

#### Q: Can `sentinel-stack` run completely offline in an air-gapped facility?
**A:** Yes. If the Ubuntu machine is inside a classified or air-gapped network, pre-seed the `/home/kami/` directory with the repository folders and point your APT sources to an internal local mirror. `scripts/02_clone_repositories.sh` detects existing local source trees and synchronizes them with zero outbound internet requests.

#### Q: What happens if a compilation step runs out of RAM?
**A:** In Phase 0 (`00_check_system.sh`), the installer assesses total system RAM. If total memory is under 4 GB (common on constrained virtual machines), the compiler dynamically caps parallel build jobs (`make -j2`) to prevent the Linux kernel OOM-killer from terminating the compiler.

#### Q: How do I cleanly uninstall or remove the installed software?
**A:** Stop and disable the systemd services:
```bash
sudo systemctl disable --now sentinel-nexus blackbox-sentinel
```
Remove the installed binaries and libraries:
```bash
sudo rm -f /usr/local/bin/{sentinel,sentinel-nexus,nexus-ctl,sentinel_lab,forge-cli}
sudo rm -f /usr/local/lib/{libxinfer.so*,libblackbox.so*}
sudo rm -rf /etc/sentinel /opt/sentinel-nexus /opt/sentinel-stack
sudo ldconfig
```

#### Q: How does `sentinel-stack` handle future Linux kernel updates?
**A:** When the host Linux kernel updates, re-run:
```bash
sudo make update
```
The installer automatically recompiles the eBPF kernel filter (`xdp_filter.o`) against the newly booted kernel headers, updates the shared libraries, and restarts the systemd services seamlessly.

---

## 11. Conversion Call-To-Action (CTA)

```text
+------------------------------------------------------------------------------------------------------+
|                     DEPLOY THE COMPLETE ACTIVE DEFENSE STACK IN SECONDS                              |
|                                                                                                      |
|   Eliminate dependency deadlocks and manual compilation errors. Build, link, and deploy              |
|   all six tiers of the Blackbox Sentinel ecosystem with verified, deterministic automation.          |
|                                                                                                      |
|   [ Clone sentinel-stack on GitHub ]    [ Read Master Documentation ]         [ Contact Systems Team ]|
|   github.com/kamisaberi/sentinel-stack  aryorithm.com/technology/stack        research@aryorithm.com  |
+------------------------------------------------------------------------------------------------------+
```

---

### Master Documentation Series Complete (8 of 8 Projects)

1. **`xinfer-essential` (`libxinfer.so` - Tier 1):** Universal Heterogeneous AI Runtime (15 Silicon Backends).
2. **`blackbox-essential` (`libblackbox.so` - Tier 2):** In-Kernel eBPF/XDP Mitigation Core & TPM 2.0 Attestation.
3. **`blackbox-sentinel` (`sentinel` - Tier 3):** Commercial Cyber-Physical Edge XDR/SIEM Appliance.
4. **`xinfer-forge` (`forge-cli` - Tier 4):** Continual Active Learning Daemon & Safety Regression Gate.
5. **`sentinel-lab` (`sentinel_lab` - Tier 5):** Academic Research Testbed, SLAB Wire Protocol & LaTeX Preprint.
6. **`sentinel-nexus` (`sentinel-nexus` - Tier 6):** Central Fleet Command Plane & Collective Defense Grid.
7. **`sentinel-matrix` (`sentinel-matrix` - Tier 7):** Autonomous Cyber-Range, Multi-Modal OmniFlow & Digital Twin.
8. **`sentinel-stack` (`sentinel-stack` - Master Installer):** Unified 6-Tier Topological Meta-Installer & Orchestrator.
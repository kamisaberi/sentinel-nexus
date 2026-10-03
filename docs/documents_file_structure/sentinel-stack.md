# Project 8 of 8: `sentinel-stack` (`sentinel-stack`)
## Complete Documentation & Help Desk System File Structure (`docs/`)

This is the complete, full-version documentation tree for **`sentinel-stack`**. It is formatted for enterprise documentation platforms (**MkDocs Material**, **Docusaurus**, or **Starlight**) and covers 1-click master installation automation, **Ubuntu 24.04/26.04 dependency resolution (PEP 668 & 64-bit `t64` packages)**, the **strict topological compilation DAG (Tiers 1 through 6)**, **hardened Linux systemd daemonization**, and the **automated binary/library bridge to `sentinel-matrix`**.

---

```text
sentinel-stack/docs/
├── mkdocs.yml                                 # Documentation site configuration (navigation, theme, search)
├── index.md                                   # Documentation home & master orchestration overview
│
├── getting-started/                           # 1-Click Onboarding & Quickstart
│   ├── overview.md                            # Meta-builder architecture: Automating the 6-tier ecosystem
│   ├── system-requirements.md                 # Ubuntu 24.04/26.04 LTS, x86_64/aarch64, RAM sizing & sudo access
│   ├── one-click-quickstart.md                # 1-Command execution: sudo ./install.sh
│   ├── verifying-installed-stack.md           # Running post-installation smoke tests (sudo make verify)
│   ├── architecture-at-a-glance.md            # High-level diagram: 6 Tiers -> Shared Libraries -> Systemd
│   └── post-install-next-steps.md             # Launching web UI (:9443), testing CLI, and exploring matrix
│
├── architecture/                              # Systems Engineering & DAG Design
│   ├── meta-installer-architecture.md         # Decoupled build automation & lifecycle engine design
│   ├── topological-compilation-dag.md         # Mathematical dependency DAG: xinfer -> blackbox -> sentinel...
│   ├── dependency-graph-formalization.md      # Symbol resolution: Why Tier N must precede Tier N+1
│   ├── system-layout-and-paths.md             # Filesystem layout: /usr/local/lib, /usr/local/bin, /etc/sentinel
│   ├── air-gapped-installation-model.md       # Pre-seeding source trees and local APT mirrors offline
│   └── atomic-rollback-safeguards.md          # State traps and failure containment during compilation
│
├── installation-phases/                       # The 6-Phase Linear Pipeline
│   ├── pipeline-overview.md                   # Chronological pipeline execution (Phase 0 through Phase 5)
│   ├── phase-0-system-validation.md           # 00_check_system.sh: Probing OS, kernel, RAM, and mounting bpffs
│   ├── phase-1-dependency-resolution.md       # 01_install_dependencies.sh: Compilers, gRPC, and Python venv
│   ├── phase-2-repository-synchronization.md  # 02_clone_repositories.sh: Local rsync sync vs. shallow git clone
│   ├── phase-3-topological-compilation.md     # 03_build_all_tiers.sh: Sequential CMake/make build execution
│   ├── phase-4-systemd-daemonization.md       # 04_setup_systemd.sh: Unit generation, capabilities, and startup
│   └── phase-5-smoke-testing.md               # 05_verify_installation.sh: Automated health check assertions
│
├── dependency-management/                     # OS & Toolchain Resolution (Ubuntu 24.04 / 26.04)
│   ├── ubuntu-packaging-engine.md             # Non-interactive APT flags and automated mirror selection
│   ├── t64-package-resolution.md              # Managing libprotobuf-dev and libgrpc++-dev 64-bit time_t packages
│   ├── pep-668-python-virtual-env.md          # Isolated environment at /opt/sentinel-stack/venv (Zero OS pollution)
│   ├── ebpf-compiler-toolchains.md            # Clang, LLVM, libelf-dev, and kernel header alignment
│   ├── openvino-tensorrt-prerequisites.md     # Detecting Intel GPU/NPU drivers and CUDA runtime toolkits
│   └── non-interactive-execution.md           # DEBIAN_FRONTEND=noninteractive and Dpkg force-confdef options
│
├── compilation-dag-tiers/                     # Tier-by-Tier Build Mechanics
│   ├── building-tier-1-xinfer.md              # Compiling libxinfer.so & installing headers to /usr/local/include
│   ├── building-tier-2-blackbox.md            # Compiling xdp_filter.o (Clang BPF) & building libblackbox.so
│   ├── building-tier-3-sentinel.md            # Linking 26 subsystems, 30 plugins & sentinel daemon binary
│   ├── building-tier-4-forge.md               # Installing PyTorch CPU in venv & creating /usr/local/bin/forge-cli
│   ├── building-tier-5-lab.md                 # Compiling sentinel_lab binary & SLAB research testbed
│   ├── building-tier-6-nexus.md               # Compiling sentinel-nexus, nexus-ctl, and deploying web SPA
│   ├── dynamic-linker-cache-ldconfig.md       # Managing /etc/ld.so.cache updates after each tier build
│   └── parallel-job-scaling-ram.md            # Dynamic compiler thread capping (make -j2 vs. nproc) under low RAM
│
├── systemd-daemonization/                     # Production Linux Service Units
│   ├── systemd-architecture.md                # Lifecycle management, auto-restart circuits, and dependencies
│   ├── sentinel-nexus-service.md              # Configuring sentinel-nexus.service (WorkingDir, ports, ulimits)
│   ├── blackbox-sentinel-service.md           # Configuring blackbox-sentinel.service (Real-time priorities)
│   ├── linux-capabilities-management.md       # Granting CAP_NET_ADMIN, CAP_SYS_ADMIN, CAP_BPF without full root
│   ├── real-time-process-scheduling.md        # Real-time round-robin scheduling (SCHED_RR, priority 80, nice -10)
│   └── daemon-logging-and-journalctl.md       # Centralized logging, journalctl filtering, and log rotation
│
├── verification-and-smoke-tests/              # Post-Build Automated Quality Gates
│   ├── verification-suite-overview.md         # 05_verify_installation.sh test matrix and exit codes
│   ├── tier-1-libxinfer-check.md              # Verifying libxinfer.so presence in dynamic linker cache
│   ├── tier-2-libblackbox-check.md            # Verifying libblackbox.so and xdp_filter.o bytecode validity
│   ├── tier-3-sentinel-check.md               # Testing sentinel daemon binary execution and version output
│   ├── tier-4-forge-cli-check.md              # Validating forge-cli wrapper, venv activation, and PyTorch
│   ├── tier-5-sentinel-lab-check.md           # Testing sentinel_lab binary execution and SLAB socket hooks
│   ├── tier-6-nexus-ctl-check.md              # Testing sentinel-nexus daemon and nexus-ctl CLI responsiveness
│   └── automated-ci-cd-integration.md         # Running sentinel-stack verification in GitHub Actions / GitLab CI
│
├── bridge-to-sentinel-matrix/                 # Cyber-Range Staging Handover
│   ├── matrix-bridge-architecture.md          # Packaging compiled host binaries for Docker containerization
│   ├── harvesting-host-binaries.md            # Copying sentinel-nexus, sentinel, and nexus-ctl to shared/bin/
│   ├── dynamic-library-extraction-ldd.md      # Auto-harvesting libabsl, libre2, and libgrpc into shared/lib/
│   ├── proto-and-web-asset-handover.md        # Synchronizing .proto files and web SPA assets into sentinel-matrix
│   └── vmware-docker-mesh-handshake.md        # One-touch handover from host compilation to `make up` mesh launch
│
├── configuration-and-customization/           # Stack Customization
│   ├── stack-yaml-specification.md            # Structure and schema of configs/stack.yaml
│   ├── customizing-install-paths.md           # Overriding /usr/local/ prefixes for customized distributions
│   ├── configuring-git-branches-and-tags.md   # Pinning releases (e.g., v1.0.0 vs. main) across repositories
│   └── customizing-cmake-build-flags.md       # Passing custom optimization flags (-O3, -march=native, -flto)
│
├── operations-and-makefile/                   # Maintenance & Lifecycle Operations
│   ├── makefile-reference.md                  # Complete make targets reference (install, verify, update, status)
│   ├── updating-installed-tiers.md            # Pulling latest git commits and recompiling (sudo make update)
│   ├── monitoring-daemons-status.md           # Inspecting running services with sudo make status
│   ├── clean-uninstallation-guide.md          # Complete uninstallation, daemon removal, and library purge
│   └── automated-backup-and-recovery.md       # Backing up /etc/sentinel/ and /opt/sentinel-nexus/ configuration
│
└── troubleshooting/                           # Help Desk & Diagnostics
    ├── oom-compiler-crashes.md                # Resolving g++ / clang internal compiler errors under low memory
    ├── missing-kernel-headers.md              # Debugging linux-headers mismatch during eBPF compilation
    ├── grpc-protobuf-linking-errors.md        # Fixing undefined symbol errors in gRPC/Protobuf dynamic libraries
    ├── python-externally-managed-errors.md    # Resolving PEP 668 pip errors via isolated virtual environment
    ├── systemd-service-failed-starts.md       # Diagnosing status=203/EXEC, missing configs, and permission issues
    ├── dynamic-linker-library-not-found.md    # Fixing "cannot open shared object file" via ldconfig / ld.so.conf
    ├── faq.md                                 # Technical Frequently Asked Questions
    └── support.md                             # Issue tracking, enterprise support SLAs, and reporting bugs
```


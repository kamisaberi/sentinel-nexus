# Project 2 of 8: `blackbox-essential` (`libblackbox.so`)
## Complete Documentation & Help Desk System File Structure (`docs/`)

This is the complete, full-version documentation tree for **`blackbox-essential`**. It is structured for technical documentation engines (**MkDocs Material**, **Docusaurus**, or **Starlight**) and covers in-kernel eBPF/XDP programming, driver-level packet filtering, lock-free SPMC memory concurrency, and physical TPM 2.0 hardware attestation.

---

```text
blackbox-essential/docs/
├── mkdocs.yml                                 # Documentation site configuration (navigation, theme, search)
├── index.md                                   # Documentation home & executive architectural overview
│
├── getting-started/                           # Onboarding & Setup
│   ├── overview.md                            # Core mission: Sub-microsecond (< 0.84µs) active defense
│   ├── system-requirements.md                 # Linux kernel 5.15+, eBPF JIT, clang, llvm, libelf-dev
│   ├── installation.md                        # Building from source, package managers, and library install
│   ├── cmake-integration.md                   # Linking libblackbox.so into C++20 applications via CMake
│   ├── quickstart-drop-test.md                # 5-minute first packet drop (minimal in-kernel filter test)
│   └── verifying-kernel-support.md            # Validating eBPF JIT, BTF, bpffs, and XDP driver support
│
├── architecture/                              # Deep Systems Design
│   ├── core-engine-design.md                  # Decoupled Tier 2 architecture & execution pipeline
│   ├── sub-microsecond-physics.md             # Why userspace firewalls fail critical cyber-physical systems
│   ├── zero-skb-allocation.md                 # Discarding frames before Linux kernel sk_buff creation
│   ├── data-plane-vs-control-plane.md         # Fast-path kernel filter vs. slow-path userspace controller
│   └── kernel-userspace-abi.md                # ABI stability, BPF map descriptors, and syscall boundaries
│
├── ebpf-xdp-subsystem/                        # Linux Kernel Ingress Engine
│   ├── xdp-filter-architecture.md             # Lifecycle of incoming Ethernet frames in xdp_threat_filter()
│   ├── driver-mode-vs-skb-mode.md             # Native XDP (XDP_FLAGS_DRV_MODE) vs. Generic SKB fallback
│   ├── bpf-verifier-guarantees.md             # Memory bounds safety proofs, loop limits, and safety verification
│   ├── compiling-bpf-bytecode.md              # Clang/LLVM compilation flags (-O2 -target bpf) and build_bpf.sh
│   ├── bpf-map-management.md                  # BPF_MAP_TYPE_HASH mechanics, sizing, and pinned map namespaces
│   ├── ttl-ephemeral-expiry.md                # Nanosecond timestamp eviction logic in kernel space
│   └── kernel-lockdown-and-signing.md         # Linux Kernel Lockdown Mode compatibility & cryptographic signing
│
├── spmc-ring-buffer/                          # Lock-Free Concurrency Core
│   ├── lock-free-architecture.md              # Single-Producer Multi-Consumer (SPMC) ring buffer overview
│   ├── atomic-memory-ordering.md              # Formalizing std::memory_order_release and memory_order_acquire
│   ├── cache-line-padding.md                  # Preventing false sharing with alignas(64) cache-line alignment
│   ├── zero-mutex-producer.md                 # Non-blocking enqueue operations from network driver threads
│   ├── multi-consumer-scaling.md              # Distributing flow events to parallel inference worker threads
│   └── saturation-and-backpressure.md         # Bounded circular capacity handling and tail-drop mechanics
│
├── hardware-identity-tpm/                     # Cryptographic Silicon Attestation
│   ├── attestation-architecture.md            # Why software-only identities fail in hostile environments
│   ├── tier1-physical-tpm2.md                 # Interfacing with /dev/tpmrm0 via TCG TSS2 specifications
│   ├── tpm2-pcr-measurements.md               # Generating cryptographic quotes over PCR 0 (BIOS) and PCR 4
│   ├── tier2-virtual-tpm.md                   # Hypervisor attestation (VMware vTPM, QEMU swtpm signatures)
│   ├── tier3-dmi-fallback.md                  # Motherboard DMI product_uuid hashing & machine-id fallback
│   ├── anti-cloning-protections.md            # Automated detection and revocation of cloned virtual appliances
│   └── quote-verification-flow.md             # Attestation Identity Key (AIK) signature validation protocol
│
├── af-xdp-zero-copy/                          # High-Throughput User-Space Networking
│   ├── umem-architecture.md                   # Packet buffer ring allocation in unified user-memory (UMEM)
│   ├── rx-fill-rings.md                       # Coordinating descriptor exchanges between NIC and userspace
│   ├── zero-copy-packet-transfer.md           # Zero-copy DMA transfers from NIC directly to inference memory
│   ├── line-rate-saturation-10gbe.md          # Pushing 1.25M+ events/sec on Intel X520 and E810 adapters
│   └── multi-core-rss-queues.md               # Scaling across multi-queue NICs using Receive Side Scaling (RSS)
│
├── model-config/                              # Decoupled Machine Learning Binding
│   ├── dynamic-tensor-binding.md              # Decoupling C++ security engines from neural network topologies
│   ├── mapping-tensor-dimensions.md           # Binding arbitrary input dimensions (32-dim, 42-dim, 80-dim)
│   ├── model-config-schema.md                 # JSON/YAML declarative configuration schema definition
│   └── threshold-and-mitigation-rules.md      # Mapping model output probabilities to in-kernel drop actions
│
├── api-reference/                             # Complete C++20 Doxygen/Breathe API Reference
│   ├── index.md                               # API namespace overview (`blackbox::`)
│   ├── xdp-manager.md                         # Class `blackbox::XdpManager`
│   ├── event-ring-buffer.md                   # Class `blackbox::EventRingBuffer`
│   ├── hardware-identity.md                   # Class `blackbox::HardwareIdentity`
│   ├── model-config.md                        # Class `blackbox::ModelConfig`
│   ├── kernel-telemetry.md                    # Struct `blackbox::KernelTelemetry`
│   ├── data-structures.md                     # Structs `XdpConfig`, `IdentityClaims`, `BlockedIpEntry`
│   └── error-codes.md                         # Class `blackbox::BlackboxException` & return codes
│
├── tutorials/                                 # Step-by-Step Practical Guides
│   ├── building-in-kernel-firewall.md         # Creating a wire-speed packet blocker in 50 lines of C++20
│   ├── attaching-xdp-to-vmware-vnic.md        # Configuring eBPF on VMware ens33 / vmxnet3 virtual interfaces
│   ├── extracting-tpm2-quotes.md              # Reading and verifying physical TPM 2.0 silicon quotes
│   ├── wiring-xdp-to-xinfer.md                # Connecting eBPF packet capture to xInfer neural scoring
│   └── high-rate-packet-blaster-testing.md    # Stress-testing the SPMC ring buffer with 1M+ packets/sec
│
├── benchmarking/                              # Performance Profiling & Sizing
│   ├── methodology.md                         # Microsecond-level timer standards and testbed hardware specs
│   ├── latency-percentiles.md                 # Empirical p50, p90, p95, p99, and p99.9 latency distributions
│   ├── xdp-vs-iptables-nftables.md            # Comparative analysis: eBPF/XDP vs. Linux Netfilter
│   ├── xdp-vs-suricata-nfqueue.md             # Comparative analysis: Driver-level XDP vs. userspace NFQUEUE
│   ├── cpu-cycle-profiling.md                 # Measuring CPU cycles per packet drop (< 120 cycles)
│   └── memory-saturation-benchmarks.md        # Measuring ring buffer stability under line-rate saturation
│
├── compliance/                                # Regulatory & Defense Certification
│   ├── cmmc-level-2.md                        # Mapping to CMMC 2.0 / NIST SP 800-171 Control SI.L2-3.14.1
│   ├── iec-62443-industrial.md                # Mapping to IEC 62443-3-3 System Integrity (FR 3) & Boundary (FR 5)
│   ├── eu-nis2-compliance.md                  # Fulfilling European NIS2 Article 21 incident handling mandates
│   └── audit-log-tamper-evidence.md           # Cryptographic integrity proofs for regulatory auditors
│
└── troubleshooting/                           # Help Desk & Diagnostics
    ├── bpf-verifier-rejection-guide.md        # Debugging "R1 invalid mem access", unbounded loops, and stack size
    ├── xdp-attachment-failures.md             # Resolving "Operation not supported" and driver attachment errors
    ├── tpm-permission-and-device-errors.md    # Fixing /dev/tpmrm0 access denied and missing resource manager
    ├── vmware-veth-skb-issues.md              # Debugging packet drops on VMware virtual interfaces and veth
    ├── faq.md                                 # Technical Frequently Asked Questions
    └── support.md                             # Issue tracker, security vulnerability disclosure, and support SLAs
```


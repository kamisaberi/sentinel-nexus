# Project 5 of 8: `sentinel-lab` (`sentinel_lab`)
## Complete Documentation & Help Desk System File Structure (`docs/`)

This is the complete, full-version documentation tree for **`sentinel-lab`**. It is structured for academic documentation platforms (**MkDocs Material**, **Docusaurus**, or **Starlight**) and covers open-science reproducibility, the **SLAB dynamic binary wire protocol**, the **CIC-IDS-2017 PortScan automated evaluation harness**, dual-silicon comparative benchmarking (**Intel OpenVINO vs. NVIDIA TensorRT**), and university thesis curriculum integration.

---

```text
sentinel-lab/docs/
├── mkdocs.yml                                 # Documentation site configuration (navigation, theme, search)
├── index.md                                   # Documentation home & academic research overview
│
├── getting-started/                           # Academic Onboarding & Setup
│   ├── overview.md                            # Open research testbed: Sub-microsecond threat mitigation
│   ├── system-requirements.md                 # Linux kernel 5.15+, Clang, LLVM, CMake 3.20+, Python 3.10+
│   ├── installation-and-build.md              # Compiling eBPF bytecode (build_bpf.sh) and native C++ testbed
│   ├── ten-minute-quickstart.md               # 10-minute setup: From git clone to empirical F1 & latency metrics
│   ├── verifying-environment.md               # Checking raw socket capabilities, eBPF JIT, and memory limits
│   └── architecture-at-a-glance.md            # High-level diagram: SLAB Stream -> eBPF Hook -> AI Silicon
│
├── architecture/                              # Research & Systems Engineering Design
│   ├── testbed-architecture.md                # Decoupled C++20 research engine and socket polling loop
│   ├── academic-reproducibility-imperative.md # Why static CSV training fails in real-world packet processing
│   ├── hardware-in-the-loop-design.md         # Validating true driver-level mitigation on live 10GbE network frames
│   ├── latency-measurement-physics.md         # Microsecond clock precision, hardware timestamps, and jitter
│   └── zero-overhead-instrumentation.md       # Profiling execution cycles without polluting benchmark results
│
├── slab-protocol/                             # The SLAB Universal Binary Wire Protocol
│   ├── protocol-specification.md              # SLAB specification: [Magic | EventID | GroundTruth | Dim | Tensor]
│   ├── bytefield-wire-layout.md               # Formal 24-byte header layout and 32-bit word alignment
│   ├── zero-copy-casting.md                   # Memory-mapping raw socket payload buffers directly to float arrays
│   ├── multi-dataset-compatibility.md         # Cross-dataset evaluation: CIC-IDS-2017 (32-dim) vs. UNSW-NB15 (42-dim)
│   ├── python-slab-serializer.md              # Using tools/csv_to_slab.py to convert custom research datasets
│   └── protocol-validation-checks.md          # Validating the 0x534C4142 magic header and payload bounds
│
├── dual-silicon-testbed/                      # Heterogeneous Silicon Comparison
│   ├── dual-target-comparative-model.md       # Comparative methodology: 1-to-1 parity on identical SLAB streams
│   ├── intel-openvino-pipeline.md             # Evaluating Intel Core Ultra NPU vs. Xeon CPU via ov::Tensor
│   ├── nvidia-tensorrt-pipeline.md            # Evaluating NVIDIA Jetson Orin vs. L4 GPU via CUDA streams
│   ├── cross-silicon-benchmark-standards.md   # Normalizing batch sizes, precision (FP16/INT8), and memory copies
│   └── thermal-and-power-profiling.md         # Measuring energy consumption (Joules per classification)
│
├── preprint-and-open-science/                 # Academic Publication & Artifacts
│   ├── preprint-overview.md                   # Paper abstract, theoretical formalization, and key findings
│   ├── cern-zenodo-doi.md                     # Permanent citable DOI (https://doi.org/10.5281/zenodo.XXXXXXX)
│   ├── compiling-latex-paper.md               # Compiling paper.tex locally (IEEE single-column pdflatex guide)
│   ├── citing-sentinel-lab.md                 # BibTeX entries, citation standards, and artifact attribution
│   └── open-access-licensing.md               # MIT License & Creative Commons Attribution 4.0 (CC-BY-4.0)
│
├── evaluation-harness/                        # Autonomous Benchmark Pipeline
│   ├── harness-architecture.md                # Architecture of examples/run_full_evaluation.py
│   ├── dataset-acquisition-cic-ids-2017.md    # Automated fetching of the official 77.4 MB CIC PortScan dataset
│   ├── flow-normalization-pipeline.md         # Normalizing raw flow statistics into invariant tensors [-1.0, 1.0]
│   ├── wire-speed-raw-socket-injection.md     # Streaming SLAB binary packets over raw AF_PACKET sockets (60k+ EPS)
│   ├── metrics-calculation.md                 # Computing Confusion Matrix, Accuracy, Precision, Recall, F1
│   └── percentile-latency-profiler.md         # Calculating exact p50, p90, p95, p99, and p99.9 latency distributions
│
├── comparative-benchmarks/                    # Empirical Performance Evaluation
│   ├── benchmark-methodology.md               # Bare-metal testbed hardware specs (i9-14900K, 192GB DDR5, X520 10GbE)
│   ├── sentinel-vs-suricata-7.md              # Sentinel XDP Drop (0.84µs) vs. Suricata NFQUEUE Drop (8.4ms)
│   ├── sentinel-vs-snort-3.md                 # Throughput, memory consumption, and rule evaluation comparisons
│   ├── sentinel-vs-elastic-siem.md            # Edge in-kernel mitigation vs. cloud log indexing latency (4.2s)
│   ├── sentinel-vs-splunk-enterprise.md       # Resource utilization: 180MB RAM (Sentinel) vs. 68GB RAM (Splunk)
│   ├── latency-cdf-percentiles.md             # Empirical Cumulative Distribution Function (CDF) curves
│   └── reproducibility-audit.md               # Third-party reproducibility verification logs
│
├── university-curriculum/                     # Master's & PhD Thesis Guide
│   ├── thesis-topics-guide.md                 # Ready-made research proposals in low-latency systems & edge AI
│   ├── course-module-integration.md           # Laboratory exercises for Advanced OS & Network Security courses
│   ├── taltech-collaboration-guide.md         # Research alignment with TalTech Cyber Security Laboratory
│   ├── aalto-collaboration-guide.md           # Research alignment with Aalto University Secure Systems Group
│   └── student-grant-support.md               # Supporting academic grant applications with benchmark data
│
├── tutorials/                                 # Hands-on Research Walkthroughs
│   ├── evaluating-custom-csv-datasets.md      # Converting and evaluating an unlabelled university campus PCAP
│   ├── benchmarking-intel-npu-vs-cpu.md       # Measuring inference latency on Intel Meteor Lake / Lunar Lake NPUs
│   ├── measuring-xdp-drop-cycles.md           # Measuring CPU clock cycles consumed per drop via Linux perf
│   ├── exporting-reproducible-csv-artifacts.md# Formatting benchmark outputs for publication-ready LaTeX tables
│   └── running-testbed-in-vmware.md           # Executing the research harness inside VMware virtual machines
│
└── troubleshooting/                           # Help Desk & Diagnostics
    ├── dataset-download-errors.md             # Resolving Canadian Institute for Cybersecurity archive timeouts
    ├── raw-socket-permission-denied.md        # Managing CAP_NET_RAW and root execution boundaries
    ├── ebpf-jit-compiler-errors.md            # Fixing missing kernel BTF, bpffs, and clang BPF target issues
    ├── openvino-tensorrt-linking-issues.md    # Resolving shared library paths and CUDA driver mismatches
    ├── faq.md                                 # Technical Frequently Asked Questions
    └── support.md                             # Academic issue tracker, contributing guide, and research contacts
```


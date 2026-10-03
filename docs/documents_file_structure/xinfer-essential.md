# Project 1 of 8: `xinfer-essential` (`libxinfer.so`)
## Complete Documentation & Help Desk System File Structure (`docs/`)

This is the complete, full-version documentation tree for **`xinfer-essential`**. It is formatted for modern documentation engines (**MkDocs Material**, **Docusaurus**, or **Starlight**) and covers all 15 silicon targets, zero-copy memory architectures, C++20 API references, and plugin development.

---

```text
xinfer-essential/docs/
├── mkdocs.yml                                 # Documentation site configuration (navigation, theme, search)
├── index.md                                   # Documentation home & executive architectural overview
│
├── getting-started/                           # Onboarding & Setup
│   ├── overview.md                            # High-level runtime capabilities & design principles
│   ├── system-requirements.md                 # OS, compiler (GCC 12+/Clang 16+), and hardware prerequisites
│   ├── installation.md                        # Building from source, package managers, and binary installation
│   ├── cmake-integration.md                   # Linking libxinfer.so via CMake (find_package vs target_link_libraries)
│   ├── hello-world.md                         # 5-minute first inference run (minimal C++20 example)
│   └── verifying-installation.md              # Running built-in self-tests and sanity checks
│
├── architecture/                              # Deep Systems Design
│   ├── core-engine-design.md                  # Decoupled C++20 execution pipeline & state machine
│   ├── zero-copy-model.md                     # Eliminating memcpy bottlenecks at wire-speed
│   ├── memory-domains.md                      # Host, Device, Host-Pinned, and DMA-BUF memory classification
│   ├── thread-safety-concurrency.md           # Lock-free execution, re-entrancy, and multi-threaded scaling
│   └── symbol-isolation.md                    # Dynamic linker boundary rules (-fvisibility=hidden & XINFER_API)
│
├── silicon-backends/                          # The 15 Hardware Execution Guides
│   ├── index.md                               # Silicon compatibility matrix & capability overview
│   ├── nvidia-tensorrt.md                     # CUDA streams, TensorRT .engine loading, FP16/INT8 execution
│   ├── intel-openvino.md                      # Core Ultra NPU, Xeon CPU, and iGPU execution via ov::Tensor
│   ├── rockchip-rknn.md                       # RKNPU2 driver integration, .rknn model format, and DMA-BUF
│   ├── qualcomm-qnn.md                        # Snapdragon NPU / Hexagon DSP via FastRPC shared memory
│   ├── amd-vitis-ai.md                        # Xilinx DPU FPGA execution (.xmodel) over PCIe DMA rings
│   ├── apple-coreml.md                        # Apple Silicon Neural Engine execution (.mlmodelc) via Metal
│   ├── amd-ryzen-ai.md                        # AMD XDNA NPU architecture integration (.onnx)
│   ├── mediatek-neuropilot.md                 # MediaTek APU acceleration (.dla / .pte) via ION memory
│   ├── hailo-hailort.md                       # Hailo-8 M.2 coprocessor driver integration (.hef)
│   ├── ambarella-cvflow.md                    # Ambarella CVFlow vision processor execution (.cavalry)
│   ├── samsung-enn.md                         # Exynos NPU execution (.nnc) over hardware memory rings
│   ├── google-coral-edgetpu.md                # Edge TPU USB/PCIe execution (.tflite) via libedgetpu
│   ├── intel-fpga-ai-suite.md                 # Altera Cyclone/Arria/Agilex bitstream loading (.aocx)
│   ├── microchip-vectorblox.md                # PolarFire FPGA execution (.blob) over AXI shared RAM
│   └── lattice-sensai.md                      # Ultra-low power iCE40/CrossLink FPGA streaming (.bin)
│
├── memory-management/                         # Zero-Copy & Low-Level Memory
│   ├── dma-buf-integration.md                 # Direct mapping of Linux kernel DMA-BUF descriptors
│   ├── host-pinned-memory.md                  # Allocating non-pageable memory (cudaHostRegister/mlock)
│   ├── unified-memory.md                      # Unified virtual address spaces on heterogeneous SoCs
│   ├── tensor-backing-buffers.md              # Managing persistent memory ownership and memory pools
│   └── alignment-and-cache.md                 # 64-byte cache-line alignment and TLB miss optimization
│
├── plugin-development/                        # Dynamic Extension Subsystem
│   ├── plugin-architecture.md                 # Dynamic linker mechanics (dlopen with RTLD_LAZY | RTLD_LOCAL)
│   ├── iinference-plugin-interface.md         # Implementing the C++20 IInferencePlugin ABI contract
│   ├── building-custom-plugins.md             # Writing, compiling, and testing a custom pre/post-processor
│   ├── plugin-lifecycle.md                    # Registration, validation, execution, and teardown states
│   └── standard-plugins/                      # Documentation for the 30 Pre-Built Plugins
│       ├── yolo-nms-decoder.md                # Vision: Non-Maximum Suppression bounding box parser
│       ├── nvdec-video-unpacker.md            # Vision: Hardware-accelerated H.264/HEVC stream decoder
│       ├── ultraface-detector.md              # Vision: Real-time facial bounding box and landmark extractor
│       ├── thermal-matrix-normalizer.md       # Vision: Radiometric infrared sensor tensor converter
│       ├── mel-spectrogram-fft.md             # Audio: Acoustic anomaly and vibration FFT generator
│       ├── netflow-tensor-assembler.md        # Network: Directional 32-dim flow vector constructor
│       ├── modbus-apdu-vectorizer.md          # SCADA: Industrial Modbus register-to-tensor mapping
│       ├── dicom-pacs-normalizer.md           # Medical: 16-bit radiology pixel array normalizer
│       └── aes-weight-decryption.md           # Security: Hardware-decrypted weight unbundler on boot
│
├── model-hub/                                 # Model Resolution & Caching
│   ├── modelhub-architecture.md               # Dynamic resolution pipeline (Memory -> Local -> HTTPS)
│   ├── https-caching-rules.md                 # Remote repository synchronization and cache invalidation
│   ├── cryptographic-verification.md          # Pre-execution SHA-256 checksum enforcement
│   ├── air-gapped-offline-mode.md             # Pre-seeding models for air-gapped and classified deployments
│   └── supported-model-formats.md             # ONNX (Opset 11--17), TensorRT Engine, OpenVINO IR, RKNN
│
├── api-reference/                             # Complete C++20 Doxygen/Breathe API Reference
│   ├── index.md                               # API namespace overview (`xinfer::`)
│   ├── inference-engine.md                    # Class `xinfer::InferenceEngine`
│   ├── tensor.md                              # Class `xinfer::Tensor`
│   ├── model-hub.md                           # Class `xinfer::ModelHub`
│   ├── plugin-manager.md                      # Class `xinfer::PluginManager`
│   ├── engine-config.md                       # Struct `xinfer::EngineConfig`
│   ├── data-types.md                          # Enums `DataType`, `Precision`, `BackendType`, `MemoryType`
│   └── error-handling.md                      # Class `xinfer::InferenceException` & error return codes
│
├── tutorials/                                 # Step-by-Step Practical Guides
│   ├── netflow-threat-autoencoder.md          # 1D tabular vector scoring in under 12 microseconds
│   ├── realtime-yolo-edge-vision.md           # 30 FPS camera inference on Rockchip RK3588 & Jetson
│   ├── scada-modbus-anomaly-detection.md      # Inline industrial control frame inspection
│   ├── thermal-overheating-detector.md        # Real-time industrial physical equipment monitoring
│   └── dual-model-shadow-execution.md         # Running production and candidate models in parallel
│
├── benchmarking/                              # Performance Profiling & Sizing
│   ├── methodology.md                         # Microsecond-level hardware timer profiling standards
│   ├── bare-metal-results.md                  # Comprehensive results on Intel Xeon, i9-14900K, and Jetson
│   ├── comparative-studies.md                 # xInfer vs. ONNX Runtime vs. LibTorch vs. OpenVINO Native
│   ├── memory-profiling.md                    # Measuring idle vs. saturation memory usage and heap churn
│   └── power-efficiency-joules.md             # Performance-per-watt analysis on edge ARM/NPU boards
│
└── troubleshooting/                           # Help Desk & Diagnostics
    ├── common-build-errors.md                 # Missing compilers, CMake version mismatches, missing drivers
    ├── linker-symbol-conflicts.md             # Debugging dynamic library collisions and undefined symbols
    ├── backend-initialization-failures.md     # Resolving driver issues (/dev/rknpu, CUDA init, NPU busy)
    ├── out-of-memory-diagnostics.md           # Diagnosing IOMMU, DMA allocation, and swap exhaustion
    ├── faq.md                                 # Technical Frequently Asked Questions
    └── support.md                             # GitHub issues, enterprise support SLAs, and reporting bugs
```

---

*This concludes the complete documentation system structure for **Project 1: `xinfer-essential`**.*  
*Ready to proceed to **Project 2: `blackbox-essential` (`libblackbox.so`)** upon your confirmation.*
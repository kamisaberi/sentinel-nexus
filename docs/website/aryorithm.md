# Aryorithm Corporate & Commercial Web Architecture: Expanded Pages Brief

This document provides the complete UI/UX blueprint, technical copywriting, and information architecture for the commercial, operational, and user-facing expansion pages of **`aryorithm.com`**.

---

## 1. Information Architecture & Expanded Sitemap

```text
aryorithm.com
├── /platform                     (Sentinel Nexus Fleet Grid)
├── /products                     (Blackbox Sentinel Edge Appliance)
├── /technology                   (xInfer & Blackbox C++20 Core Engines)
├── /research                     (Sentinel-Lab Academic Preprint & Benchmarks)
│
├── [ EXPANDED COMMERCIAL & CORPORATE PAGES ]
├── /about                        (Company Story, Engineering Ethos & Sovereignty)
├── /team                         (Leadership, Kernel Engineers, Silicon Architects & Careers)
├── /pricing                      (Licensing Tiers, Hardware Appliances & ROI Calculator)
├── /contact                      (Defense Procurement, POC Units & Encrypted PGP Comms)
├── /portal                       (Customer Enclave: Login, Air-Gapped Licensing & WebAuthn)
│   ├── /portal/login
│   ├── /portal/register
│   └── /portal/license-activate
├── /insights                     (Zero-Day Advisories, Benchmark Papers & Engineering Logs)
├── /faq                          (Deep-Tech Technical FAQ & Architectural Comparisons)
└── /trust                        (Compliance Center: CMMC 2.0, NIST SP 800-171, IEC 62443)
```

---

## 2. Page-by-Page Detailed Blueprints

---

### Page 1: About Us (`/about`) — *The Sovereign Engineering Mission*

#### Page Intent
Establishes Aryorithm as a mission-driven, deep-tech research and engineering firm rather than another venture-backed SaaS repackaging cloud APIs. It explains the philosophical and mathematical necessity of moving from **retrospective cloud querying** to **deterministic edge mitigation**.

#### Wireframe & Visual Sections
1. **Hero Banner:**
   * **Badge:** `MISSION DIRECTIVE`
   * **Headline:** *"Engineered for the Millisecond Where Cloud Defense Fails."*
   * **Subheadline:** *"When a kinetic cyber-physical attack targets an electrical grid, a naval vessel, or an automated production line, cloud-bound SIEMs taking 15 to 60 seconds to query logs are already post-mortems. Aryorithm was founded to enforce sub-microsecond, hardware-attested mitigation at the physical edge."*
2. **The "Why We Exist" Manifesto:**
   * Two visual columns: On the left, a schematic of a weaponized SCADA exploit hitting a turbine in $3.2\,\text{ms}$; on the right, the traditional cloud pipeline (log collection $\rightarrow$ transport $\rightarrow$ ingestion $\rightarrow$ elastic index $\rightarrow$ ticket generation $\rightarrow$ manual human drop = **38 seconds**).
   * Aryorithm’s answer: In-kernel eBPF drop executed in **`0.84 microseconds`**.
3. **Core Engineering Axioms (Interactive Cards):**
   * **Axiom 1: Deterministic C++20 Fast-Path:** Zero garbage collection, zero Python runtime in the mitigation path, zero thread-blocking mutexes.
   * **Axiom 2: Cryptographic Silicon Roots:** Security cannot rest on software MAC addresses. Every appliance is attested via physical TPM 2.0 silicon quotes.
   * **Axiom 3: Absolute Air-Gap Sovereignty:** Zero cloud dependencies, zero external CDNs, and zero data leakage. High-value telemetry stays on the owner’s premises.
4. **Company Milestones & Lineage:**
   * Interactive vertical timeline showing the technical progression from foundational Linux kernel eBPF/XDP research to 15-backend silicon inference runtimes, culminating in Sentinel Nexus fleet orchestration.

---

### Page 2: Team, Leadership & Careers (`/team` & `/careers`)

#### Page Intent
Instills high confidence in defense contractors, enterprise CISOs, and silicon partners by highlighting deep systems engineering credentials (kernel contributors, hardware architects, and machine learning researchers).

#### Wireframe & Visual Sections
1. **Leadership & Architecture Board:**
   * High-contrast, monochromatic executive cards with glowing border hovers:
     * **Founder & Chief Systems Architect:** Background in low-latency Linux kernel internals, eBPF, and real-time defense computing.
     * **Head of Silicon AI & Inference:** Specialist in heterogeneous acceleration (NVIDIA TensorRT, Intel OpenVINO, NPU compilation).
     * **Director of Cyber-Physical Systems (CPS):** Veteran of SCADA/ICS critical infrastructure, Modbus/DNP3 reverse engineering.
     * **Head of Cryptography & Hardware Security:** Specializing in TCG TPM 2.0 specifications, zero-trust attestation, and side-channel power analysis.
2. **The Advisory Enclave:**
   * Former industrial control system CISOs, naval electrical engineers, and high-performance computing researchers.
3. **Engineering Culture (`/careers` section):**
   * **Headline:** *"We Do Not Build CRUD Apps. We Write Systems That Cannot Fail."*
   * **Tech Stack Showcase:** `C++20`, `Linux Kernel (XDP/eBPF)`, `CUDA / TensorRT`, `TCG TPM 2.0`, `PyTorch (MAE/InfoNCE)`, `Zero-Copy DMA-BUF`.
4. **Open Roles (Filterable Technical Openings):**
   * *Staff Systems Engineer (Linux Kernel & eBPF/XDP)*
   * *Senior AI Silicon Compiler Engineer (OpenVINO / TensorRT / RKNN)*
   * *Industrial Protocol Security Researcher (SCADA / Modbus / DNP3 / S7)*
   * *Lead UI/UX Systems Designer (High-Density Air-Gapped Web Dashboards)*
   * Each job listing includes an interactive `"Engineering Challenge"` terminal modal where candidates can paste C++ or eBPF patches to fast-track their interview.

---

### Page 3: Pricing, Licensing & Enclave Tiers (`/pricing`)

#### Page Intent
Transparently structures commercialization across three tiers: Open-Core/Academic, Turnkey Edge Appliances, and Enterprise Multi-Site Orchestration.

```text
========================================================================================================
 TIER 1: OPEN RESEARCH         TIER 2: EDGE APPLIANCE         TIER 3: SENTINEL NEXUS         TIER 4: SOVEREIGN DEFENSE
 (Academic & Community)       (Single Node Commercial)       (Enterprise Fleet Grid)         (Classified / Air-Gapped)
--------------------------------------------------------------------------------------------------------
 • libxinfer.so Core           • 1x Blackbox Sentinel Node    • Central Nexus Orchestrator   • Dedicated Custom Silicon
 • libblackbox.so Core         • All 26 Decoupled Modules     • Up to 5,000 Edge Appliances  • Link-16 / Military Plugins
 • Sentinel-Lab Benchmarks     • 30 Industrial OT/IT Plugins  • Sub-50ms Collective Defense  • Cryptographic Sneakernet
 • Community Discord Support   • Local eBPF (<1ms Drop)       • Automated Canary OTA Rollout • Custom Safety Gates
 • Free / Open-Core            • Air-Gapped Web UI (8443)     • CMMC / IEC 62443 Reporting   • L3 24/7 Cleared Support
                               • Hardware or Virtual VM       • xinfer-forge Retraining Farm • Custom SLA & Source Escrow
 Free & Open Source            $4,800 / node / year           Contact for Deployment Quote   Custom Sovereign Contract
========================================================================================================
```

#### Interactive Elements
1. **Interactive Fleet ROI Calculator:**
   * **Inputs:** Sliders for `Number of Sites` (1 to 500) and `Daily NetFlow Volume` (GB/day).
   * **Outputs:** 
     * *Cloud SIEM Ingestion & Egress Cost:* Displays estimated annual AWS/Splunk bills (e.g., $180,000/yr).
     * *Aryorithm Sentinel Nexus Cost:* Displays edge fixed cost with **`$0 Cloud Egress`** and **`94% Cost Reduction`**.
2. **Appliance Form Factor Selector:**
   * Toggle between **Turnkey 1U Rugged Hardware** (shipped pre-configured with TPM 2.0 and Intel/NVIDIA accelerators) and **Virtual Appliance** (OVA/QCOW2 image ready for VMware vSphere, KVM, or Proxmox).

---

### Page 4: Contact Us & Defense Procurement (`/contact`)

#### Page Intent
Provides tailored communication paths for enterprise POC requests, military/defense procurement, and cryptographically secure zero-day vulnerability disclosures.

#### Wireframe & Visual Sections
1. **Split-Screen Contact Hub:**
   * **Left Side: Direct Procurement Inquiries**
     * Interactive intake form:
       * Work Email & Organization Name
       * Deployment Scope (Critical Infrastructure OT, Defense/Government Enclave, Enterprise IT/DMZ, Academic Research)
       * Silicon Architecture of Interest (Intel OpenVINO, NVIDIA TensorRT, Rockchip RKNN, Hailo, Qualcomm)
       * Request Type: *Request Evaluation Unit (Hardware POC)*, *Schedule Technical Briefing*, or *Procurement Contract (Cage Code / DUNS)*.
   * **Right Side: Sovereign & Cleared Communications**
     * **PGP Public Key:** Visual block showing the fingerprint and a 1-click `[ Copy PGP Key ]` button for sensitive/classified RFP submissions.
     * **Encrypted Security Channel:** Dedicated contact for responsible vulnerability disclosure.
     * **Physical Lab Locations:** Primary testing facility in Amsterdam, Netherlands, and secure evaluation enclaves.

---

### Page 5: Customer Enclave Portal (`/login` & `/register`)

#### Page Intent
Authentication for licensed customers. Designed as an ultra-hardened, zero-trust terminal interface without social sign-ins (no Google/GitHub buttons).

#### UX & Visual Design
* **Interface Style:** Centered minimalist black card (`#0D111A`) framed by a cyan laser border and real-time TLS connection telemetry.
* **Authentication Options:**
  1. **FIDO2 / WebAuthn Hardware Security Key (Default):** Prompts for physical YubiKey or Titan security key touch.
  2. **TPM 2.0 Hardware Machine Certificate:** mTLS mutual handshake verifying the engineer’s machine identity.
  3. **Enterprise SSO / SAML:** For Okta, Ping Identity, and Microsoft Entra ID with mandatory TOTP MFA.
* **Air-Gapped Offline Activation Modal (`/portal/license-activate`):**
  * For isolated facilities without internet access:
    * Paste the appliance hardware quote generated by `./sentinel --generate-hardware-token`.
    * Download an offline, cryptographically signed `.lic` envelope to activate enterprise modules on-premise.

---

### Page 6: Technical Insights, News & Research (`/insights`)

#### Page Intent
Demonstrates technical authority through deep-dive engineering whitepapers, benchmark releases, and zero-day threat analysis rather than generic marketing blog posts.

#### Content Taxonomy & Sample Articles
1. **Advisories & Exploit Dissections:**
   * *“Dissecting the Modbus Function Code 0x05 Exploit: How eBPF Kernel Drops Prevent PLC Valve Manipulation in Under 1 Microsecond.”*
   * *“Why Prompt Injection Firewalls (Module 23) Fail in Python and How In-Memory C++ Hook Guards Succeed.”*
2. **Silicon Acceleration Engineering:**
   * *“Pushing 1.25 Million Packets/sec with AF_XDP and Zero-Copy DMA-BUF Across Intel Xeon and Core i9-14900K.”*
   * *“Porting YOLOv8 & NetFlow Autoencoders to Rockchip RKNN and Hailo-8: A Comparative Power-Performance Study.”*
3. **Continuous Learning & AI Safety:**
   * *“Defeating Adversarial Model Poisoning at the Edge: Formalizing the Golden Attack Regression Safety Gate.”*
4. **Features:**
   * Filter tags by silicon (`OpenVINO`, `TensorRT`, `eBPF`, `SCADA`).
   * Clean syntax-highlighted C++20 and eBPF code snippets using dark terminal formatting.
   * Direct links to download PDF versions and raw benchmark data artifacts.

---

### Page 7: Technical Knowledge Base & FAQ (`/faq`)

#### Page Intent
Addresses technical, architectural, and procurement questions asked by CISOs, network engineers, and compliance auditors.

#### Structured Questions & Answers
* **Q1: Why use Linux eBPF/XDP instead of Suricata or Snort?**
  * *Answer:* Suricata and Snort operate in userspace via `libpcap` or `NFQUEUE`. Packets must traverse the entire Linux network stack, undergo socket allocation, and cross kernel-user boundaries, introducing 5.0 to 15.0 milliseconds of latency. Aryorithm’s `xdp_filter.o` executes directly at the network interface card (NIC) driver level before socket memory is allocated, dropping malicious packets in **0.84 microseconds**—over 5,000 times faster.
* **Q2: How does `xinfer-forge` adapt models without an internet connection?**
  * *Answer:* Forge runs as an on-premise background daemon implementing Self-Supervised Masked Autoencoding (MAE) on ambient, unlabeled site NetFlow vectors. It does not send data offsite. Before any candidate model is compiled to ONNX, it must achieve 100% detection on an immutable local suite of `golden_attacks.yaml`. If a candidate misses a single known threat, it is aborted automatically.
* **Q3: What happens if an edge appliance loses connection to Sentinel Nexus?**
  * *Answer:* Appliances are autonomous by design. If a WAN link drops, the edge appliance continues running its local eBPF filter and `libxinfer` engine without degradation. When reconnected, buffered candidate vectors and drop logs are automatically synchronized.
* **Q4: Can Blackbox Sentinel run inside VMware or virtualized environments?**
  * *Answer:* Yes. `libblackbox` utilizes an adaptive 3-tier hardware identity engine. It first queries `/dev/tpmrm0` for physical TPM 2.0; if virtualized, it identifies VMware vTPM / QEMU swtpm; if neither is present, it computes a cryptographic hash from the motherboard DMI product UUID.
* **Q5: Which regulatory compliance standards does Aryorithm support out of the box?**
  * *Answer:* Nexus generates automated, cryptographically verifiable compliance reports for **CMMC 2.0 (Level 2)**, **NIST SP 800-171**, and **IEC 62443-3-3 / 4-2** for industrial critical infrastructure.

---

### Page 8: Trust, Compliance & Verification Center (`/trust`)

#### Page Intent
Serves as the central repository for enterprise security audits, cryptographic verification keys, and compliance mapping.

#### Key Content Blocks
1. **Compliance Matrix:**
   * **CMMC 2.0 Level 2 / NIST SP 800-171:** Control mappings for `AC.L2-3.1.1` (Access Control), `IA.L2-3.5.1` (Hardware Identification & Authentication), and `SI.L2-3.14.1` (Sub-millisecond Incident Response SLA).
   * **IEC 62443:** Zone segmentation, SCADA protocol constraint validation (Modbus, DNP3, PROFINET).
2. **Software Bill of Materials (SBOM):**
   * Downloadable cryptographically signed CycloneDX and SPDX SBOMs proving zero hidden third-party cloud dependencies.
3. **Cryptographic Checksum Repository:**
   * Public SHA-256 and GPG verification signatures for all released `.onnx` models, `libxinfer.so`, `libblackbox.so`, and appliance ISOs.

---

## 3. UI/UX Component Specifications for Frontend Engineers

To ensure seamless implementation by your web development team, build the site using these specific UI components:

```text
[ ARYORITHM FRONTEND ATOMIC COMPONENT LIBRARY ]
 ├── Primitives/
 │   ├── TerminalWindow.tsx      -> Dark macOS/Linux style header with code block & copy button
 │   ├── MetricBadge.tsx         -> Glowing monospace metric indicator (e.g. "0.84 µs")
 │   ├── LaserBorderCard.tsx     -> Panel with subtle animated gradient stroke on hover
 │   └── StatusPulse.tsx         -> Green/Red/Cyan heartbeat dot indicator
 │
 ├── Interactive Modules/
 │   ├── LatencyComparator.tsx   -> Slider comparing eBPF vs Cloud SIEM tick clock
 │   ├── SiliconGridSelector.tsx -> 15-target clickable chip matrix (OpenVINO, TensorRT, etc.)
 │   ├── WebCliEmulator.tsx      -> In-browser interactive `nexus-ctl` command sandbox
 │   └── EnclaveMap.tsx          -> HTML5 Canvas radial fleet topology animation
 │
 └── Modals & Forms/
     ├── PgpModal.tsx            -> 1-click GPG public key modal with fingerprint verification
     └── HardwareQuoteForm.tsx   -> Interactive hardware configuration builder for POC units
```

This comprehensive design brief equips your design and frontend development team to build an authoritative, deep-tech web presence at **`aryorithm.com`** that communicates the sovereign performance of the Blackbox Sentinel ecosystem.
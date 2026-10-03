# Project 4 of 8: `xinfer-forge` (`forge-cli`)
## Complete Documentation & Help Desk System File Structure (`docs/`)

This is the complete, full-version documentation tree for **`xinfer-forge`**. It is structured for technical documentation engines (**MkDocs Material**, **Docusaurus**, or **Starlight**) and covers self-supervised Masked Autoencoders (MAE), InfoNCE contrastive representation learning, the immutable golden attack regression safety gate, automated ONNX Opset 17 compilation, and closed-loop Sentinel Nexus staging.

---

```text
xinfer-forge/docs/
├── mkdocs.yml                                 # Documentation site configuration (navigation, theme, search)
├── index.md                                   # Documentation home & executive architectural overview
│
├── getting-started/                           # Onboarding & Setup
│   ├── overview.md                            # Edge continual active learning without cloud connectivity
│   ├── system-requirements.md                 # Python 3.10+, PyTorch CPU/CUDA, ONNX, and isolated venv
│   ├── installation.md                        # Package setup, PEP 668 isolated environment (/opt/sentinel-stack/venv)
│   ├── quickstart-first-adaptation.md         # 5-minute training run on a mock 32-dim NetFlow batch
│   ├── verifying-setup.md                     # Validating PyTorch tensor engines, ONNX exporters, and CLI stubs
│   └── architecture-at-a-glance.md            # High-level diagram: Telemetry -> MAE -> Safety Gate -> Nexus
│
├── architecture/                              # Deep Systems Design
│   ├── continual-learning-architecture.md     # Decoupled continuous training pipeline & event loop
│   ├── the-concept-drift-problem.md           # Why static detection models degrade in industrial networks
│   ├── the-adversarial-poisoning-problem.md   # Mathematical modeling of "boiling-the-frog" data poisoning
│   ├── closed-loop-flywheel-design.md         # Edge extraction -> Nexus curation -> Forge training -> Hot-reload
│   └── air-gapped-execution-model.md          # 100% on-premise execution with zero external WAN connectivity
│
├── self-supervised-engine/                    # Machine Learning Core (MAE & InfoNCE)
│   ├── mae-architecture.md                    # 32-dimensional continuous flow tensor autoencoder topology
│   ├── stochastic-masking-strategy.md         # 30% random feature masking for tabular physical representations
│   ├── infonce-contrastive-learning.md        # Manifold regularization and temporal flow alignment
│   ├── latent-embedding-space.md              # Compact bottleneck geometry (z in R^8) and outlier separation
│   ├── loss-optimization-math.md              # Total loss formulation: L_total = L_MAE + lambda * L_InfoNCE
│   ├── neural-layer-specifications.md         # Linear projections, LayerNorm, and LeakyReLU activation functions
│   └── hyperparameter-tuning.md               # Learning rates, batch sizing, masking ratios, and temperature (tau)
│
├── safety-regression-gate/                    # Anti-Poisoning & Verification Core
│   ├── safety-gate-philosophy.md              # The non-negotiable zero-tolerance regression invariant
│   ├── golden-attacks-corpus.md               # Structure and taxonomy of configs/safety/golden_attacks.yaml
│   ├── threat-categories-covered.md           # Modbus overrides (T0855), Triton (T0843), Stuxnet (T0831), C2 (T1071)
│   ├── zero-tolerance-math.md                 # Strict logical conjunction proof: S(theta*) = 1.000
│   ├── automated-purge-circuit.md             # Immediate deletion of compromised candidate weights
│   ├── alert-dispatch-on-regression.md        # Raising CISO critical alarms when poisoning attempts occur
│   └── tpm-pcr-sealing.md                     # Cryptographically anchoring golden attack signatures to physical TPM 2.0
│
├── compilation-and-staging/                   # Model Export & Distribution
│   ├── onnx-export-pipeline.md                # Compiling PyTorch .pt weights via torch.onnx.export (Opset 17)
│   ├── dynamic-batch-axes.md                  # Supporting variable inference batch sizes: [batch_size, 32]
│   ├── cryptographic-hashing-sha256.md        # Generating checksum manifests to prevent in-flight tampering
│   ├── nexus-rest-staging-api.md              # Dispatching POST /api/v1/ota/stage payloads to Sentinel Nexus
│   └── staged-rollout-lifecycle.md            # Tracking progression: SHADOW_MODE -> CANARY_5_PCT -> FLEET_WIDE
│
├── nexus-integration/                         # Tier 6 Fleet Synchronization Bridge
│   ├── nexus-bridge-architecture.md           # Architecture of forge/nexus_bridge.py
│   ├── dataset-discovery-watcher.md           # Monitoring /var/lib/sentinel-nexus/forge_datasets/ for batches
│   ├── parsing-curated-csv-batches.md         # Reading forge_dataset_*.csv and .manifest.json descriptors
│   ├── active-learning-uncertainty-gate.md    # Filtering vectors in the [0.40 - 0.60] prediction entropy window
│   ├── automated-wrapper-script.md            # Executing deploy/run_nexus_adaptation.sh
│   └── closed-loop-validation-testing.md      # Testing that model v2 detects zero-days missed by model v1
│
├── cli-reference/                             # forge-cli Command Reference
│   ├── cli-overview.md                        # Command syntax, flags, and environment variables
│   ├── command-train.md                       # `forge-cli train`: Manual dataset training execution
│   ├── command-validate-safety.md             # `forge-cli validate-safety`: Standalone golden gate audit
│   ├── command-export-onnx.md                 # `forge-cli export-onnx`: Standalone ONNX compilation
│   ├── command-stage.md                       # `forge-cli stage`: Remote staging to Sentinel Nexus
│   ├── command-auto-cycle.md                  # `forge-cli auto-cycle`: Autonomous infinite adaptation loop
│   └── configuration-files.md                 # Structure of forge_config.yaml and training hyperparameter files
│
├── tutorials/                                 # Practical Step-by-Step Guides
│   ├── training-on-ambient-netflow.md         # End-to-end retraining on raw industrial network captures
│   ├── adding-custom-golden-attacks.md        # Embedding new proprietary zero-day signatures into the safety gate
│   ├── tuning-mae-masking-ratio.md            # Optimizing tabular masking for SCADA vs. enterprise IT traffic
│   ├── recovering-from-poisoning-alerts.md    # Investigating why a candidate model was rejected by the safety gate
│   └── deploying-forge-in-vmware.md           # Configuring Forge inside sentinel-matrix Docker/VMware mesh
│
├── benchmarking/                              # Performance Profiling & Empirical Data
│   ├── methodology.md                         # Benchmark metrics, continuous drift simulation, and testbeds
│   ├── drift-adaptation-curves.md             # 6-month accuracy comparison: Static (64.2%) vs. Forge (98.0%)
│   ├── poisoning-resilience-experiments.md    # Empirical results under active adversarial poisoning waves
│   ├── cpu-vs-gpu-training-speed.md           # Execution benchmarks: 4-Core Intel CPU (18s) vs. NVIDIA L4 (1.8s)
│   └── resource-footprint.md                  # Disk consumption, checkpoint pruning, and memory usage
│
├── compliance/                                # AI Safety & Regulatory Alignment
│   ├── eu-ai-act-article-15.md                # Satisfying High-Risk AI mandates for robustness and anti-poisoning
│   ├── nist-sp-800-218-ssdf.md                # Validating AI model integrity under Task PW.8.1
│   ├── auditing-training-runs.md              # Immutable logging of loss curves, sample counts, and hashes
│   └── data-privacy-zero-egress.md            # Proving zero PII/customer data leakage in self-supervised learning
│
└── troubleshooting/                           # Help Desk & Diagnostics
    ├── loss-divergence-and-nans.md            # Resolving exploding gradients and numerical instability in MAE
    ├── safety-gate-rejection-guide.md         # Debugging false negatives during golden attack regression audits
    ├── nexus-staging-failures.md              # Resolving REST timeouts, connection refused, and invalid URLs
    ├── python-pep668-venv-issues.md           # Fixing Ubuntu 24.04/26.04 externally-managed-environment errors
    ├── faq.md                                 # Technical Frequently Asked Questions
    └── support.md                             # Issue reporting, security disclosures, and enterprise support SLAs
```

---

*This concludes the complete documentation system structure for **Project 4: `xinfer-forge`**.*  
*Ready to proceed to **Project 5: `sentinel-lab` (Tier 5 Academic Research Testbed, SLAB Protocol & Preprint)** upon your confirmation.*
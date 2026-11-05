# Canary OTA Model Rollout

`sentinel-nexus` manages neural network model deployments through a three-stage rollout lifecycle (`Shadow Mode` $\to$ `5% Canary` $\to$ `Fleet-Wide`) guarded by automated SLA watchdogs.

## Section Navigation

- [Staged Rollout Lifecycle](./staged-rollout-lifecycle.md): State transitions, promotion criteria, and risk mitigation invariants.
- [Canary Orchestrator Engine](./canary-orchestrator-engine.md): Deterministic cohort assignment using SHA-256 consistent hashing over node UUIDs.
- [RollbackGuard SLA Watchdog](./rollback-guard-sla-watchdog.md): Real-time SLA watchdog enforcing the $1{,}000\,\mu\text{s}$ ($1.0\,\text{ms}$) latency ceiling.
- [False-Positive Surge Protection](./false-positive-surge-protection.md): EWMA drop rate monitoring triggering automated rollbacks on $> 5\times$ drop spikes.
- [Model Repository & Hashing](./model-repository-and-hashing.md): Local file layout (`/var/lib/sentinel-nexus/models/`) and streaming HTTP/2 downloads.
- [Zero-Downtime Hot Reload](./zero-downtime-hot-reload-flow.md): Atomic memory pointer swaps executing model updates with 0ms downtime.

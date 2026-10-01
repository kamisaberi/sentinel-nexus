---

### File: `sentinel-nexus/docs/canary-ota-rollout/rollback-guard-sla-watchdog.md`

```markdown
# `RollbackGuard`: Automated SLA Latency Watchdog

Edge cyber-physical security requires deterministic execution. If an updated candidate model introduces complex operator paths that push inference latency beyond **$1{,}000\,\mu\text{s}$ ($1.0\,\text{ms}$)**, it breaches line-rate mitigation SLAs.

`RollbackGuard` (`src/nexus/RollbackGuard.cpp`) continuously monitors heartbeat telemetry from Canary appliances, triggering an **instant, automated rollback** upon SLA violations.

---

## 1. Watchdog Enforcement Loop

```text
 [ Canary Edge Appliances (5% Cohort) ]
                   │
                   ▼ Telemetry Heartbeat Stream (Every 5 seconds)
 ┌─────────────────────────────────────────────────────────────┐
 │ RollbackGuard::evaluate_heartbeat()                         │
 │  - Reads: current_drop_latency_us                           │
 └─────────────────┬───────────────────────────────────────────┘
                   │
        ┌──────────┴──────────┐
        ▼ Latency <= 1000 µs  ▼ Latency > 1000 µs (SLA Breach!)
 ┌───────────────┐     ┌───────────────────────────────────────┐
 │ Nominal State │     │ EMERGENCY ROLLBACK TRIGGERED          │
 │ Canary Retained     │ • Dispatches AbortRollout command     │
 └───────────────┘     │ • Pushes network_threat_v1.onnx to 5% │
                       │ • Restores Sub-Microsecond Mitigation │
                       └───────────────────────────────────────┘
```

---

## 2. In-Engine Watchdog Implementation (`RollbackGuard.cpp`)

```cpp
#include <sentinel_nexus/RollbackGuard.hpp>

namespace sentinel::nexus {

constexpr double MAX_PERMITTED_LATENCY_US = 1000.0; // 1.0 ms SLA Ceiling

void RollbackGuard::evaluate_telemetry(const std::string& uuid, double reported_latency_us) {
    if (orchestrator_.current_stage() != DeploymentStage::CANARY_5_PCT) {
        return; // Watchdog active during Canary phase
    }

    if (reported_latency_us > MAX_PERMITTED_LATENCY_US) {
        consecutive_violations_++;

        if (consecutive_violations_ >= 3) { // 3 consecutive heartbeat breaches
            XINFER_LOG_CRITICAL(
                "RollbackGuard: Node {} reported {}µs latency (Breaches 1000µs SLA). TRIGGERING AUTOMATED ROLLBACK!",
                uuid, reported_latency_us
            );
            execute_emergency_rollback("LATENCY_SLA_BREACH");
        }
    } else {
        consecutive_violations_ = 0; // Reset consecutive counter on healthy tick
    }
}

void RollbackGuard::execute_emergency_rollback(std::string_view reason) {
    orchestrator_.transition_to(DeploymentStage::ROLLED_BACK);
    // Broadcast fallback rule: Command Canary cohort to hot-reload previous baseline model
    broadcaster_.broadcast_model_rollback(previous_verified_model_version_);
}

} // namespace sentinel::nexus
```
```

---

### File: `sentinel-nexus/docs/canary-ota-rollout/false-positive-surge-protection.md`

```markdown
# False-Positive Surge Protection & Drop Burst Aborts

A candidate model that passes the Golden Attacks Safety Gate may still trigger false positives on unexpected ambient traffic, leading to unauthorized drops on legitimate plant communication flows.

`sentinel-nexus` monitors collective drop volume to detect and abort **False-Positive Surges**.

---

## 1. Statistical Surge Detection Formula

`sentinel-nexus` maintains an Exponentially Weighted Moving Average (EWMA) of drop rates across the fleet:

$$\mu_{\text{baseline}}(t) = \alpha \cdot \text{Drops}_{\text{current}} + (1 - \alpha) \cdot \mu_{\text{baseline}}(t - 1), \quad \alpha = 0.05$$

$$\text{Surge Ratio} = \frac{\text{Drops}_{\text{Canary Cohort}}}{\mu_{\text{baseline}}}$$

```text
 Drop Rate (Drops/Sec)
  1000 ──┐                                                     ┌── FALSE POSITIVE SURGE!
         │                                                     │   Surge Ratio > 5.0x
   500 ──┼─────────────────────────────────────────────────────┼──────────────────────
         │                                                     │   Auto-Rollback Triggered
   100 ──┼─── Baseline EWMA μ = 85 drops/sec ──────────────────┘
     0 ──┴────────────────────────────────────────────────────────────────────────────► Time
```

---

## 2. Automated Circuit Interruption

If the active drop rate on Canary appliances spikes by more than **$5\times$ ($500\%$)** relative to the moving baseline:
1. `sentinel-nexus` aborts the Canary rollout immediately.
2. An automated instruction reverts Canary nodes to the previous baseline model within $50\,\text{ms}$.
3. The offending candidate weights are quarantined for active learning investigation.
```

---

### File: `sentinel-nexus/docs/canary-ota-rollout/model-repository-and-hashing.md`

```markdown
# Local Model Repository & Cryptographic Hashing

`sentinel-nexus` maintains a local, on-premises model repository in `/var/lib/sentinel-nexus/models/`. It serves verified ONNX models to edge appliances over authenticated HTTP streaming connections.

---

## 1. Repository Layout

```text
/var/lib/sentinel-nexus/models/
├── network_threat_v1.onnx               # Active Baseline Model
├── network_threat_v1.manifest.json       # SHA-256 & Verification Manifest
├── network_threat_v2_canary.onnx        # Candidate Model under Evaluation
├── network_threat_v2_canary.manifest.json
└── quarantine/                          # Quarantined models rejected by RollbackGuard
```

---

## 2. HTTP Streaming Download Endpoint (`GET /api/v1/models/{filename}`)

Edge appliances stream model files using chunked HTTP/2 transfers:

```text
Edge Appliance (blackbox-sentinel)                   Sentinel-Nexus Hub
       │                                                         │
       │ GET /api/v1/models/network_threat_v2.onnx               │
       │ Authorization: Bearer <APPLIANCE_JWT>                   │
       ├────────────────────────────────────────────────────────►│
       │                                                         │
       │◄────────────────────────────────────────────────────────┤
       │ 200 OK (Content-Type: application/octet-stream)         │
       │ X-Checksum-SHA256: e9a2c31e847b2c94b13a7b41e2...        │
       │ [Chunked Binary Stream: 7,412 Bytes]                    │
```

Before passing the model to `libxinfer.so`, the edge node computes the streaming SHA-256 hash. If the checksum does not match the `X-Checksum-SHA256` header, the file is deleted immediately.
```

---

### File: `sentinel-nexus/docs/canary-ota-rollout/zero-downtime-hot-reload-flow.md`

```markdown
# Zero-Downtime Atomic Model Hot-Reload Flow

In high-concurrency packet filtering, stopping the daemon or flushing in-kernel filter maps to update AI weights creates operational vulnerabilities.

`blackbox-sentinel` and `sentinel-nexus` coordinate **Zero-Downtime Atomic Hot-Reloads**.

---

## 1. Hot-Reload Sequence

```text
 [ Step 1: Download Complete & SHA-256 Verified on Edge Node ]
                              │
                              ▼ Local In-Memory Allocation
 ┌─────────────────────────────────────────────────────────────┐
 │ 1. xinfer::InferenceEngine loads candidate model into RAM   │
 │   - Compiles execution graph for target silicon (NPU/GPU)   │
 │   - Pins input/output scratchpad buffers                    │
 └────────────────────────────┬────────────────────────────────┘
                              │ Secondary Graph Ready
                              ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ 2. Atomic Pointer Swap (Zero Latency Penalty)               │
 │   - std::atomic<InferenceEngine*>::store(new_engine)        │
 │   - Active eBPF drop filters continue uninterrupted         │
 └────────────────────────────┬────────────────────────────────┘
                              │
                              ▼ Graceful Cleanup
 ┌─────────────────────────────────────────────────────────────┐
 │ 3. Drain and release memory of previous model version       │
 └─────────────────────────────────────────────────────────────┘
  TOTAL DOWNTIME: 0.00 Milliseconds (Zero Packet Loss)
```

---

## 2. Triggering Hot-Reloads via Command Line

Force an immediate edge model reload using `nexus-ctl`:

```bash
nexus-ctl ota reload --node edge-substation-alpha --version 2.4.0
```

### Verification
Query the edge node health status:

```bash
curl -k -s https://edge-substation-alpha:8443/api/v1/health | jq .active_model_sha256
# Expected Output: "e9a2c31e847b2c94b13a7b41e2d90100..."
```
```


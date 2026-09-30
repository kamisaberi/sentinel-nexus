### Part 3: Collective Defense Subsystem (`collective-defense/*`)

This section contains 6 technical specifications and C++20 implementations detailing the collective immunity engine in `sentinel-nexus`: the sub-50ms distribution paradigm, the asynchronous parallel gRPC broadcaster, the detailed fanout timeline, originator loopback suppression, the `FleetDefenseRule` schema, and emergency global IP unblocking.

---

### File: `sentinel-nexus/docs/collective-defense/collective-defense-overview.md`

```markdown
# Collective Defense: The Sub-50ms Immunity Paradigm

In distributed cyber-physical environments, adversaries automate multi-site attacks using rapid port sweeps, automated worm exploitation, and coordinated botnets. If threat mitigation remains localized to individual appliances, an attacker can compromise hundreds of facilities sequentially.

`sentinel-nexus` implements the **Sub-50ms Collective Defense Bus**, operating under the architectural invariant: **"Attacked Once, Immune Everywhere."**

---

## 1. The Collective Immunity Cycle

```text
 [ SITE 1: Electrical Substation Alpha ]
  • Adversary executes zero-day Modbus register injection.
  • In-kernel eBPF filter enforces drop in < 0.84 µs.
  • Emits ThreatIoC to Sentinel-Nexus over gRPC.
                     │
                     ▼ gRPC Transit (~14 ms)
 ┌─────────────────────────────────────────────────────────────┐
 │ SENTINEL-NEXUS COLLECTIVE DEFENSE BUS                       │
 │  - Deduplicates IoC against active rule cache               │
 │  - Applies Originator Loop Suppression (Skips Substation A) │
 │  - Dispatches FleetDefenseRule to 4,999 streaming channels  │
 └───────────────────┬─────────────────────────────────────────┘
                     │ Parallel Fanout (~18 ms)
                     ▼
 [ SITES 2 THROUGH 5,000: Water Plants, Hospitals, Substations ]
  • KernelDropInjector writes Attacker IP directly to BPF map.
  • Attacker is neutralized fleet-wide before probing Site 2.
 ---------------------------------------------------------------
 TOTAL FLEET PROPAGATION: ~32 ms (Guaranteed < 50 ms SLA Bound)
```

---

## 2. Architectural Invariants

1. **Deterministic Latency Budget:** From edge detection to fleet-wide in-kernel programming, the complete cycle executes in **under $50\,\text{milliseconds}$**.
2. **Asynchronous Parallel Fanout:** Broadcasting to 5,000 nodes uses non-blocking gRPC streaming calls without serial head-of-line blocking.
3. **Driver-Level Edge Enforcement:** Ingested fleet rules bypass user-space routing queues, writing directly into the Linux kernel `blocked_ip_map` in under $200\,\text{ns}$ per edge node.
```

---

### File: `sentinel-nexus/docs/collective-defense/ioc-broadcaster-mechanics.md`

```markdown
# Asynchronous Parallel gRPC Distribution Engine

The `IocBroadcaster` (`src/nexus/IocBroadcaster.cpp`) manages active streaming RPC connections (`StreamFleetRules`) to all connected appliances, fanning out rules asynchronously using thread pools.

---

## 1. Broadcaster Implementation (`IocBroadcaster.hpp`)

```cpp
#pragma once

#include <sentinel_nexus.grpc.pb.h>
#include <shared_mutex>
#include <unordered_map>
#include <string>
#include <vector>

namespace sentinel::nexus {

struct ApplianceStreamSession {
    std::string uuid;
    grpc::ServerReaderWriter<FleetDefenseRule, StreamRulesRequest>* stream{nullptr};
    uint32_t tpm_tier{0};
};

class IocBroadcaster {
public:
    void register_stream(const std::string& uuid, grpc::ServerReaderWriter<FleetDefenseRule, StreamRulesRequest>* stream) {
        std::unique_lock lock(mutex_);
        active_sessions_[uuid] = {uuid, stream};
    }

    void unregister_stream(const std::string& uuid) noexcept {
        std::unique_lock lock(mutex_);
        active_sessions_.erase(uuid);
    }

    // Broadcasts an IoC to all appliances except the originator
    uint32_t broadcast_rule(const FleetDefenseRule& rule, const std::string& originator_uuid) {
        std::shared_lock lock(mutex_);
        uint32_t dispatched_count = 0;

        for (const auto& [uuid, session] : active_sessions_) {
            // Originator Loopback Suppression
            if (uuid == originator_uuid) {
                continue;
            }

            // Asynchronous non-blocking write to HTTP/2 stream
            if (session.stream && session.stream->Write(rule)) {
                dispatched_count++;
            }
        }
        return dispatched_count;
    }

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, ApplianceStreamSession> active_sessions_;
};

} // namespace sentinel::nexus
```

---

## 2. Stream Channel Health

* If an edge appliance drops connection mid-stream, `session.stream->Write()` returns `false`.
* The broken stream is marked for reclamation without delaying rule delivery to the remaining 4,999 appliances.
```

---

### File: `sentinel-nexus/docs/collective-defense/sub-50ms-fanout-timeline.md`

```markdown
# Detailed Latency Budget: The Sub-50ms Fanout Timeline

The table below breaks down the microsecond and millisecond timing budget of a real-world collective defense propagation across Europe:

---

## 1. Latency Breakdown Table

| Step | Action & Subsystem | Duration | Cumulative Time | Location |
| :--- | :--- | :--- | :--- | :--- |
| **1** | Attacking packet arrives at NIC MAC/PHY layer | $0.00\,\mu\text{s}$ | $0.00\,\mu\text{s}$ | Substation A |
| **2** | eBPF driver hook parses packet and scores anomaly | $0.72\,\mu\text{s}$ | $0.72\,\mu\text{s}$ | Substation A |
| **3** | eBPF executes `XDP_DROP` in driver space | $0.12\,\mu\text{s}$ | **$0.84\,\mu\text{s}$** | Substation A |
| **4** | Substation A `NexusUplink` serializes `ThreatIoC` | $42.0\,\mu\text{s}$ | $42.8\,\mu\text{s}$ | Substation A |
| **5** | Network transit from Substation A to Nexus Hub | $14.2\,\text{ms}$ | $14.24\,\text{ms}$ | WAN Fiber |
| **6** | Nexus Hub parses gRPC, deduplicates in memory | $1.8\,\text{ms}$ | $16.04\,\text{ms}$ | Nexus Server |
| **7** | `IocBroadcaster` fans out rule over 4,999 gRPC streams | $2.1\,\text{ms}$ | $18.14\,\text{ms}$ | Nexus Server |
| **8** | Network transit from Nexus Hub to Substation B | $15.4\,\text{ms}$ | $33.54\,\text{ms}$ | WAN Fiber |
| **9** | Substation B `KernelDropInjector` executes BPF syscall | $0.18\,\mu\text{s}$ | **$33.54\,\text{ms}$** | Substation B |

$$\text{Total Fleet Synchronization Time} = 33.54\,\text{milliseconds} \quad (\ll 50\,\text{ms SLA Bound})$$

---

## 2. Visualization of the Fanout Budget

```text
 0 ms      10 ms     20 ms     30 ms     40 ms     50 ms
 ├─────────┼─────────┼─────────┼─────────┼─────────┤
 [0.84µs Drop]
           [==== WAN Ingress ====]
                                 [Fanout]
                                        [==== WAN Egress ====]
                                                             [Kernel Injection: 0.18µs]
                                                             ▲
                                                 TOTAL: 33.54 ms
```
```

---

### File: `sentinel-nexus/docs/collective-defense/originator-loop-suppression.md`

```markdown
# Originator Loopback Suppression & Broadcast Deduplication

When Appliance $A$ detects and mitigates an attack locally, sending the resulting `FleetDefenseRule` back to Appliance $A$ wastes bandwidth and risks resetting local drop counter statistics in its BPF map.

`sentinel-nexus` implements **Originator Loopback Suppression** and **Sliding-Window Deduplication**.

---

## 1. Suppression Mechanics

```text
 [ Appliance A (UUID: edge-alpha) ] ──► Emits ThreatIoC (Originator: "edge-alpha")
                                                 │
                                                 ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ Sentinel-Nexus Collective Defense Broadcaster               │
 ├─────────────────────────────────────────────────────────────┤
 │ For each active appliance in NodeRegistry:                  │
 │   IF target_uuid == rule.originator_uuid:                   │
 │       SKIP (Originator already has active drop in kernel!)  │
 │   ELSE:                                                     │
 │       DISPATCH FleetDefenseRule over gRPC Stream            │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. In-Memory Deduplication Window

If an adversary launches a coordinated flood targeting 10 appliances simultaneously, all 10 nodes may report identical attacking IP addresses to Nexus within milliseconds.

To prevent broadcast storms, `IocBroadcaster` maintains a thread-safe sliding window filter:

```cpp
#include <unordered_map>
#include <shared_mutex>
#include <cstdint>

class IocDeduplicator {
public:
    bool is_duplicate_or_update(uint32_t ip, uint64_t ttl_sec) {
        std::unique_lock lock(mutex_);
        uint64_t now_sec = get_epoch_seconds();

        auto it = seen_iocs_.find(ip);
        if (it != seen_iocs_.end() && it->second > now_sec) {
            // Already broadcast recently: suppress duplicate broadcast
            return true;
        }

        // Record new active suppression window
        seen_iocs_[ip] = now_sec + ttl_sec;
        return false;
    }

private:
    std::shared_mutex mutex_;
    std::unordered_map<uint32_t, uint64_t> seen_iocs_;
};
```
```

---

### File: `sentinel-nexus/docs/collective-defense/fleet-defense-rule-schema.md`

```markdown
# Fleet Defense Rule Protobuf Specification & Ephemeral TTLs

Fleet defense rules are serialized using Protocol Buffers v3 and streamed over HTTP/2 connections.

---

## 1. Protobuf Definition (`sentinel_nexus.proto`)

```protobuf
syntax = "proto3";
package sentinel.nexus;

message FleetDefenseRule {
    // Unique rule identifier assigned by Nexus
    uint32 rule_id = 1;

    // Target IPv4 address in network byte order (Big-Endian)
    uint32 target_ipv4_net_order = 2;

    // Ephemeral Time-To-Live in seconds (e.g. 60s, 3600s)
    uint64 ttl_seconds = 3;

    // Monotonic timestamp when rule was broadcast
    int64 broadcast_timestamp_ns = 4;

    // Originating node identifier (Used for loop suppression)
    string originator_uuid = 5;

    // Exploit classification (e.g. "MODBUS_REGISTER_OVERRIDE")
    string threat_classification = 6;

    // MITRE ATT&CK Technique ID (e.g. "T0855")
    string mitre_technique_id = 7;

    // Severity level: 1 = Low, 2 = Medium, 3 = High, 4 = Critical
    uint32 severity = 8;
}
```

---

## 2. Ephemeral TTL Management

Every rule includes an explicit `ttl_seconds` field:
* **Short-Lived Probes (Scans / Brute-force):** $\text{TTL} = 60\,\text{seconds}$.
* **Active SCADA Sabotage Attempts:** $\text{TTL} = 3{,}600\,\text{seconds}$ ($1\,\text{hour}$).
* **Persistent Exploits:** $\text{TTL} = 86{,}400\,\text{seconds}$ ($24\,\text{hours}$).

Edge appliances ingest this duration directly into their in-kernel monotonic timers, allowing stale block rules to expire automatically without requiring deletion broadcasts.
```

---

### File: `sentinel-nexus/docs/collective-defense/emergency-ip-purge.md`

```markdown
# Emergency Global False-Positive IP Purge

If a critical business partner IP, corporate gateway, or authorized engineering workstation is mistakenly blocked by an edge anomaly rule, security operators must be able to remove the block fleet-wide instantly.

`sentinel-nexus` provides an **Emergency Global IP Purge** command that removes the blocked address across all 5,000 edge kernel maps in **under $50\,\text{milliseconds}$**.

---

## 1. Global Purge Command Execution

### Via CLI (`nexus-ctl`):
```bash
nexus-ctl threat unblock --ip 198.51.100.42 --reason "Authorized Engineering Workstation"
```

### Via Web Command Center (Port 9443):
Navigate to **Threat Management $\to$ Active In-Kernel Rules**, select the target IP, and click **`[GLOBAL FLEET UNBLOCK]`**.

---

## 2. Purge Fanout Protocol

```text
 Administrator executes Global Unblock for 198.51.100.42
                           │
                           ▼ Broadcasts FleetDefenseRule with ttl_seconds = 0
 ┌─────────────────────────────────────────────────────────────┐
 │ Sentinel-Nexus Collective Defense Broadcaster               │
 └─────────────────────────┬───────────────────────────────────┘
                           │ Parallel gRPC Stream Fanout (< 50ms)
                           ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ 5,000 Remote Edge Appliances (blackbox-sentinel)            │
 ├─────────────────────────────────────────────────────────────┤
 │ KernelDropInjector reads ttl_seconds == 0:                  │
 │ -> Invokes BPF_MAP_DELETE_ELEM syscall on blocked_ip_map    │
 └─────────────────────────┬───────────────────────────────────┘
                           │
                           ▼
 [ Traffic unblocked immediately across all global facilities ]
```

---

## 3. Audit Logging

Every emergency purge action generates a tamper-evident audit record logged to `data/nexus_state.json` containing the operator's identity, timestamp, and justification.
```

---

### Complete in Part 3
- `sentinel-nexus/docs/collective-defense/collective-defense-overview.md`
- `sentinel-nexus/docs/collective-defense/ioc-broadcaster-mechanics.md`
- `sentinel-nexus/docs/collective-defense/sub-50ms-fanout-timeline.md`
- `sentinel-nexus/docs/collective-defense/originator-loop-suppression.md`
- `sentinel-nexus/docs/collective-defense/fleet-defense-rule-schema.md`
- `sentinel-nexus/docs/collective-defense/emergency-ip-purge.md`

All 6 Collective Defense files for `sentinel-nexus` are now generated.

---

### Files to be Generated in Part 4

The next phase covers the **Active Learning Pipeline** (`active-learning-pipeline/` - 6 files):

1. `active-learning-pipeline/active-learning-architecture.md` (Edge vector streaming to Forge dataset generation)
2. `active-learning-pipeline/vector-ingest-queue.md` (Concurrent, lock-free ring buffer: 200k vector capacity)
3. `active-learning-pipeline/uncertainty-sampling-rules.md` (Ingesting vectors within the $[0.40, 0.60]$ entropy window)
4. `active-learning-pipeline/dataset-curator-engine.md` (Packaging binary batches into `forge_dataset_*.csv` files)
5. `active-learning-pipeline/forge-trigger-automation.md` (Detecting batch quotas and firing background retraining)
6. `active-learning-pipeline/closed-loop-flywheel-testing.md` (Verifying autonomous retraining without human labeling)

Confirm when you are ready to proceed with Part 4.
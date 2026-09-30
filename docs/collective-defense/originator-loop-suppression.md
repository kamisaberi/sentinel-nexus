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


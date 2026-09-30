---

### File: `sentinel-nexus/docs/architecture/ports-and-protocols-matrix.md`

```markdown
# Network Ports, Protocols & Transport Routing Matrix

`sentinel-nexus` separates machine-to-machine fleet orchestration, administrative web interfaces, and real-time telemetry streaming onto discrete network ports to enforce access control boundaries.

---

## 1. Port Allocation Matrix

```text
               ┌────────────────────────────────────────────────────────┐
               │              SENTINEL-NEXUS NETWORK LISTENER           │
               └───────┬───────────────────┬────────────────────┬───────┘
                       │                   │                    │
                       ▼                   ▼                    ▼
                PORT 50051:         PORT 9443:           PORT 9444:
                gRPC (HTTP/2)       REST API & Web UI    Real-Time SSE Stream
                mTLS 1.3            HTTPS (TLS 1.3)      HTTP/1.1 EventStream
                       │                   │                    │
                       ▼                   ▼                    ▼
               Edge Appliances     Security Operators   Dashboard Canvas
               (blackbox-sentinel) & CI/CD Pipelines    (ws_client.js)
```

| Port | Transport Protocol | Application Protocol | Authentication Mechanism | Primary Consumers |
| :--- | :--- | :--- | :--- | :--- |
| **`50051`** | TCP / HTTP/2 | gRPC Protobuf | **Mutual TLS (mTLS 1.3)** with TPM-backed client certificates | Edge appliances (`blackbox-sentinel`) |
| **`9443`** | TCP / HTTPS | REST API & Web SPA | **Bearer JWT** (HMAC-SHA256 / Ed25519) | Web browsers, `nexus-ctl`, CI/CD pipelines |
| **`9444`** | TCP / HTTP | Server-Sent Events | Scoped Session Token / Local Loopback | Local dashboard canvas, internal bridges |
| **`443`** | TCP / HTTPS (Outbound)| Aryorithm Cloud REST | Bearer JWT (Renewed every 1 hour) | Aryorithm SaaS Cloud (`app.aryorithm.com`) |

---

## 2. Inbound Service Multiplexing

* **Port 50051 (Fleet RPC):** Rejects any connection that fails client certificate verification. Unauthenticated probes or HTTP/1.1 requests are terminated at the TLS handshake.
* **Port 9443 (Management Console):** Serves the embedded web application and REST endpoints. Strict Cross-Origin Resource Sharing (CORS) rules prevent cross-site scripting vulnerabilities.
* **Port 9444 (Telemetry Stream):** Maintains open, long-lived HTTP/1.1 connections streaming `text/event-stream` payloads without proxy buffering headers (`X-Accel-Buffering: no`).
```

---

### File: `sentinel-nexus/docs/architecture/in-memory-state-engine.md`

```markdown
# In-Memory State Engine: `NodeRegistry` & Synchronization

Managing the operational states, heartbeat counters, active IP block tables, and sensor inventories of 5,000 edge appliances requires microsecond state access. `sentinel-nexus` maintains all fleet state in a centralized, in-memory **`NodeRegistry`**.

---

## 1. Read-Heavy Synchronization: `std::shared_mutex`

In fleet orchestration, state reads (telemetry lookups, web dashboard requests, rule fanout routing) occur thousands of times per second, while state mutations (node registrations, status transitions) occur infrequently.

`NodeRegistry` uses **Reader-Writer Locks** (`std::shared_mutex`):

```text
 Concurrent Read Requests (Web UI, SSE Stream, Rule Fanout)
        │                   │                   │
        ▼ shared_lock       ▼ shared_lock       ▼ shared_lock
 ┌─────────────────────────────────────────────────────────────┐
 │ NodeRegistry::nodes_ (std::unordered_map<UUID, NodeState>)  │
 └──────────────────────────────▲──────────────────────────────┘
                                │ unique_lock (Exclusive Access)
                  Node Heartbeat Mutation / Register
```

---

## 2. Implementation: `NodeRegistry.hpp`

```cpp
#pragma once

#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <optional>
#include <vector>

namespace sentinel::nexus {

struct NodeState {
    std::string uuid;
    std::string hostname;
    std::string ip_address;
    std::string agent_version;
    uint32_t tpm_tier{0};
    bool is_online{false};
    uint64_t last_heartbeat_ns{0};
    uint64_t total_drops{0};
    double last_drop_latency_us{0.0};
    std::vector<std::string> active_sensors;
};

class NodeRegistry {
public:
    // Read Operations: Concurrent readers permitted
    std::optional<NodeState> get_node(const std::string& uuid) const {
        std::shared_lock lock(mutex_);
        auto it = nodes_.find(uuid);
        if (it != nodes_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::vector<NodeState> get_all_nodes() const {
        std::shared_lock lock(mutex_);
        std::vector<NodeState> result;
        result.reserve(nodes_.size());
        for (const auto& [_, node] : nodes_) {
            result.push_back(node);
        }
        return result;
    }

    // Write Operations: Exclusive writer lock
    void update_heartbeat(const std::string& uuid, uint64_t drops, double latency_us) {
        std::unique_lock lock(mutex_);
        auto it = nodes_.find(uuid);
        if (it != nodes_.end()) {
            it->second.last_heartbeat_ns = get_monotonic_ns();
            it->second.total_drops = drops;
            it->second.last_drop_latency_us = latency_us;
            it->second.is_online = true;
        }
    }

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, NodeState> nodes_;
};

} // namespace sentinel::nexus
```

---

## 3. Scale Metrics

* **Lookup Latency:** $< 120\,\text{ns}$ per node lookup across a 5,000-node registry.
* **Memory Overhead:** $< 25\,\text{MB}$ total RAM required to hold full metadata and sensor inventories for 5,000 appliances.
```

---

### File: `sentinel-nexus/docs/architecture/data-persistence-model.md`

```markdown
# Data Persistence Model: `StateDatabase` & `TimeSeriesEngine`

While `sentinel-nexus` executes primarily out of RAM, it maintains continuous state persistence across process restarts using an append-only JSON journal (`StateDatabase`) and an in-memory ring-buffer time-series database (`TimeSeriesEngine`).

---

## 1. Atomic Journaling (`nexus_state.json`)

To prevent file corruption during sudden power outages:
1. State is serialized to a temporary staging file: `data/nexus_state.json.tmp`.
2. The file is flushed to physical storage using `fsync()`.
3. An atomic POSIX `rename()` replaces the active database (`data/nexus_state.json`).

```text
 [ State Change Event ] ──► Write to nexus_state.json.tmp ──► fsync() ──► rename()
                                                                               │
                                                                               ▼
                                                             [ Active nexus_state.json ]
```

---

## 2. In-Memory Time-Series Engine (`TimeSeriesEngine.cpp`)

Metrics displayed on the Web Command Center (e.g., 24-hour fleet drop trends, latency distributions) are maintained in circular memory rings:

```cpp
#include <array>
#include <atomic>
#include <cstdint>

namespace sentinel::nexus {

struct MetricSample {
    uint64_t timestamp_sec;
    uint64_t total_drops;
    double fleet_latency_us;
    uint32_t active_nodes;
};

class TimeSeriesEngine {
public:
    static constexpr size_t RING_SIZE = 86400; // 24 hours at 1Hz resolution

    void record_sample(const MetricSample& sample) noexcept {
        size_t idx = cursor_.fetch_add(1, std::memory_order_relaxed) % RING_SIZE;
        samples_[idx] = sample;
    }

    std::vector<MetricSample> get_history(size_t seconds) const {
        // Extracts contiguous history window without heap reallocation
        std::vector<MetricSample> history;
        history.reserve(seconds);
        // ...
        return history;
    }

private:
    std::array<MetricSample, RING_SIZE> samples_{};
    std::atomic<size_t> cursor_{0};
};

} // namespace sentinel::nexus
```
```

---

### File: `sentinel-nexus/docs/architecture/air-gapped-sovereignty.md`

```markdown
# Air-Gapped Data Sovereignty & $0.00 Cloud Egress

`sentinel-nexus` is architected for complete air-gapped sovereignty. In municipal water utilities, nuclear power stations, and defense operations centers, the platform functions autonomously without WAN connectivity.

---

## 1. Sovereign On-Premises Topology

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ AIR-GAPPED INDUSTRIAL FACILITY                              │
 │                                                             │
 │  ┌───────────────────────┐        ┌───────────────────────┐ │
 │  │ blackbox-sentinel (1) │        │ blackbox-sentinel (N) │ │
 │  └───────────┬───────────┘        └───────────┬───────────┘ │
 │              │ Internal Network (10.240.0.0/24)│            │
 │              └────────────────┬───────────────┘             │
 │                               ▼                             │
 │               ┌───────────────────────────────┐             │
 │               │ sentinel-nexus Fleet Hub      │             │
 │               │ • Port 50051: Fleet gRPC      │             │
 │               │ • Port 9443: Web Console      │             │
 │               │ • Port 9444: Real-Time SSE    │             │
 │               │ • 100% On-Premises Execution  │             │
 │               └───────────────────────────────┘             │
 └─────────────────────────────────────────────────────────────┘
                                 ║
                                 ╫ PHYSICAL AIR-GAP (NO INTERNET ROUTE)
                                 ║
                         [ PUBLIC CLOUD ]
```

---

## 2. Data Sovereignty Guarantees

* **Zero Telemetry Leakage:** Ingress network flows, raw PCAP files, and asset metadata remain inside the customer's on-premises boundary.
* **$0.00 Cloud Egress Fees:** Fleet coordination, active learning curation, and Canary model rollouts execute entirely over local Ethernet/fiber links.
* **Zero External Dependencies:** The web command center embeds all HTML, CSS, JavaScript, and SVG vector graphics directly in the C++ binary—making zero outbound requests to external CDNs.
```

---

### File: `sentinel-nexus/docs/architecture/high-availability-clustering.md`

```markdown
# High-Availability Clustering & Failover Topology

In mission-critical infrastructure, a single point of failure in the fleet command plane is unacceptable. `sentinel-nexus` supports **Active-Standby High-Availability (HA) Clustering** managed via a Virtual IP (VIP) and state replication.

---

## 1. Active-Standby Failover Architecture

```text
                           [ Virtual IP (VIP): 10.240.0.10 ]
                                         │
                 ┌───────────────────────┴───────────────────────┐
                 ▼ (Active Node)                                 ▼ (Standby Node)
 ┌───────────────────────────────┐               ┌───────────────────────────────┐
 │ sentinel-nexus Primary        │               │ sentinel-nexus Secondary      │
 │ • State: LEADER (Active)      │               │ • State: FOLLOWER (Warm)      │
 │ • Holds VIP (10.240.0.10)     │               │ • Monitors Primary Heartbeat  │
 └───────────────┬───────────────┘               └───────────────▲───────────────┘
                 │                                               │
                 └──────── Synchronous State Journaling ─────────┘
                   (nexus_state.json synced over private link)
```

---

## 2. Failover Protocol Sequence

1. **Health Monitoring:** The secondary Nexus instance polls the primary instance over a dedicated heartbeat link every $500\,\text{ms}$.
2. **Failure Detection:** If the primary instance fails to respond for $1{,}500\,\text{ms}$ (3 missed heartbeats):
   * The secondary instance promotes itself to **`LEADER`**.
   * The secondary instance issues an **Arp Gratuitous** broadcast claiming the Virtual IP (`10.240.0.10`).
3. **Seamless Appliance Reconnection:** All 5,000 edge appliances (`blackbox-sentinel`) reconnect to the VIP within $2.0\text{ seconds}$ without configuration changes.
```


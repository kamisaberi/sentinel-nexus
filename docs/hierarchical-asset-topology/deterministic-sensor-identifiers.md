---

### File: `sentinel-nexus/docs/hierarchical-asset-topology/deterministic-sensor-identifiers.md`

```markdown
# Deterministic Sensor & Industrial Asset Identification

Industrial automation networks often feature unmanaged legacy devices lacking hostname registration, DHCP leases, or SNMP agents. 

`sentinel-nexus` derives **Deterministic Sensor Identifiers** using physical network coordinates and protocol attributes.

---

## 1. Deterministic Identifier Derivation Formula

$$\text{SensorID} = \text{Protocol} \parallel \text{"-"} \parallel \text{IPv4} \parallel \text{"-"} \parallel \text{Port} \parallel \text{"-"} \parallel \text{UnitAddress}$$

```text
 Modbus TCP Example:
  • Protocol: "modbus"
  • Ingress IP: "10.240.0.101"
  • Port: "502"
  • Unit ID: "u1"
  ──► Resulting Sensor ID: "modbus-10.240.0.101-502-u1"

 Siemens S7Comm Example:
  • Protocol: "s7"
  • Ingress IP: "10.240.0.102"
  • Port: "102"
  • Rack/Slot: "r0-s2"
  ──► Resulting Sensor ID: "s7-10.240.0.102-102-r0-s2"
```

---

## 2. C++ Identifier Generator (`AssetTopology.hpp`)

```cpp
#pragma once

#include <string>
#include <sstream>
#include <cstdint>

namespace sentinel::nexus {

class SensorIdentifierGenerator {
public:
    static std::string generate_modbus_id(uint32_t ip, uint16_t port, uint8_t unit_id) {
        std::ostringstream ss;
        ss << "modbus-"
           << (ip & 0xFF) << "." << ((ip >> 8) & 0xFF) << "."
           << ((ip >> 16) & 0xFF) << "." << ((ip >> 24) & 0xFF)
           << "-" << port << "-u" << static_cast<int>(unit_id);
        return ss.str();
    }

    static std::string generate_s7_id(uint32_t ip, uint16_t port, uint8_t rack, uint8_t slot) {
        std::ostringstream ss;
        ss << "s7-"
           << (ip & 0xFF) << "." << ((ip >> 8) & 0xFF) << "."
           << ((ip >> 16) & 0xFF) << "." << ((ip >> 24) & 0xFF)
           << "-" << port << "-r" << static_cast<int>(rack) << "-s" << static_cast<int>(slot);
        return ss.str();
    }
};

} // namespace sentinel::nexus
```
```

---

### File: `sentinel-nexus/docs/hierarchical-asset-topology/cascading-health-engine.md`

```markdown
# Cascading Health Engine & State Machine

`sentinel-nexus` maintains active operational health states for all entities across the 4-tier asset tree. Health state transitions propagate hierarchically to reflect operational disruptions accurately.

---

## 1. The Four Health States

```text
 ┌───────────────┐
 │    ONLINE     │ Node and connected sensors operating within nominal bounds.
 └───────┬───────┘
         │ eBPF drop rate surge (> 20%) OR Subsystem enters DEGRADED mode
         ▼
 ┌───────────────┐
 │   DEGRADED    │ Traffic passes; mitigation active; potential anomaly under review.
 └───────┬───────┘
         │ Node heartbeats cease for > 15 seconds
         ▼
 ┌───────────────┐
 │  UNREACHABLE  │ Appliance stopped communicating; network link suspected down.
 └───────┬───────┘
         │ Operator clean shutdown received (DeregisterAppliance RPC)
         ▼
 ┌───────────────┐
 │    OFFLINE    │ Graceful shutdown confirmed; phantom alerts suppressed.
 └───────────────┘
```

---

## 2. Cascading Tree Propagation Rules

* **Parent Degradation:** If an edge appliance node enters `DEGRADED`, all attached Tier 4 sensors are marked with a warning badge on the Web Command Center.
* **Parent Unreachability:** If an appliance transitions to `UNREACHABLE` or `OFFLINE`, all subordinate sensors transition to `INHERITED_OFFLINE` automatically.
* **Automatic Recovery:** When the appliance resumes valid mTLS heartbeats on port 50051, the entire sub-tree transitions back to `ONLINE` within a single polling interval ($5.0\text{ seconds}$).
```

---

### File: `sentinel-nexus/docs/hierarchical-asset-topology/liveness-heartbeat-tracking.md`

```markdown
# Liveness Heartbeat Tracking & Timeout Engine

`sentinel-nexus` tracks appliance liveness using non-blocking monotonic timestamp comparisons across the in-memory `NodeRegistry`.

---

## 1. Heartbeat Window Timeline

```text
 t = 0s         t = 5s         t = 10s        t = 15s (Timeout Threshold)
 ├── Heartbeat ──┼── Heartbeat ──┼── Missed ────┼── DEADLINE EXPIRED
 [   ONLINE   ]  [   ONLINE   ]  [   GRACE   ]  [ TRANSITION: UNREACHABLE ]
```

* **Heartbeat Period:** Edge nodes report every $5.0\text{ seconds}$.
* **Grace Period Window:** $15.0\text{ seconds}$ (3 consecutive missed heartbeats).
* **State Transition:** If $t_{\text{current}} - t_{\text{last\_heartbeat}} > 15.0\,\text{s}$, the node status changes from `ONLINE` to `UNREACHABLE`.

---

## 2. In-Engine Liveness Sweeper (`LivenessTracker.cpp`)

```cpp
#include <sentinel_nexus/NodeRegistry.hpp>
#include <chrono>

namespace sentinel::nexus {

constexpr uint64_t LIVENESS_TIMEOUT_NS = 15'000'000'000ULL; // 15 Seconds

void sweep_appliance_liveness(NodeRegistry& registry) {
    uint64_t now_ns = get_monotonic_ns();
    auto all_nodes = registry.get_all_nodes();

    for (const auto& node : all_nodes) {
        if (node.is_online) {
            uint64_t delta = now_ns - node.last_heartbeat_ns;
            
            if (delta > LIVENESS_TIMEOUT_NS) {
                XINFER_LOG_WARN("LivenessTracker: Node {} missed heartbeats (delta: {}ms). Marking UNREACHABLE.",
                    node.uuid, delta / 1'000'000);
                registry.mark_node_unreachable(node.uuid);
            }
        }
    }
}

} // namespace sentinel::nexus
```
```

---

### File: `sentinel-nexus/docs/hierarchical-asset-topology/instant-0ms-graceful-disconnect.md`

```markdown
# Instant 0ms Graceful Disconnect Handling

When an edge appliance shuts down normally (via `systemctl stop sentinel`, system reboot, or `SIGINT`), waiting for the 15-second heartbeat timeout generates false "Node Lost" alarms in enterprise SOCs.

`sentinel-nexus` processes synchronous **`DeregisterAppliance`** requests, transitioning the appliance to `OFFLINE` in **under 5 milliseconds**.

---

## 1. RPC Deregistration Flow

```text
 Edge Appliance (SIGINT Intercepted)                   Sentinel-Nexus Hub
       │                                                         │
       │ DeregisterAppliance(appliance_uuid, GRACEFUL_SHUTDOWN)  │
       ├────────────────────────────────────────────────────────►│
       │                                                         │
       │                                            ┌────────────┴────────────┐
       │                                            │ 1. Mark node OFFLINE    │
       │                                            │ 2. Suppress SOC alerts  │
       │                                            │ 3. Close gRPC stream    │
       │                                            └────────────┬────────────┘
       │                                                         │
       │ 200 OK Response (Deregistration Acknowledged)           │
       │◄────────────────────────────────────────────────────────┤
       │
 [ Process Exits Cleanly ]
```

---

## 2. Server-Side gRPC Implementation (`FleetServiceImpl.cpp`)

```cpp
grpc::Status FleetServiceImpl::DeregisterAppliance(
    grpc::ServerContext* context,
    const DeregisterRequest* request,
    DeregisterResponse* response
) {
    const std::string& uuid = request->appliance_uuid();

    // 1. Authenticate calling appliance token
    if (!validate_jwt_context(context)) {
        return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid token");
    }

    // 2. Transition state immediately to OFFLINE
    node_registry_.mark_node_offline(uuid, request->reason());

    // 3. Update broad-spectrum SSE stream (Port 9444)
    sse_broadcaster_.push_node_offline_event(uuid);

    XINFER_LOG_INFO("FleetService: Appliance {} disconnected gracefully (0ms delay).", uuid);
    response->set_success(true);
    return grpc::Status::OK;
}
```
```

---

### File: `sentinel-nexus/docs/hierarchical-asset-topology/fleet-sync-payload-schema.md`

```markdown
# Fleet Sync Payload Schema (`POST /api/v1/fleet/sync`)

Every 5 seconds, the `SaaSConnector` subsystem on `sentinel-nexus` serializes the active 4-tier asset tree into a nested JSON structure and transmits it to the central cloud platform (`app.aryorithm.com`).

---

## 1. JSON Payload Specification

```json
{
  "tenant_id": "tenant-municipal-utility-bavaria",
  "nexus_id": "nexus-central-munich",
  "sync_timestamp_ns": 1791172800184000000,
  "nodes": [
    {
      "node_id": "edge-substation-alpha",
      "hostname": "substation-01.internal",
      "ip_address": "10.240.0.101",
      "agent_version": "2.4.0",
      "status": "ONLINE",
      "tpm_tier": "TIER1_PHYSICAL_TPM",
      "cpu_usage_pct": 4.2,
      "memory_used_mb": 1420,
      "total_drops_today": 1420,
      "last_latency_us": 0.82,
      "sensors": [
        {
          "sensor_id": "modbus-10.240.0.101-502-u1",
          "protocol": "MODBUS_TCP",
          "ip_address": "10.240.0.101",
          "port": 502,
          "status": "ONLINE",
          "anomalies_detected": 0
        },
        {
          "sensor_id": "s7-10.240.0.102-102-r0-s2",
          "protocol": "S7COMM",
          "ip_address": "10.240.0.102",
          "port": 102,
          "status": "ONLINE",
          "anomalies_detected": 1
        }
      ]
    }
  ]
}
```

---

## 2. Invariants

* **Compression:** Large fleet sync payloads ($> 500\text{ nodes}$) are compressed with `gzip` on transmission (`Content-Encoding: gzip`).
* **Non-Blocking Egress:** The sync payload is generated from read-only `NodeRegistry` snapshots, ensuring edge packet drops and local telemetry processing continue uninterrupted.
```


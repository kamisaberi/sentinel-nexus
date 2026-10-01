---

### File: `sentinel-nexus/docs/explainable-ai-xai/global-threat-cache-indexing.md`

```markdown
# Global Threat Cache Indexing & MITRE ATT&CK Taxonomy

`sentinel-nexus` indexes incoming XAI feature attributions alongside the **MITRE ATT&CK for Enterprise and ICS** taxonomies, correlating raw network deviations with adversarial tactics, techniques, and procedures (TTPs).

---

## 1. Indexing Taxonomy Pipeline

```text
 Ingress XAI Record: Top Attribution = "MODBUS_REGISTER_SETPOINT_40001"
                           │
                           ▼ Rule Mapping Table
 ┌─────────────────────────────────────────────────────────────┐
 │ MITRE ATT&CK for ICS Taxonomy Alignment                     │
 ├─────────────────────────────────────────────────────────────┤
 │ Tactic    : TA0108 (Impair Process Control)                 │
 │ Technique : T0855 (Unauthorized Command Message)            │
 │ Severity  : CRITICAL (Level 4)                              │
 └─────────────────────────┬───────────────────────────────────┘
                           │
                           ▼ Update In-Memory Index
 ┌─────────────────────────────────────────────────────────────┐
 │ Global Threat Cache (Thread-Safe Bitmap Postings)           │
 │  • Index by MITRE ID   : "T0855" -> [Inc-1802, Inc-1805]    │
 │  • Index by Attacker IP: 198.51.100.42 -> [Inc-1802]        │
 │  • Index by Asset ID   : "plc-m340" -> [Inc-1802]           │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. In-Memory Threat Cache (`GlobalThreatCache.hpp`)

```cpp
#pragma once

#include <shared_mutex>
#include <unordered_map>
#include <string>
#include <vector>

namespace sentinel::nexus {

struct IndexedThreatEvent {
    std::string incident_id;
    uint32_t src_ip;
    std::string mitre_technique_id;
    double anomaly_score;
    std::string top_feature;
    uint64_t timestamp_ns;
};

class GlobalThreatCache {
public:
    void insert_event(const IndexedThreatEvent& event) {
        std::unique_lock lock(mutex_);
        events_[event.incident_id] = event;
        mitre_index_[event.mitre_technique_id].push_back(event.incident_id);
    }

    std::vector<IndexedThreatEvent> get_by_mitre(const std::string& mitre_id) const {
        std::shared_lock lock(mutex_);
        std::vector<IndexedThreatEvent> results;
        auto it = mitre_index_.find(mitre_id);
        if (it != mitre_index_.end()) {
            for (const auto& id : it->second) {
                results.push_back(events_.at(id));
            }
        }
        return results;
    }

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, IndexedThreatEvent> events_;
    std::unordered_map<std::string, std::vector<std::string>> mitre_index_;
};

} // namespace sentinel::nexus
```
```

---

### File: `sentinel-nexus/docs/explainable-ai-xai/xai-api-endpoints.md`

```markdown
# Auditor REST Endpoints: Querying Explainable Telemetry

Compliance officers and security analysts query physical feature deviations via the `sentinel-nexus` REST API on port **9443**.

---

## 1. Endpoint: `GET /api/v1/threats/xai`

Returns a paginated list of historical threat mitigations with full top-3 feature attributions.

### Query Parameters
* `limit` (optional, integer, default: 50): Number of records to return.
* `mitre_id` (optional, string): Filter by MITRE technique (e.g. `T0855`).
* `since_ns` (optional, integer): Monotonic epoch timestamp filter.

### Request:
```bash
curl -k -H "Authorization: Bearer $JWT" \
    https://localhost:9443/api/v1/threats/xai?limit=1&mitre_id=T0855
```

### Response (`200 OK`):
```json
{
  "total_records": 1,
  "incidents": [
    {
      "incident_id": "inc-1802-8f1c2a04",
      "timestamp_ns": 1791172800184000000,
      "source_ip": "198.51.100.42",
      "destination_ip": "10.240.0.101",
      "mitre_technique_id": "T0855",
      "mitigation_action": "XDP_DROP",
      "mitigation_latency_us": 0.82,
      "top_attributions": [
        {
          "rank": 1,
          "feature_name": "MODBUS_REGISTER_SETPOINT_40001",
          "contribution_percentage": 64.2,
          "observed": 9850.0,
          "baseline": 2100.0,
          "unit": "PSI"
        },
        {
          "rank": 2,
          "feature_name": "FLOW_PACKETS_PER_SECOND",
          "contribution_percentage": 23.8,
          "observed": 82000.0,
          "baseline": 150.0,
          "unit": "pps"
        },
        {
          "rank": 3,
          "feature_name": "INTER_ARRIVAL_TIME_MEAN",
          "contribution_percentage": 12.0,
          "observed": 0.000012,
          "baseline": 0.012500,
          "unit": "s"
        }
      ]
    }
  ]
}
```

---

## 2. Endpoint: `GET /api/v1/threats/xai/{incident_id}`

Retrieves the raw high-resolution residual vector for a single incident.
```

---

### File: `sentinel-nexus/docs/explainable-ai-xai/rendering-xai-in-web-and-tui.md`

```markdown
# Live XAI Visualization: Web Command Center & Terminal TUI

`sentinel-nexus` renders Microsecond Residual Decomposition (MRD) attributions live across both the **Web Command Center (Port 9443)** and the **Terminal TUI (`nexus-tui`)**.

---

## 1. Web Command Center Rendering (HTML5 Canvas & SVG)

The Web Command Center visualizes feature deviations using SVG impact bars:

```text
 ┌─────────────────────────────────────────────────────────────────────────────┐
 │ INCIDENT #1802 ATTRIBUTION BREAKDOWN                                       │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ Threat: MODBUS REGISTER TAMPER (T0855) | Score: 0.284 | In-Kernel Drop: OK  │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ [1] Modbus Setpoint 40001 (64.2%)                                           │
 │     Observed: 9,850 PSI | Normal: 2,100 PSI (Residual: +7,750 PSI)          │
 │     Impact: ████████████████████████████████                                │
 │                                                                             │
 │ [2] Flow Packet Rate (23.8%)                                                │
 │     Observed: 82,000 pps | Normal: 150 pps                                  │
 │     Impact: ████████████                                                    │
 │                                                                             │
 │ [3] Packet Inter-Arrival Jitter (12.0%)                                     │
 │     Observed: 0.001 ms | Normal: 12.5 ms                                    │
 │     Impact: ██████                                                          │
 └─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Terminal TUI Rendering (`make tui`)

In air-gapped server environments without desktop web browsers, system operators run the dual-panel terminal TUI:

```text
 ┌─ Sentinel-Nexus TUI v2.4 ─────────────────────────────── Top Attributions ─┐
 │ Node                 Status   Drops   SLA      │ Rank 1: MODBUS_REG_40001  │
 │ edge-substation-01   ONLINE   1,420   0.82 µs  │  Delta: +7,750 PSI (64%)  │
 │ edge-substation-02   ONLINE     840   0.81 µs  │ Rank 2: FLOW_PPS          │
 │ edge-substation-03   ONLINE      12   0.84 µs  │  Delta: +81,850 pps (24%) │
 │ > edge-water-plant   ONLINE   4,129   0.82 µs  │ Rank 3: IAT_MEAN          │
 └────────────────────────────────────────────────┴───────────────────────────┘
```
```


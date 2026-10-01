---

### File: `sentinel-nexus/docs/explainable-ai-xai/top-3-feature-attribution-schema.md`

```markdown
# Top-3 Feature Attribution Schema Specification

Every mitigation event reported to `sentinel-nexus` contains a structured **Top-3 Feature Attribution** payload describing the primary mathematical drivers of the anomaly.

---

## 1. JSON Schema Definition

```json
{
  "incident_id": "inc-1802-8f1c2a04",
  "appliance_uuid": "edge-substation-alpha",
  "timestamp_ns": 1791172800184000000,
  "mitre_technique_id": "T0855",
  "anomaly_score": 0.2842,
  "anomaly_threshold": 0.0820,
  "top_attributions": [
    {
      "rank": 1,
      "feature_index": 0,
      "feature_name": "MODBUS_REGISTER_SETPOINT_40001",
      "contribution_percentage": 64.2,
      "observed_value": 9850.0,
      "baseline_value": 2100.0,
      "residual_delta": "+7750.0 PSI",
      "semantic_description": "Safety-critical gas relief valve setpoint exceeded maximum physical limit (4500 PSI)."
    },
    {
      "rank": 2,
      "feature_index": 20,
      "feature_name": "FLOW_PACKETS_PER_SECOND",
      "contribution_percentage": 23.8,
      "observed_value": 82000.0,
      "baseline_value": 150.0,
      "residual_delta": "+81850.0 pps",
      "semantic_description": "Severe packet transmission rate burst indicative of automated command flooding."
    },
    {
      "rank": 3,
      "feature_index": 16,
      "feature_name": "INTER_ARRIVAL_TIME_MEAN",
      "contribution_percentage": 12.0,
      "observed_value": 0.000012,
      "baseline_value": 0.012500,
      "residual_delta": "-0.012488 s",
      "semantic_description": "Loss of normal cyclic polling jitter; automated script injection profile detected."
    }
  ]
}
```

---

## 2. Field Specifications

| Property | Type | Description |
| :--- | :--- | :--- |
| `rank` | `uint32_t` | Contribution ranking: `1` (Highest driver) to `3`. |
| `feature_index` | `uint32_t` | Tensor dimension index ($0$ through $31$). |
| `contribution_percentage`| `double` | Mathematical percentage of the total residual error: $\frac{e_j}{\sum e_k} \times 100$. |
| `observed_value` | `double` | Unscaled physical value observed on the wire. |
| `baseline_value` | `double` | Unscaled value predicted by the autoencoder baseline. |
| `residual_delta` | `string` | Formatted physical delta with dimensional engineering units. |
```

---

### File: `sentinel-nexus/docs/explainable-ai-xai/semantic-dictionary-mapping.md`

```markdown
# Semantic Dictionary Mapping: Tensor Coordinates to Physical Units

In raw neural tensor processing, an anomaly is represented as a deviation on abstract index `j`. The **Semantic Dictionary** (`src/xai/SemanticDictionary.hpp`) translates continuous tensor indices into human-readable physical parameters.

---

## 1. 32-Dimensional NetFlow/SCADA Mapping Dictionary

```text
 Index  Feature Identifier               Physical Engineering Units
 ─────  ───────────────────────────────  ──────────────────────────
   0    FLOW_DURATION                    Seconds (s)
   1    TOTAL_PACKETS_SRC_TO_DST         Packets (count)
   2    TOTAL_PACKETS_DST_TO_SRC         Packets (count)
   3    TOTAL_BYTES_SRC_TO_DST           Bytes (B)
   4    TOTAL_BYTES_DST_TO_SRC           Bytes (B)
   5    PACKET_LENGTH_MIN                Bytes (B)
   6    PACKET_LENGTH_MAX                Bytes (B)
   7    PACKET_LENGTH_MEAN               Bytes (B)
   8    PACKET_LENGTH_STDDEV             Bytes (B)
   9    TCP_WINDOW_BYTES_SRC             Bytes (B)
  10    TCP_WINDOW_BYTES_DST             Bytes (B)
  11    HEADER_LENGTH_SRC                Bytes (B)
  12    HEADER_LENGTH_DST                Bytes (B)
  13    AVG_SEGMENT_SIZE_SRC             Bytes (B)
  14    AVG_SEGMENT_SIZE_DST             Bytes (B)
  15    TCP_INITIAL_RTT                  Microseconds (µs)
  16    INTER_ARRIVAL_TIME_MEAN          Seconds (s)
  17    INTER_ARRIVAL_TIME_STDDEV        Seconds (s)
  18    FLOW_ACTIVE_TIME_MEAN            Seconds (s)
  19    FLOW_IDLE_TIME_MEAN              Seconds (s)
  20    FLOW_PACKETS_PER_SECOND          Packets per Second (pps)
  21    FLOW_BYTES_PER_SECOND            Bytes per Second (Bps)
  22    TCP_FLAG_SYN                     Binary Flag (0/1)
  23    TCP_FLAG_RST                     Binary Flag (0/1)
  24    TCP_FLAG_PSH                     Binary Flag (0/1)
  25    TCP_FLAG_ACK                     Binary Flag (0/1)
  26    TCP_FLAG_URG                     Binary Flag (0/1)
  27    TCP_FLAG_ECE                     Binary Flag (0/1)
  28    DOWN_UP_RATIO                    Scalar Ratio
  29    AVG_PACKET_SIZE                  Bytes (B)
  30    PROTOCOL_ENCODING                Protocol (6=TCP, 17=UDP)
  31    DESTINATION_PORT                 Port Number (1-65535)
```

---

## 2. In-Engine Un-Scaling Implementation (`SemanticDictionary.hpp`)

```cpp
#pragma once

#include <string_view>
#include <array>
#include <cmath>

namespace sentinel::nexus {

struct FeatureSemanticDescriptor {
    std::string_view identifier;
    std::string_view unit;
    double scale_factor;
    bool is_log_scaled;
};

class SemanticDictionary {
public:
    static double unscale_feature(uint32_t index, float normalized_val) noexcept {
        if (index >= DICTIONARY.size()) return static_cast<double>(normalized_val);
        const auto& desc = DICTIONARY[index];
        
        if (desc.is_log_scaled) {
            // Invert log1p: x = exp(norm) - 1.0
            return std::expm1(static_cast<double>(normalized_val));
        }
        return static_cast<double>(normalized_val) * desc.scale_factor;
    }

private:
    static constexpr std::array<FeatureSemanticDescriptor, 32> DICTIONARY{{
        {"FLOW_DURATION", "s", 1.0, true},
        {"TOTAL_PACKETS_SRC_TO_DST", "pkts", 1.0, true},
        // ... (Remaining 30 semantic descriptors)
        {"DESTINATION_PORT", "port", 65535.0, false}
    }};
};

} // namespace sentinel::nexus
```
```

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


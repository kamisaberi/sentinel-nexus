---

### File: `sentinel-nexus/docs/rest-api-reference/endpoints-threats.md`

```markdown
# Threat Mitigation & XAI Endpoints (`/api/v1/threats/*`)

These endpoints provide programmatic access to the **Sub-50ms Collective Defense Bus**, active in-kernel drop tables, MITRE ATT&CK correlation heatmaps, and Explainable AI (XAI) feature attributions.

---

## 1. Broadcast Emergency Threat: `POST /api/v1/threats/broadcast`

Manually broadcasts an IP drop rule fleet-wide, programming all 5,000 edge kernel `blocked_ip_map` tables in under $50\,\text{ms}$.

### Request:
```bash
curl -k -X POST https://localhost:9443/api/v1/threats/broadcast \
    -H "Authorization: Bearer $TOKEN" \
    -H "Content-Type: application/json" \
    -d '{
      "target_ip": "198.51.100.42",
      "ttl_seconds": 3600,
      "threat_name": "SCADA_MODBUS_INJECTION",
      "mitre_id": "T0855",
      "justification": "Emergency operator containment"
    }'
```

### Response (`200 OK`):
```json
{
  "status": "BROADCAST_SUCCESS",
  "rule_id": 9042,
  "dispatched_nodes": 4992,
  "propagation_latency_ms": 31.4,
  "action": "IN_KERNEL_XDP_DROP_ENFORCED"
}
```

---

## 2. Query MITRE ATT&CK Heatmap: `GET /api/v1/threats/mitre`

Returns aggregated tactical incident counts categorized by MITRE ATT&CK Enterprise and ICS IDs.

### Response (`200 OK`):
```json
{
  "tactics": [
    {
      "tactic_id": "TA0108",
      "tactic_name": "Impair Process Control",
      "techniques": [
        { "technique_id": "T0855", "incident_count": 1420, "severity": "CRITICAL" },
        { "technique_id": "T0831", "incident_count": 842,  "severity": "CRITICAL" }
      ]
    }
  ]
}
```

---

## 3. Query Top-3 Feature Attributions: `GET /api/v1/threats/xai`

Retrieves Microsecond Residual Decomposition (MRD) root-cause feature attributions for historical threat mitigations.
```

---

### File: `sentinel-nexus/docs/rest-api-reference/endpoints-ota-models.md`

```markdown
# OTA Model Management Endpoints (`/api/v1/ota/*` & `/api/v1/models/*`)

These endpoints govern Canary model staging, artifact streaming, rollout state advancement, and emergency rollbacks.

---

## 1. Stage New Model Artifact: `POST /api/v1/ota/stage`

Accepts an ONNX model binary and cryptographic manifest from `xinfer-forge` or a CI/CD pipeline.

### Request (Multipart Form-Data):
```bash
curl -k -X POST https://localhost:9443/api/v1/ota/stage \
    -H "Authorization: Bearer $TOKEN" \
    -F "model_binary=@network_threat_v2.onnx" \
    -F "manifest_json=@network_threat_v2.manifest.json"
```

### Response (`201 Created`):
```json
{
  "status": "STAGED",
  "model_version": "2.4.0",
  "sha256": "e9a2c31e847b2c94b13a7b41e2d9010000000000000000000000000000000000",
  "deployment_stage": "STAGE_SHADOW_MODE",
  "canary_cohort_percentage": 0
}
```

---

## 2. Advance Rollout Stage: `POST /api/v1/ota/advance`

Promotes the candidate model along the staged rollout progression:  
`SHADOW_MODE` $\longrightarrow$ `CANARY_5_PCT` $\longrightarrow$ `FLEET_WIDE`.

### Request:
```bash
curl -k -X POST https://localhost:9443/api/v1/ota/advance \
    -H "Authorization: Bearer $TOKEN" \
    -H "Content-Type: application/json" \
    -d '{"target_stage": "CANARY_5_PCT"}'
```

---

## 3. Trigger Emergency Rollback: `POST /api/v1/ota/rollback`

Forces an immediate abort of active Canary models, reverting all edge appliances to the previous verified baseline in $< 50\,\text{ms}$.

---

## 4. Download Model Binary: `GET /api/v1/models/{filename}`

Streams the raw binary ONNX model to edge appliances during hot-reloads. Returns `Content-Type: application/octet-stream` and `X-Checksum-SHA256`.
```

---

### File: `sentinel-nexus/docs/rest-api-reference/endpoints-compliance.md`

```markdown
# Compliance & Audit Endpoints (`/api/v1/reports/*`)

These endpoints generate on-demand compliance audit records and regulatory scorecards for industrial and defense audits.

---

## 1. Generate Master Compliance Scorecard: `GET /api/v1/reports/compliance`

Returns an aggregated evaluation across all supported regulatory frameworks.

### Query Parameters
* `format` (optional, string): `json` (default), `text`, or `pdf`.
* `tenant_id` (optional, string): Scope report to a specific organization.

### Request:
```bash
curl -k -H "Authorization: Bearer $TOKEN" \
    https://localhost:9443/api/v1/reports/compliance?format=json
```

### Response (`200 OK`):
```json
{
  "timestamp_iso": "2026-10-05T07:53:00Z",
  "managed_nodes": 4992,
  "standards": {
    "iec_62443_3_3": {
      "status": "COMPLIANT",
      "security_level": "SL 3 / SL 4",
      "fr3_system_integrity": "100%",
      "fr5_zone_segmentation": "100%",
      "fr7_resource_availability": "100%"
    },
    "cmmc_level_2": {
      "status": "COMPLIANT",
      "si_l2_3_14_1_flaw_remediation": "SUB_MICROSECOND_VERIFIED",
      "sc_l2_3_13_1_boundary_protection": "ACTIVE"
    },
    "eu_nis_2": {
      "status": "COMPLIANT",
      "article_21_incident_handling": "SUB_50MS_BROADCAST_ACTIVE",
      "cloud_egress_fees": "$0.00"
    }
  }
}
```

---

## 2. Dedicated Standard Routes

* `GET /api/v1/reports/cmmc`: Detailed NIST SP 800-171 control proofs.
* `GET /api/v1/reports/scada`: IEC 62443-3-3 industrial conduit proofs.
```

---

### File: `sentinel-nexus/docs/rest-api-reference/endpoints-sse-stream.md`

```markdown
# Persistent Real-Time Telemetry Stream (`GET /api/v1/telemetry/stream`)

`sentinel-nexus` exposes a persistent **Server-Sent Events (SSE)** endpoint on port **9444** (and proxied via port 9443). It streams fleet telemetry, threat mitigations, and Canary health events at up to $100\text{ Hz}$.

---

## 1. Stream Endpoint Specification

* **Route:** `GET /api/v1/telemetry/stream` (Port 9444 or 9443)
* **Headers:**
  * `Accept: text/event-stream`
  * `Authorization: Bearer <TOKEN>`
* **Response Header:** `Content-Type: text/event-stream; charset=utf-8`

---

## 2. Stream Event Types

### Event: `fleet_tick`
Pushed every second with aggregated fleet-wide metrics:
```text
event: fleet_tick
data: {"timestamp_ns":1791172800184000000,"online_nodes":4992,"drops_today":1420891,"fleet_sla_us":0.82,"active_rules":12}
```

### Event: `threat_drop`
Pushed instantly when an edge appliance mitigates an active attack:
```text
event: threat_drop
data: {"incident_id":"inc-1802","appliance":"edge-substation-alpha","src_ip":"198.51.100.42","mitre_id":"T0855","action":"XDP_DROP","latency_us":0.81}
```

### Event: `collective_defense_sync`
Pushed when a rule is broadcast across the fleet bus:
```text
event: collective_defense_sync
data: {"rule_id":9042,"target_ip":"198.51.100.42","ttl_seconds":3600,"dispatched_nodes":4991,"latency_ms":31.4}
```

### Event: `canary_rollback`
Pushed if `RollbackGuard` aborts a candidate model rollout:
```text
event: canary_rollback
data: {"reason":"LATENCY_SLA_BREACH","observed_latency_us":1420.5,"reverted_to":"network_threat_v1.onnx"}
```

---

## 3. Client Consumption Example (Python)

```python
import sseclient
import requests

url = "http://127.0.0.1:9444/stream"
headers = {"Accept": "text/event-stream"}
response = requests.get(url, headers=headers, stream=True)
client = sseclient.SSEClient(response)

for event in client.events():
    print(f"[{event.event}] -> {event.data}")
```
```


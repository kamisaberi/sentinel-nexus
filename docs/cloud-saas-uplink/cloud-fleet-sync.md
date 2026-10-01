---

### File: `sentinel-nexus/docs/cloud-saas-uplink/cloud-fleet-sync.md`

```markdown
# High-Frequency Fleet Synchronization (`POST /api/v1/fleet/sync`)

Every **5.0 seconds**, `SaaSConnector` serializes the active 4-tier asset hierarchy and telemetry summary, transmitting the nested payload to the cloud backend over a single non-blocking HTTP/2 request.

---

## 1. Transmission Payload Schema

```json
{
  "tenant_id": "tenant-municipal-utility-bavaria",
  "nexus_id": "nexus-central-munich",
  "sync_timestamp_ns": 1791172800184000000,
  "fleet_summary": {
    "total_nodes": 4992,
    "online_nodes": 4990,
    "degraded_nodes": 2,
    "total_drops_today": 1420891,
    "fleet_sla_median_us": 0.82
  },
  "nodes": [
    {
      "node_id": "edge-substation-alpha",
      "status": "ONLINE",
      "ip_address": "10.240.0.101",
      "tpm_tier": 1,
      "cpu_pct": 4.2,
      "npu_temp_c": 44.2,
      "drops_today": 1420,
      "sensors": [
        { "sensor_id": "modbus-10.240.0.101-502-u1", "status": "ONLINE" },
        { "sensor_id": "s7-10.240.0.102-102-r0-s2",   "status": "ONLINE" }
      ]
    }
  ]
}
```

---

## 2. In-Engine Sync Loop (`SaaSConnector.cpp`)

```cpp
void SaaSConnector::run_sync_loop() {
    while (is_running_.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::seconds(5));

        if (!authenticate()) {
            continue;
        }

        // 1. Serialize active 4-tier tree snapshot
        std::string payload = build_fleet_sync_payload();

        // 2. Dispatch via non-blocking HTTPS POST
        HttpResponse resp = http_post_json("/api/v1/fleet/sync", payload, true);

        if (resp.status_code == 401) {
            // Token expired mid-run: invalidate token to force renewal next cycle
            std::lock_guard<std::recursive_mutex> lock(auth_mutex_);
            active_jwt_token_.clear();
        }
    }
}
```
```

---

### File: `sentinel-nexus/docs/cloud-saas-uplink/inbound-global-threat-feed.md`

```markdown
# Inbound Global Consortium Threat Feed

Subscribers to the Aryorithm Global Defense Consortium opt-in to receive anonymized zero-day threat indicators discovered across other enterprise tenant enclaves.

---

## 1. Global Feed Ingestion & eBPF Verification

```text
 Aryorithm Global Threat Cloud (app.aryorithm.com)
                       │
                       ▼ GET /api/v1/threats/global-feed (Polled every 60s)
 ┌─────────────────────────────────────────────────────────────┐
 │ SaaSConnector::poll_global_threat_feed()                    │
 ├─────────────────────────────────────────────────────────────┤
 │ Ingests verified zero-day IoC records:                      │
 │  • Target IPv4 / Subnet CIDR                                │
 │  • Verified Attestation: Discovered on TIER 1 Hardware      │
 │  • MITRE ATT&CK Technique ID & Threat Signature             │
 └─────────────────────┬───────────────────────────────────────┘
                       │
                       ▼ Direct Local In-Kernel Injection
 ┌─────────────────────────────────────────────────────────────┐
 │ Injected into local Nexus Collective Defense Bus (< 50ms)   │
 │ -> Programmed into all 5,000 edge eBPF blocked_ip_map tables│
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Inbound IoC Schema (`GlobalThreatFeed.json`)

```json
{
  "feed_timestamp_ns": 1791172800184000000,
  "new_indicators": [
    {
      "indicator_id": "ioc-global-8412",
      "target_ipv4_net_order": 3221225542,
      "target_ip_str": "192.0.2.70",
      "ttl_seconds": 86400,
      "severity": "CRITICAL",
      "threat_name": "TRITON_SIS_OVERRIDE_SCANNER",
      "mitre_id": "T0843",
      "consensus_confidence": 0.998
    }
  ]
}
```
```

---

### File: `sentinel-nexus/docs/cloud-saas-uplink/remote-ciso-commands.md`

```markdown
# Remote CISO Administrative Commands (`GET /api/v1/commands/pending`)

For enterprise CISOs managing hundreds of remote facilities, the cloud portal (`app.aryorithm.com`) allows authorized administrators to issue high-priority operational commands that local Nexus instances retrieve during polling sweeps.

---

## 1. Command Execution Flow

```text
 Corporate CISO executes Emergency Canary Rollback on Web Dashboard
                              │
                              ▼ Queued in Cloud Backend: /commands/pending
 ┌─────────────────────────────────────────────────────────────┐
 │ Local SaaSConnector polls /commands/pending every 2 seconds │
 └────────────────────────────┬────────────────────────────────┘
                              │
                              ▼ Command Decrypted & Signature Verified
 ┌─────────────────────────────────────────────────────────────┐
 │ SaaSConnector::execute_remote_command()                     │
 ├─────────────────────────────────────────────────────────────┤
 │ • EMERGENCY_MODEL_ROLLBACK: Aborts active Canary fleet-wide │
 │ • GLOBAL_IP_PURGE         : Clears false-positive block rule│
 │ • FORENSIC_DUMP_TRIGGER   : Gathers signed PCAP bundle      │
 └────────────────────────────┬────────────────────────────────┘
                              │
                              ▼ Executed Locally in < 50ms
 [ Fleet state updated; completion receipt returned to Cloud ]
```

---

## 2. Cryptographic Command Signing

To prevent rogue server takeovers from compromising edge plants:
* All administrative commands dispatched by the cloud portal are **digitally signed with the corporate CISO’s offline administrative key**.
* `sentinel-nexus` validates the command signature against pre-enrolled public keys in `/etc/sentinel-nexus/certs/admin_authority.crt` before executing rollbacks or policy changes.
```


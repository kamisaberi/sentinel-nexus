### Part 11: Hybrid Cloud SaaS Uplink (`cloud-saas-uplink/*`)

This section contains 5 technical implementation guides detailing the outbound cloud integration engine of `sentinel-nexus`: the decoupled C++ `SaaSConnector` client, JWT token cycling and mutex deadlock prevention, the 5-second 4-tier fleet synchronization transmission, inbound global consortium threat feed polling, and remote CISO emergency command execution.

---

### File: `sentinel-nexus/docs/cloud-saas-uplink/saas-connector-architecture.md`

```markdown
# SaaS Connector Architecture (`SaaSConnector.cpp`)

The `SaaSConnector` subsystem is an optional, outbound-only C++20 HTTPS client that connects on-premises `sentinel-nexus` hubs to the **Aryorithm Cloud SaaS Platform (`app.aryorithm.com`)**. It maintains high-level executive visibility across distributed multi-region facilities while preserving the **zero-ingress, air-gapped operational independence** of local hubs.

---

## 1. Outbound-Only Architecture

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ ON-PREMISES SECURE ENCLAVE                                  │
 │                                                             │
 │  ┌───────────────────────────────────────────────────────┐  │
 │  │ sentinel-nexus Hub Core (NodeRegistry & StateDatabase)│  │
 │  └───────────────────────────┬───────────────────────────┘  │
 │                              │ In-Memory Read Snapshots     │
 │                              ▼                              │
 │  ┌───────────────────────────────────────────────────────┐  │
 │  │ SaaSConnector.cpp (Outbound Background Thread)        │  │
 │  │ • Isolated std::recursive_mutex                      │  │
 │  │ • 3-Second Socket Timeouts (SO_RCVTIMEO/SO_SNDTIMEO)  │  │
 │  │ • Non-Blocking Asynchronous Egress                    │  │
 │  └───────────────────────────┬───────────────────────────┘  │
 └──────────────────────────────┼──────────────────────────────┘
                                │
                                ▼ Outbound HTTPS TLS 1.3 (Port 443 Only)
                                │ ZERO INBOUND PORTS OPEN TO CLOUD
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ Aryorithm Cloud SaaS Platform (app.aryorithm.com)           │
 │  - FastAPI Backend & PostgreSQL TimeScale Cluster           │
 │  - Multi-Tenant Executive CISO Dashboard                    │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Critical Deadlock & Socket Timeout Invariants

During high-concurrency production runs, socket communication is protected by three architectural safeguards:

1. **`std::recursive_mutex` Re-entrancy:** Prevents thread self-deadlocks where `authenticate()` and `http_post_json()` attempt to acquire the authentication lock on the same thread.
2. **Explicit Socket Timeouts:** All TCP sockets enforce a **3-second timeout** (`SO_RCVTIMEO` and `SO_SNDTIMEO`), preventing daemon hangs during network degradation.
3. **HTTP `Content-Length` Parsing:** The socket reader parses `Content-Length` headers directly rather than waiting for EOF, preventing connection stalls.
```

---

### File: `sentinel-nexus/docs/cloud-saas-uplink/jwt-authentication-and-renewal.md`

```markdown
# JWT Authentication, Session Caching & Token Renewal

`SaaSConnector` authenticates with the cloud control plane using scoped JSON Web Tokens (JWT). It caches session state locally in `data/cloud_session.json` and implements automatic renewal upon token expiration or HTTP `401 Unauthorized` responses.

---

## 1. Authentication Lifecycle Sequence

```text
 SaaSConnector Startup
          │
          ▼ Check data/cloud_session.json
 ┌─────────────────────────────────────────────────────────────┐
 │ 1. Does a valid cached JWT exist with > 5 minutes lease?    │
 └────────┬───────────────────────────────────────────┬────────┘
          │ YES                                       │ NO / Expired
          ▼                                           ▼
 ┌─────────────────────────────┐             ┌─────────────────────────────┐
 │ Load active token from disk │             │ POST /api/v1/auth/login     │
 │ and proceed to fleet sync   │             │ Authenticate API key/secret │
 └─────────────────────────────┘             └──────────────┬──────────────┘
                                                            │
                                                            ▼ Save Session
                                             ┌─────────────────────────────┐
                                             │ Cache JWT & expiry timestamp│
                                             │ to data/cloud_session.json  │
                                             └─────────────────────────────┘
```

---

## 2. In-Engine Authentication Implementation (`SaaSConnector.cpp`)

```cpp
#include <sentinel_nexus/SaaSConnector.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <chrono>

namespace sentinel::nexus {

bool SaaSConnector::authenticate() {
    std::lock_guard<std::recursive_mutex> lock(auth_mutex_);

    // 1. Check local session cache
    if (load_cached_session()) {
        uint64_t now_epoch = get_epoch_seconds();
        if (now_epoch + 300 < token_expiry_epoch_) { // 5-minute safety buffer
            return true; // Token valid
        }
    }

    // 2. Perform authentication request
    nlohmann::json login_body = {
        {"api_key", config_.cloud_api_key},
        {"api_secret", config_.cloud_api_secret},
        {"nexus_id", config_.nexus_id}
    };

    HttpResponse resp = http_post_json("/api/v1/auth/login", login_body.dump(), false);

    if (resp.status_code == 200) {
        auto resp_json = nlohmann::json::parse(resp.body);
        active_jwt_token_ = resp_json["access_token"].get<std::string>();
        token_expiry_epoch_ = resp_json["expires_at"].get<uint64_t>();

        save_cached_session();
        XINFER_LOG_INFO("SaaSConnector: Authenticated with cloud backend successfully.");
        return true;
    }

    XINFER_LOG_ERROR("SaaSConnector: Authentication failed with status {}", resp.status_code);
    return false;
}

} // namespace sentinel::nexus
```
```

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

---

### Complete in Part 11
- `sentinel-nexus/docs/cloud-saas-uplink/saas-connector-architecture.md`
- `sentinel-nexus/docs/cloud-saas-uplink/jwt-authentication-and-renewal.md`
- `sentinel-nexus/docs/cloud-saas-uplink/cloud-fleet-sync.md`
- `sentinel-nexus/docs/cloud-saas-uplink/inbound-global-threat-feed.md`
- `sentinel-nexus/docs/cloud-saas-uplink/remote-ciso-commands.md`

All 5 Hybrid Cloud SaaS Uplink documentation files for `sentinel-nexus` are now generated.

---

### Files to be Generated in Part 12

The next phase covers **Compliance Engines & Regulatory GRC** (`compliance-engines/` - 4 files):

1. `compliance-engines/cmmc-2.0-audit-engine.md` (Verifying AC.L2-3.1.1, IA.L2-3.5.1, and SI.L2-3.14.1)
2. `compliance-engines/iec-62443-audit-engine.md` (Verifying FR 3 System Integrity & FR 5 Zone Segmentation)
3. `compliance-engines/latency-sla-percentile-proofs.md` (Microsecond percentile proofs: p50 0.84µs to p99.9 1.04µs)
4. `compliance-engines/tamper-evident-audit-logging.md` (Cryptographic state journaling in `data/nexus_state.json`)

Confirm when you are ready to proceed with Part 12.
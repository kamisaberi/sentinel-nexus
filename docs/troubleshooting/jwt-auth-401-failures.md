---

### File: `sentinel-nexus/docs/troubleshooting/jwt-auth-401-failures.md`

```markdown
# Resolving JWT Authentication & 401 Unauthorized Failures

Authentication failures manifest in two distinct vectors:
1. **Inbound 401s:** Security operators or scripts calling `https://localhost:9443/api/v1/*`.
2. **Outbound 401s:** `SaaSConnector.cpp` transmitting 4-tier tree telemetry to `https://app.aryorithm.com`.

---

## 1. Inbound REST API 401 Unauthorized

### Symptom
```text
HTTP/1.1 401 Unauthorized
{"error": "TOKEN_EXPIRED", "message": "JWT token lease expired at epoch 1791176400"}
```

### Remediation
1. Re-authenticate using the operations CLI:
   ```bash
   nexus-ctl auth login admin@substation.internal
   ```
2. Verify token validity:
   ```bash
   nexus-ctl auth whoami
   ```
3. In automated scripts, pass the new token via the `Authorization: Bearer <TOKEN>` header.

---

## 2. Outbound `SaaSConnector` 401 Token Cycling

### Symptom
```text
[ERROR] SaaSConnector: Cloud sync rejected with status 401. Token refresh required.
```

### The C++ Mutex Deadlock Fix
In earlier versions, `SaaSConnector::authenticate()` acquired `auth_mutex_`, then invoked `http_post_json()`, which called `get_active_token()` and attempted to acquire the same non-recursive mutex on the same thread—freezing the background connector.

Verify that `SaaSConnector.hpp` declares an **`std::recursive_mutex`**:

```cpp
// Correct declaration in SaaSConnector.hpp:
std::recursive_mutex auth_mutex_;
```

This permits re-entrant calls within the same thread while keeping token acquisition safe across asynchronous workers.
```

---

### File: `sentinel-nexus/docs/troubleshooting/sse-stream-disconnects.md`

```markdown
# Troubleshooting Real-Time SSE Stream Disconnects (Port 9444)

The Server-Sent Events (SSE) telemetry pipeline pushes updates at up to $100\text{ Hz}$ to the Web Command Center. If intermediary reverse proxies (such as Nginx, HAProxy, or Envoy) or browser timeouts interrupt the stream, the UI topology will freeze.

---

## 1. Symptom & Visual Indicator

On the Web Command Center (port 9443):
* The top status card turns amber: `SSE CONNECTION DROPPED (RECONNECTING...)`.
* Node topology graphs freeze and fail to render live packet pulses.

---

## 2. Resolving Proxy Buffering Delays

If `sentinel-nexus` is fronted by an enterprise reverse proxy, the proxy may attempt to buffer the event stream rather than flushing chunks immediately.

### Nginx Configuration Fix:
Add these proxy parameters to `/etc/nginx/conf.d/nexus.conf`:

```nginx
location /stream {
    proxy_pass http://127.0.0.1:9444/stream;
    proxy_http_version 1.1;
    proxy_set_header Connection "";
    
    # Disable proxy response buffering for real-time streaming
    proxy_buffering off;
    proxy_cache off;
    chunked_transfer_encoding off;
    
    # Extend read timeout to prevent idle disconnects
    proxy_read_timeout 86400s;
    proxy_send_timeout 86400s;
}
```

Reload Nginx:

```bash
sudo nginx -s reload
```

---

## 3. Keepalive Heartbeat Verification

`sentinel-nexus` emits an empty comment frame (`: keepalive\n\n`) every $15.0\text{ seconds}$ to prevent firewall state tables from closing idle TCP connections. 

Verify the raw stream output from the command line:

```bash
curl -N -v http://localhost:9444/stream
```
```

---

### File: `sentinel-nexus/docs/troubleshooting/config-file-parsing-errors.md`

```markdown
# Resolving YAML Parser Inline Comment Stripping Bugs

A critical parsing bug can occur when editing `/etc/sentinel-nexus/nexus.yaml`: if values contain inline comments, naive string readers may append the comment text to URLs, ports, or API endpoints.

---

## 1. Symptom

```text
[ERROR] SaaSConnector: Failed to resolve endpoint 'https://app.aryorithm.com # Central SaaS Cloud'
[ERROR] Uvicorn/FastAPI: 400 Bad Request: Invalid HTTP request received (Path contains spaces)
```

---

## 2. Root Cause & In-Engine Remediation

When parsing configuration lines such as:

```yaml
cloud_endpoint: "https://app.aryorithm.com" # Central SaaS Cloud
```

A standard parser without comment stripping treats `"https://app.aryorithm.com" # Central SaaS Cloud` as a single string literal.

### The C++ Code Fix in `ConfigManager.cpp`

`ConfigManager.cpp` strips comments before trimming quotation marks and whitespace:

```cpp
std::string sanitize_yaml_value(std::string raw_val) {
    // 1. Locate inline comment marker '#'
    size_t comment_pos = raw_val.find('#');
    if (comment_pos != std::string::npos) {
        raw_val = raw_val.substr(0, comment_pos);
    }

    // 2. Trim whitespace
    raw_val.erase(0, raw_val.find_first_not_of(" \t\r\n"));
    raw_val.erase(raw_val.find_last_not_of(" \t\r\n") + 1);

    // 3. Strip quotation marks
    if (raw_val.size() >= 2 && raw_val.front() == '"' && raw_val.back() == '"') {
        raw_val = raw_val.substr(1, raw_val.size() - 2);
    }
    return raw_val;
}
```

Always run `sentinel-nexus --validate-config` to verify that values parse cleanly before starting services.
```

---

### File: `sentinel-nexus/docs/troubleshooting/faq.md`

```markdown
# Technical Frequently Asked Questions (FAQ)

---

### Q1: How does the Collective Defense Bus achieve sub-50ms fanout across 5,000 appliances?
`sentinel-nexus` maintains persistent, open HTTP/2 multiplexed streams (`StreamFleetRules`) to all connected appliances over mutual TLS 1.3 on port 50051. When a threat indicator arrives, the `IocBroadcaster` iterates through its in-memory stream map in parallel, avoiding TCP handshakes, TLS renegotiations, or database lookups on the distribution path.

---

### Q2: What happens if an edge appliance loses connection to Nexus?
The edge appliance (`blackbox-sentinel`) operates with complete local autonomy:
* It continues evaluating network flows using its active in-memory model.
* It drops matching attacks in kernel driver space ($< 0.84\,\mu\text{s}$).
* Telemetry events are buffered locally in RAM ring buffers until the Nexus connection is re-established.

---

### Q3: Does `sentinel-nexus` require internet access to function?
**No.** `sentinel-nexus` is designed for sovereign, air-gapped deployments. In industrial plants and classified networks, it operates without WAN access. The embedded Web Command Center (port 9443) has **zero external CDN dependencies**, and all model retraining cycles (`xinfer-forge`) execute on-premises.

---

### Q4: How does `RollbackGuard` decide when to abort a Canary model?
During the Canary phase (where 5% of the fleet runs the candidate model in active mitigation), `RollbackGuard` tracks latency metrics reported via heartbeats. If any appliance reports mitigation latency exceeding **$1{,}000\,\mu\text{s}$ ($1.0\,\text{ms}$)** for 3 consecutive intervals, or if the drop rate surges by more than $500\%$ over moving baseline averages, `RollbackGuard` triggers an immediate rollback to the previous baseline model within $50\,\text{ms}$.

---

### Q5: Can I manage multiple distinct organizations on a single Nexus instance?
**Yes.** `sentinel-nexus` enforces a multi-tenant hierarchy (`Tenant` $\to$ `Nexus` $\to$ `Sentinel` $\to$ `Sensor`). All database records, telemetry queues, and API queries are partitioned strictly by `tenant_id`.
```

---

### File: `sentinel-nexus/docs/troubleshooting/support.md`

```markdown
# Enterprise Support SLAs & Incident Escalation

---

## 1. Automated Diagnostic Bundle Generation

When reporting an issue with fleet synchronization, gRPC connection drops, or model staging failures, generate an automated diagnostic bundle:

```bash
nexus-ctl diag --bundle --output /tmp/nexus_diagnostic_bundle.tar.gz
```

This bundle packages:
* `/etc/sentinel-nexus/nexus.yaml` (With credentials and keys automatically sanitized).
* Active systemd service journal logs (last 1,000 lines).
* Active in-memory fleet inventory snapshot from `data/nexus_state.json`.
* Thread pool metrics and open socket counts across ports 50051, 9443, and 9444.

---

## 2. Enterprise Commercial Support SLAs

Aryorithm Technologies B.V. provides commercial support for defense and critical infrastructure networks:

| Support Tier | Target Response Time | Availability | Scope |
| :--- | :--- | :--- | :--- |
| **Standard Support** | 8 Business Hours | Mon–Fri 08:00–18:00 CET | Configuration review, updates, bug patches. |
| **Mission-Critical Defense**| **1 Hour (24/7/365)** | Round-the-Clock | Dedicated systems architect, high-availability cluster design, custom protocol integration, on-site emergency support. |

For technical inquiries and enterprise SLA contracts:
* **Customer Portal:** `https://app.aryorithm.com/support`
* **Email:** `support@aryorithm.com`

---

## 3. Coordinated Security Vulnerability Disclosure

If you identify a potential security bypass, authentication flaw, or verifier escape in `sentinel-nexus`:
* Send an encrypted PGP message to **`security@aryorithm.com`**.
* We acknowledge disclosures within **48 hours** and provide CVE assignment, risk remediation, and backported security patches according to coordinated disclosure guidelines.
```


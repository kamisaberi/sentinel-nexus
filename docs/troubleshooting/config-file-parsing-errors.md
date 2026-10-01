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


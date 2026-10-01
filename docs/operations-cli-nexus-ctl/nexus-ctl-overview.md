### Part 9: Operations CLI (`nexus-ctl`) (`operations-cli-nexus-ctl/*`)

This section contains 6 technical reference guides and usage manuals for the **`nexus-ctl`** standalone administrative CLI: global command syntax and authentication flags, fleet listing, manual threat broadcasting, Canary OTA lifecycle management, compliance report generation, and cloud/local authentication token acquisition.

---

### File: `sentinel-nexus/docs/operations-cli-nexus-ctl/nexus-ctl-overview.md`

```markdown
# `nexus-ctl` Terminal Administration Tool Overview

`nexus-ctl` is a native ISO C++20 command-line administration utility installed alongside `sentinel-nexus`. It provides systems engineers, SOC analysts, and automation pipelines with programmatic control over the fleet orchestrator, collective defense rules, model staging, and compliance reporting.

---

## 1. Global Syntax

```bash
nexus-ctl [GLOBAL OPTIONS] COMMAND [SUBCOMMAND] [ARGUMENTS...]
```

### Global Options

| Option Flag | Environment Variable | Default Value | Description |
| :--- | :--- | :--- | :--- |
| `--nexus-url URL` | `NEXUS_API_URL` | `https://127.0.0.1:9443` | Sentinel-Nexus management endpoint. |
| `--token STRING` | `NEXUS_AUTH_TOKEN` | Read from session cache | Scoped Bearer JWT authentication token. |
| `--config PATH` | `NEXUS_CONFIG` | `~/.nexus/config.json` | Path to local credentials and preferences. |
| `--json` | `NEXUS_JSON_OUTPUT` | `false` | Emits machine-readable JSON output to stdout. |
| `--insecure` | N/A | `false` | Skips TLS certificate verification (Dev only). |
| `--help` | N/A | N/A | Displays help syntax and flags. |

---

## 2. Command Tree Summary

* **`nexus-ctl fleet`**: Node querying, health status, telemetry, and manual node eviction.
* **`nexus-ctl threat`**: In-kernel threat broadcasting (`drop`), global unblocking (`unblock`), and XAI querying.
* **`nexus-ctl ota`**: Canary model rollout inspection, artifact staging, phase advancement, and emergency rollback.
* **`nexus-ctl report`**: On-demand compliance scorecard compilation (CMMC Level 2, IEC 62443, EU NIS 2).
* **`nexus-ctl auth`**: Operator login, session token renewal, and credential caching.

---

## 3. Local Session Cache (`~/.nexus/session.json`)

When an administrator logs in via `nexus-ctl auth login`, the resulting Bearer JWT is cached securely with `0600` file permissions:

```json
{
  "nexus_url": "https://127.0.0.1:9443",
  "auth_token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "expires_at_epoch": 1791176400,
  "operator_email": "admin@substation.internal",
  "role": "FLEET_SECURITY_ADMIN"
}
```
```

---

### File: `sentinel-nexus/docs/operations-cli-nexus-ctl/command-fleet-list.md`

```markdown
# Command: `nexus-ctl fleet list`

Queries the centralized `NodeRegistry` on `sentinel-nexus`, displaying real-time operational status, CPU utilization, drop counters, and sub-microsecond mitigation SLAs across all managed edge appliances.

---

## 1. Syntax

```bash
nexus-ctl fleet list [OPTIONS]
```

### Options

| Flag | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `--status [ALL\|ONLINE\|DEGRADED\|OFFLINE]` | String | `ALL` | Filters nodes by active health state. |
| `--tpm-tier [1\|2\|3]` | Integer | All | Filters by hardware root-of-trust level. |
| `--limit INT` | Integer | `50` | Maximum number of appliances to display. |
| `--json` | Flag | `false` | Emits raw JSON array for shell scripting. |

---

## 2. Standard Table Output

```bash
nexus-ctl fleet list --status ONLINE
```

### Output:
```text
====================================================================================================
                              SENTINEL-NEXUS ACTIVE MANAGED FLEET
====================================================================================================
NODE UUID                  HOSTNAME               IP ADDRESS     STATUS   TPM    DROPS   SLA (p99)
edge-substation-alpha      substation-01.internal 10.240.0.101   ONLINE   TIER1  14,209  0.82 µs
edge-substation-bravo      substation-02.internal 10.240.0.102   ONLINE   TIER1   8,412  0.81 µs
edge-water-treatment-01    water-munich.internal  10.240.0.103   ONLINE   TIER1   1,094  0.84 µs
edge-medical-clinic-04     radiology-04.internal  10.240.0.104   ONLINE   TIER2     142  0.83 µs
----------------------------------------------------------------------------------------------------
Total Nodes Displayed: 4 | Fleet Average Latency: 0.825 µs | Fleet SLA Status: COMPLIANT (< 1.0 µs)
====================================================================================================
```

---

## 3. JSON Output for Automated Tooling

```bash
nexus-ctl fleet list --json | jq '.[0] | {uuid: .node_uuid, ip: .ip_address, latency: .last_latency_us}'
```

### JSON Record:
```json
{
  "uuid": "edge-substation-alpha",
  "ip": "10.240.0.101",
  "latency": 0.82
}
```
```

---

### File: `sentinel-nexus/docs/operations-cli-nexus-ctl/command-threat-drop.md`

```markdown
# Command: `nexus-ctl threat drop`

Manually broadcasts an emergency IP drop rule across the entire fleet via the **Sub-50ms Collective Defense Bus**, programming the target address directly into all 5,000 edge kernel `blocked_ip_map` tables.

---

## 1. Syntax

```bash
nexus-ctl threat drop <TARGET_IP> [OPTIONS]
```

### Options

| Flag | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `<TARGET_IP>` | IPv4 String | **Required** | The target IPv4 address to block fleet-wide. |
| `--ttl INT` | Integer | `3600` | Ephemeral Time-To-Live in seconds (e.g., 3600 = 1h). |
| `--rule-id INT` | Integer | `9001` | Unique security rule identifier. |
| `--mitre-id STR` | String | `T0855` | Associated MITRE ATT&CK technique ID. |
| `--reason STR` | String | `"Manual"` | Operator justification for audit trail logging. |

---

## 2. Execution Example

```bash
nexus-ctl threat drop 198.51.100.42 \
    --ttl 7200 \
    --mitre-id T0855 \
    --reason "Coordinated Modbus brute-force sweep on Substation Alpha"
```

### Terminal Output:
```text
[*] Initiating Collective Defense Broadcast...
[+] Target Address      : 198.51.100.42 (Network Byte Order: 0x2A6433C6)
[+] Duration (TTL)      : 7200 seconds (2 Hours)
[+] MITRE Technique     : T0855 (Unauthorized Command Message)
[+] Justification       : Coordinated Modbus brute-force sweep on Substation Alpha

--------------------------------------------------------------------------------
[+] DISPATCH SUMMARY:
    Dispatched Nodes    : 4,992 Active Appliances
    Propagation Latency : 31.4 milliseconds (< 50ms SLA Bound)
    Enforcement State   : IN_KERNEL_XDP_DROP ACTIVE FLEET-WIDE
================================================================================
```

---

## 3. Global Unblocking (`threat unblock`)

To remove a false-positive block fleet-wide:

```bash
nexus-ctl threat unblock 198.51.100.42 --reason "Authorized testing completed"
```
```

---

### File: `sentinel-nexus/docs/operations-cli-nexus-ctl/command-ota-management.md`

```markdown
# Command: `nexus-ctl ota`

Manages the staged model rollout lifecycle, inspects Canary cohort performance, advances deployment stages, and triggers emergency rollbacks.

---

## 1. Syntax

```bash
nexus-ctl ota [status | stage | advance | rollback] [OPTIONS]
```

---

## 2. Subcommands

### 1. `nexus-ctl ota status`
Displays the active model, candidate Canary model, and rollout progression:

```bash
nexus-ctl ota status
```

#### Output:
```text
================================================================================
                    CANARY OTA MODEL ROLLOUT CONTROLLER
================================================================================
Active Fleet Model     : network_threat_v1.onnx (SHA256: e9a2c31e...)
Candidate Model        : network_threat_v2_canary.onnx (SHA256: 3a7b41e2...)
Current Rollout Stage  : STAGE 2: CANARY_5_PCT (Active Cohort: 250 / 5,000 Nodes)
Stage Elapsed Time     : 14 hours, 22 minutes (Required Window: 24 hours)

Canary Cohort Health:
 • Mitigation Latency p50 : 0.82 µs (Nominal)
 • Mitigation Latency p99 : 0.84 µs (Meets < 1.0 ms SLA)
 • Consecutive SLA Breaches: 0 / 3
 • False Positive Surge   : 1.02x Baseline (Nominal)

RollbackGuard Status   : ARMED & HEALTHY (Zero Rollback Triggers Fired)
================================================================================
```

---

### 2. `nexus-ctl ota stage <ONNX_FILE>`
Submits an exported model artifact to the staging repository:

```bash
nexus-ctl ota stage /opt/sentinel/models/network_threat_v2.onnx \
    --manifest /opt/sentinel/models/network_threat_v2.manifest.json
```

---

### 3. `nexus-ctl ota advance`
Promotes a verified model to the next rollout stage:

```bash
nexus-ctl ota advance --force
```

Transitions: `SHADOW_MODE` $\to$ `CANARY_5_PCT` $\to$ `FLEET_WIDE`.

---

### 4. `nexus-ctl ota rollback`
Forces an immediate emergency rollback:

```bash
nexus-ctl ota rollback --reason "Operator manual abort: Latency jitter observed"
```
```

---

### File: `sentinel-nexus/docs/operations-cli-nexus-ctl/command-compliance-reports.md`

```markdown
# Command: `nexus-ctl report`

Compiles automated regulatory compliance audit scorecards across all connected fleet assets, evaluating configurations and telemetry against **CMMC 2.0 Level 2**, **IEC 62443**, and **EU NIS 2**.

---

## 1. Syntax

```bash
nexus-ctl report [cmmc | scada | nis2] [OPTIONS]
```

### Options

| Flag | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `--format [text\|json\|pdf]` | String | `text` | Output presentation format. |
| `--output PATH` | File Path | stdout | Destination file for exported compliance bundle. |
| `--tenant-id STR` | String | All | Scope report to a specific enterprise tenant. |

---

## 2. Generating an IEC 62443 Audit Report

```bash
nexus-ctl report scada --format text
```

### Terminal Output:
```text
================================================================================
           IEC 62443-3-3 INDUSTRIAL AUTOMATION COMPLIANCE AUDIT
================================================================================
Tenant Scope           : tenant-municipal-utility-bavaria
Evaluated Appliances   : 4,992 Managed Nodes
Target Security Level  : SL 3 / SL 4

Foundational Requirements Evaluation:
 [PASS] FR 3 - System Integrity (SR 3.1 & SR 3.5)
        • Proof: In-kernel Modbus/S7 APDU validation active on 4,992/4,992 nodes.
        • Zero unvalidated industrial writes passed in last 30 days.

 [PASS] FR 5 - Zone Segmentation (SR 5.1 & SR 5.2)
        • Proof: Native eBPF/XDP boundary filters active on all ingress conduits.
        • Sub-microsecond drop SLA verified: Fleet median p50 = 0.82 µs.

 [PASS] FR 7 - Resource Availability (SR 7.1 & SR 7.2)
        • Proof: Zero-SKB allocation prevents memory starvation under 10GbE floods.
        • Total unhandled packet drops: 0.

--------------------------------------------------------------------------------
COMPLIANCE STATUS: 100% CONFORMANT (SL 3 / SL 4 CERTIFIED)
================================================================================
```
```

---

### File: `sentinel-nexus/docs/operations-cli-nexus-ctl/command-auth-login.md`

```markdown
# Command: `nexus-ctl auth login`

Authenticates an administrative operator or automation service account against `sentinel-nexus` or the cloud SaaS portal (`app.aryorithm.com`), acquiring and caching a cryptographically signed Bearer JWT token.

---

## 1. Syntax

```bash
nexus-ctl auth login [EMAIL] [PASSWORD] [OPTIONS]
```

### Options

| Flag | Type | Description |
| :--- | :--- | :--- |
| `EMAIL` | String | Administrative account email address. |
| `PASSWORD` | String | Account password (prompted securely if omitted). |
| `--cloud` | Flag | Authenticates against cloud backend (`app.aryorithm.com`). |
| `--local` | Flag | Authenticates against on-premises Nexus Hub (Default). |
| `--mfa-token STR` | String | 6-digit Time-based One-Time Password (TOTP) token. |

---

## 2. Interactive Login Example

```bash
nexus-ctl auth login admin@substation.internal
```

### Terminal Prompt:
```text
Enter password: ****************
Enter MFA OTP token (if enabled): 412891
[*] Authenticating against https://127.0.0.1:9443/api/v1/auth/login...
[+] Authentication Successful!
    Operator Identity : admin@substation.internal
    Assigned Role     : FLEET_SECURITY_ADMIN
    Token Lease       : 8 Hours (Expires: 2026-10-05 16:00:00 UTC)
    Session Cached To : ~/.nexus/session.json (Permissions: 0600)
```

---

## 3. Session Verification

Verify the active session status without logging in again:

```bash
nexus-ctl auth whoami
```

### Output:
```text
Authenticated as: admin@substation.internal [Role: FLEET_SECURITY_ADMIN]
Nexus Management URL: https://127.0.0.1:9443
Token Status: ACTIVE (4 hours, 12 minutes remaining)
```
```

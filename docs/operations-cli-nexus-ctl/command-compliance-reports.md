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
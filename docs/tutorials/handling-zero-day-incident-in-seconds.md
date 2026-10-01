---

### File: `sentinel-nexus/docs/tutorials/handling-zero-day-incident-in-seconds.md`

```markdown
# Walkthrough: Coordinated Multi-Site Zero-Day Attack Containment

This tutorial walks through an operational scenario: an adversary launches a coordinated Modbus register override attack targeting water treatment facilities across multiple regions, demonstrating how `sentinel-nexus` neutralizes the threat fleet-wide in **$< 50\,\text{milliseconds}$**.

---

## 1. Timeline of the Coordinated Incident

```text
 TIME: 02:14:00.000 UTC
 • Adversary initiates automated script targeting Modbus port 502 across:
   - Facility 1: Munich East Water Treatment (10.240.0.101)
   - Facility 2: Nuremberg Water Treatment  (10.240.0.102)
   - Facility 3: Augsburg Water Treatment   (10.240.0.103)
```

---

## 2. Step 1: Detection & Local In-Kernel Drop at Facility 1

At $t = 02:14:00.120$, the first malicious frame hits the Munich East appliance (`10.240.0.101`):
1. Subsystem `18_cps_sec` flags an illegal setpoint jump on high-pressure valve register `40001` ($+7{,}750\,\text{PSI}$).
2. Tier 2 `libblackbox.so` executes `XDP_DROP` in driver memory in **$0.81\,\mu\text{s}$**, protecting the physical valve.
3. The appliance transmits a `ThreatIoC` to `sentinel-nexus` over gRPC port 50051:

```text
[02:14:00.134] Nexus Hub receives ThreatIoC from 'edge-substation-munich':
    Target IP : 198.51.100.42
    Threat ID : MODBUS_REGISTER_OVERRIDE (T0855)
    Severity  : CRITICAL
```

---

## 3. Step 2: Collective Defense Fanout ($< 50\,\text{ms}$)

At $t = 02:14:00.136$:
1. The `IocBroadcaster` on `sentinel-nexus` initiates an asynchronous parallel fanout over active gRPC streams.
2. A `FleetDefenseRule` is dispatched to all 4,999 remaining appliances:

```text
[02:14:00.158] Nexus Hub completes fanout:
    Total Dispatched Nodes : 4,999
    Fleet Propagation Time : 24.2 milliseconds
```

---

## 4. Step 3: Proactive Pre-Emptive Drops at Facilities 2 and 3

At $t = 02:14:00.160$, the adversary's automated scanner begins probing Facility 2 (Nuremberg) and Facility 3 (Augsburg):
1. The attacker's packets arrive at the network interfaces.
2. Because the Nuremberg and Augsburg nodes already have `198.51.100.42` programmed into their in-kernel `blocked_ip_map` tables, **the packets are dropped immediately in driver space ($0.79\,\mu\text{s}$)**.
3. The attack fails across all secondary sites with **zero application-layer compromise**.

---

## 5. Verifying Incident Details in the Web Console

Open **`https://10.240.0.10:9443`** and navigate to **Threat Management $\to$ Incident History**:
* The incident timeline illustrates the single origin point and subsequent pre-emptive drops across the fleet.
* The XAI viewer provides the root-cause register deviation evidence ready for regulatory submission.
```

---

### File: `sentinel-nexus/docs/tutorials/integrating-fastapi-cloud-backend.md`

```markdown
# Connecting Sentinel-Nexus to the Cloud SaaS Backend (`app.aryorithm.com`)

For enterprises using hybrid cloud management, this tutorial covers connecting an on-premises `sentinel-nexus` hub to the **Aryorithm Multi-Tenant Cloud Portal (`app.aryorithm.com`)** via the decoupled `SaaSConnector` client.

---

## 1. Cloud Credentials Acquisition

1. Log into your account on **`https://app.aryorithm.com`**.
2. Navigate to **Organization Settings $\to$ API Keys & Hub Provisioning**.
3. Generate a new Hub Enrollment Token and copy the `API_KEY` and `API_SECRET`.

---

## 2. Configuring `nexus.yaml`

Update `/etc/sentinel-nexus/nexus.yaml` with your cloud credentials:

```yaml
cloud_saas:
  enabled: true
  cloud_endpoint: "https://app.aryorithm.com"
  cloud_api_key: "ary_live_8f1c2a04d91242e1a084"
  cloud_api_secret: "sec_9e843c1294b13a7b41e2d901000000000000000000000000"
  nexus_id: "nexus-central-munich"
  sync_interval_seconds: 5
  enable_inbound_threat_feed: true
  enable_remote_ciso_commands: true
```

Restart `sentinel-nexus`:

```bash
sudo systemctl restart sentinel-nexus
```

---

## 3. Verifying Cloud Egress Synchronization

Monitor the service journal:

```bash
sudo journalctl -u sentinel-nexus -f | grep SaaSConnector
```

### Expected Output:
```text
[INFO] SaaSConnector: Authenticating with https://app.aryorithm.com...
[INFO] SaaSConnector: Authenticated successfully. Session token cached.
[INFO] SaaSConnector: 4-Tier Fleet Sync dispatched (4,992 Nodes, 14,800 Sensors). Status: 200 OK.
```

Log back into `app.aryorithm.com` to confirm that the full asset hierarchy and live telemetry are updating on your multi-tenant executive dashboard.
```

---

### File: `sentinel-nexus/docs/tutorials/air-gapped-sneakernet-sync.md`

```markdown
# Air-Gapped Sneakernet Telemetry & Model Synchronization

In classified defense installations and strictly segregated air-gapped facilities, network connections to outside networks are prohibited. 

`sentinel-nexus` supports **Sneakernet Operations**: exporting telemetry and importing signed models using removable physical storage media.

---

## 1. Sneakernet Workflow Overview

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ AIR-GAPPED ON-PREMISES ENCLAVE                              │
 │  - sentinel-nexus exports signed telemetry bundle to USB    │
 └──────────────────────────────┬──────────────────────────────┘
                                │ Physical Media Transfer
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ INTERNET-CONNECTED ANALYSIS WORKSTATION                     │
 │  - Uploads telemetry to app.aryorithm.com                   │
 │  - Retrains weights via Cloud Forge farm                    │
 │  - Downloads cryptographically signed network_threat_v3.onnx│
 └──────────────────────────────┬──────────────────────────────┘
                                │ Physical Media Return
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ AIR-GAPPED ON-PREMISES ENCLAVE                              │
 │  - sentinel-nexus verifies signature and deploys to fleet   │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. Step 1: Exporting Telemetry Bundles

On the air-gapped Nexus server, run `export_telemetry_bundle.py` to extract audit logs, performance traces, and curated active-learning CSVs onto an encrypted USB drive:

```bash
sudo python3 /opt/sentinel-nexus/scripts/export_telemetry_bundle.py \
    --output-dir /media/secure_usb/nexus_export_20261005 \
    --include-curated-datasets \
    --include-audit-logs
```

The script signs the bundle using the local TPM 2.0 Attestation Identity Key.

---

## 3. Step 2: Importing Signed Model Artifacts

After fine-tuning on the analysis workstation, copy the signed model package (`network_threat_v3.onnx` and `network_threat_v3.manifest.json`) to the USB drive.

On the air-gapped Nexus server, import and verify the package:

```bash
sudo python3 /opt/sentinel-nexus/scripts/import_signed_model.py \
    --model-path /media/secure_usb/network_threat_v3.onnx \
    --manifest-path /media/secure_usb/network_threat_v3.manifest.json
```

### Verification Output:
```text
[*] Validating /media/secure_usb/network_threat_v3.onnx...
[+] SHA-256 Digest     : 3a7b41e2d9010000e9a2c31e847b2c94b13a7b41e2... (MATCHED)
[+] Digital Signature  : VALID (Signed by Aryorithm Master Signing CA)
[+] Safety Gate Proof  : VERIFIED (100% Detection across Golden Attacks)
[+] Model imported to fleet repository. Initiating STAGE_SHADOW_MODE rollout!
```
```


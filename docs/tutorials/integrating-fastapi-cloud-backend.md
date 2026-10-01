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


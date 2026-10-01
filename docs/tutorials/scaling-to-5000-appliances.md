### Part 13: Practical Administrative Tutorials (`tutorials/*`)

This section contains 5 hands-on operational tutorials for `sentinel-nexus`: tuning Linux operating system parameters to scale to 5,000 appliances, establishing the mutual TLS (mTLS) Public Key Infrastructure, containing a multi-site zero-day attack in seconds, integrating with the cloud SaaS backend (`app.aryorithm.com`), and executing offline sneakernet model and telemetry synchronizations.

---

### File: `sentinel-nexus/docs/tutorials/scaling-to-5000-appliances.md`

```markdown
# Scaling to 5,000 Edge Appliances: Linux OS & gRPC Tuning

Managing up to 5,000 concurrent edge appliances over bidirectional HTTP/2 gRPC streaming connections requires tuning the Linux kernel networking stack, TCP buffer pools, and process file descriptor limits.

---

## 1. Operating System Kernel Tuning (`/etc/sysctl.d/99-nexus.conf`)

Deploy the following kernel network configuration parameters:

```ini
# Increase system-wide file descriptor ceiling
fs.file-max = 2097152

# Expand socket listen backlog queues for bursty reconnects
net.core.somaxconn = 65535
net.ipv4.tcp_max_syn_backlog = 65535

# Enforce high-throughput memory buffers (Min, Default, Max in bytes)
net.ipv4.tcp_rmem = 4096 87380 16777216
net.ipv4.tcp_wmem = 4096 65536 16777216
net.core.rmem_max = 16777216
net.core.wmem_max = 16777216

# Enable TCP BBR Congestion Control for low-latency WAN links
net.core.default_qdisc = fq
net.ipv4.tcp_congestion_control = bbr

# Expand ephemeral port range
net.ipv4.ip_local_port_range = 1024 65535

# Fast socket recycling
net.ipv4.tcp_fin_timeout = 15
net.ipv4.tcp_tw_reuse = 1
```

Apply the configuration immediately:

```bash
sudo sysctl --system
```

---

## 2. Process File Descriptor Limits (`/etc/security/limits.conf`)

Ensure the executing user can maintain over $10{,}000$ open sockets:

```text
root    soft    nofile    1048576
root    hard    nofile    1048576
```

---

## 3. gRPC Server Channel Arguments (`src/nexus/main.cpp`)

In the `sentinel-nexus` C++ initialization code, pass these channel arguments to optimize resource usage:

```cpp
grpc::ServerBuilder builder;
builder.AddListeningPort("0.0.0.0:50051", creds);

// Optimize for high-density, low-latency streaming
builder.AddChannelArgument(GRPC_ARG_MAX_CONCURRENT_STREAMS, 5000);
builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIME_MS, 10000);        // 10s ping
builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 5000);      // 5s timeout
builder.AddChannelArgument(GRPC_ARG_HTTP2_MIN_SENT_PING_INTERVAL_WITHOUT_DATA_MS, 5000);
```
```

---

### File: `sentinel-nexus/docs/tutorials/setting-up-mtls-pki.md`

```markdown
# Setting Up Mutual TLS (mTLS) PKI Infrastructure

Every edge appliance authenticates to `sentinel-nexus` using **Mutual TLS 1.3**. This tutorial demonstrates how to generate an offline Certificate Authority (CA), issue the Nexus server certificate, and sign edge appliance client keys using `gen_certs.sh`.

---

## 1. Public Key Infrastructure (PKI) Layout

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ Nexus Root Certificate Authority (ca.crt & ca.key)          │
 └──────────────────────────────┬──────────────────────────────┘
                                │
        ┌───────────────────────┴───────────────────────┐
        ▼ Signs Server Cert                             ▼ Signs Client Certs
 ┌─────────────────────────────┐         ┌─────────────────────────────┐
 │ Nexus Hub Certificate       │         │ Edge Appliance Certificates │
 │ (server.crt & server.key)   │         │ (appliance.crt / key)       │
 │ SAN: DNS:nexus.internal     │         │ CN: edge-substation-alpha   │
 └─────────────────────────────┘         └─────────────────────────────┘
```

---

## 2. Generating Certificates via `scripts/gen_certs.sh`

Execute the automated PKI setup script:

```bash
cd /opt/sentinel-nexus/scripts
chmod +x gen_certs.sh
sudo ./gen_certs.sh /etc/sentinel-nexus/certs
```

### Script Execution Steps:

```bash
# 1. Generate Root CA (Valid 10 Years)
openssl req -x509 -new -nodes -newkey rsa:4096 -days 3650 \
    -keyout /etc/sentinel-nexus/certs/ca.key \
    -out /etc/sentinel-nexus/certs/ca.crt \
    -subj "/C=DE/O=Aryorithm/CN=Sentinel-Nexus-Root-CA"

# 2. Generate Server Certificate with Subject Alternative Names (SAN)
openssl req -new -nodes -newkey rsa:2048 \
    -keyout /etc/sentinel-nexus/certs/server.key \
    -out /etc/sentinel-nexus/certs/server.csr \
    -subj "/C=DE/O=Aryorithm/CN=nexus.internal"

cat <<EOF > /tmp/server_ext.cnf
subjectAltName = DNS:nexus.internal,IP:10.240.0.10,IP:127.0.0.1
EOF

openssl x509 -req -days 1095 \
    -in /etc/sentinel-nexus/certs/server.csr \
    -CA /etc/sentinel-nexus/certs/ca.crt \
    -CAkey /etc/sentinel-nexus/certs/ca.key \
    -CAcreateserial \
    -out /etc/sentinel-nexus/certs/server.crt \
    -extfile /tmp/server_ext.cnf
```

---

## 3. Testing the mTLS Handshake with `grpcurl`

Test connection to the gRPC fleet endpoint:

```bash
grpcurl -cacert /etc/sentinel-nexus/certs/ca.crt \
        -cert /etc/sentinel-nexus/certs/client.crt \
        -key /etc/sentinel-nexus/certs/client.key \
        10.240.0.10:50051 list
```

### Expected Output
```text
grpc.health.v1.Health
sentinel.nexus.FleetOrchestrator
```
```

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


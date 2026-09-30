---

### File: `sentinel-nexus/docs/getting-started/docker-deployment.md`

```markdown
# Containerized Deployment & Docker Compose

For containerized environments and digital-twin cyber ranges (`sentinel-matrix`), `sentinel-nexus` can be deployed via Docker.

---

## 1. Docker Compose Configuration (`docker-compose.yml`)

```yaml
version: "3.8"

services:
  nexus:
    build:
      context: .
      dockerfile: Dockerfile
    image: aryorithm/sentinel-nexus:2.4.0
    container_name: sentinel-nexus
    restart: always
    network_mode: "host" # Recommended for low-latency line-rate gRPC
    volumes:
      - /opt/sentinel-nexus/config:/etc/sentinel-nexus
      - /opt/sentinel-nexus/certs:/etc/sentinel-nexus/certs:ro
      - /opt/sentinel-nexus/data:/var/lib/sentinel-nexus/data
      - /opt/sentinel-nexus/models:/var/lib/sentinel-nexus/models
      - /opt/sentinel-nexus/forge_datasets:/var/lib/sentinel-nexus/forge_datasets
    environment:
      - LOG_LEVEL=INFO
      - RUST_LOG=info
    healthcheck:
      test: ["CMD", "curl", "-k", "-f", "https://localhost:9443/api/v1/health"]
      interval: 10s
      timeout: 3s
      retries: 3
```

---

## 2. Running the Container

```bash
# Build and run in background
docker compose up -d

# Check live logs
docker compose logs -f nexus
```
```

---

### File: `sentinel-nexus/docs/getting-started/verifying-services.md`

```markdown
# Verifying Services & Port Status

Confirm that all three network services exposed by `sentinel-nexus` are bound and accepting traffic.

---

## 1. Network Port Verification

Run `ss` or `netstat` to verify listener bindings:

```bash
sudo ss -tulpn | grep -E '50051|9443|9444'
```

### Expected Output
```text
tcp   LISTEN 0      4096   0.0.0.0:50051   0.0.0.0:*   users:(("sentinel-nexus",pid=12040,fd=14))
tcp   LISTEN 0      128    0.0.0.0:9443    0.0.0.0:*   users:(("sentinel-nexus",pid=12040,fd=18))
tcp   LISTEN 0      128    0.0.0.0:9444    0.0.0.0:*   users:(("sentinel-nexus",pid=12040,fd=22))
```

---

## 2. Verifying the REST API & Web Command Center (Port 9443)

Query the health endpoint:

```bash
curl -k -s https://localhost:9443/api/v1/health | jq .
```

### Response:
```json
{
  "status": "HEALTHY",
  "version": "2.4.0",
  "active_appliances": 1,
  "collective_defense_bus": "ARMED",
  "active_model_sha256": "e9a2c31e847b2c94b13a7b41e2d9010000000000000000000000000000000000"
}
```

---

## 3. Testing the Real-Time SSE Stream (Port 9444)

Listen to the continuous Server-Sent Events stream:

```bash
curl -N http://localhost:9444/stream
```

### Stream Output:
```text
event: fleet_tick
data: {"timestamp_ns":1791172800184000000,"online_nodes":1,"total_drops_today":1420,"fleet_sla_us":0.82}
```
```

---

### File: `sentinel-nexus/docs/getting-started/architecture-at-a-glance.md`

```markdown
# Architecture at a Glance

The diagram below maps the interaction between edge defense appliances (`blackbox-sentinel`), the central command hub (`sentinel-nexus`), continual AI retraining (`xinfer-forge`), and cloud visibility (`app.aryorithm.com`).

---

```text
 ┌──────────────────────────────────────────────────────────────────────────────────────────┐
 │ CLOUD SAAS PORTAL: app.aryorithm.com (Optional Multi-Tenant Executive Dashboard)         │
 └──────────────────────────────────────────▲───────────────────────────────────────────────┘
                                            │ Outbound HTTPS Sync (POST /api/v1/fleet/sync)
                                            │ Decoupled SaaSConnector ($0.00 Egress Local)
 ┌──────────────────────────────────────────┴───────────────────────────────────────────────┐
 │ SENTINEL-NEXUS CENTRAL COMMAND PLANE (Tier 6 Hub)                                        │
 │                                                                                          │
 │  ┌──────────────────────────────────────────────┐  ┌──────────────────────────────────┐  │
 │  │ Sub-50ms Collective Defense Bus              │  │ DatasetCurator (Active Learning) │  │
 │  │ • Fans out FleetDefenseRules to 5,000 nodes  │  │ • Curation window: [0.40 - 0.60] │  │
 │  │ • Originator loopback suppression            │  │ • Emits forge_dataset_*.csv      │  │
 │  └──────────────────────┬───────────────────────┘  └────────────────┬─────────────────┘  │
 │                         │                                           │                    │
 │                         │                                           ▼ File Watcher       │
 │                         │                      ┌──────────────────────────────────────┐  │
 │                         │                      │ Tier 4: xinfer-forge Continual AI    │  │
 │                         │                      │ • Self-Supervised Tabular MAE        │  │
 │                         │                      │ • Golden Attacks Safety Gate (100%)  │  │
 │                         │                      │ • Automated ONNX Opset 17 Export     │  │
 │                         │                      └────────────────────┬─────────────────┘  │
 │                         │                                           │                    │
 │                         ▼ Bi-Directional mTLS Channels              ▼ POST /ota/stage    │
 │  ┌──────────────────────────────────────────────────────────────────┴─────────────────┐  │
 │  │ gRPC Fleet Service (Port 50051)                                                    │  │
 │  │ • StreamFleetRules (< 50ms propagation) • SubmitHeartbeats • Canary OTA Updates    │  │
 │  └──────────────────────┬─────────────────────────────────────────────────────────────┘  │
 └─────────────────────────┼────────────────────────────────────────────────────────────────┘
                           │
        ┌──────────────────┼──────────────────┐ Parallel Distribution
        ▼                  ▼                  ▼ (Up to 5,000 Nodes)
 ┌──────────────┐   ┌──────────────┐   ┌──────────────┐
 │ Appliance 1  │   │ Appliance 2  │   │ Appliance N  │
 │ (Substation) │   │ (Water Plant)│   │ (Hospital)   │
 └──────────────┘   └──────────────┘   └──────────────┘
```
```


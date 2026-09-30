---

### File: `sentinel-nexus/docs/architecture/air-gapped-sovereignty.md`

```markdown
# Air-Gapped Data Sovereignty & $0.00 Cloud Egress

`sentinel-nexus` is architected for complete air-gapped sovereignty. In municipal water utilities, nuclear power stations, and defense operations centers, the platform functions autonomously without WAN connectivity.

---

## 1. Sovereign On-Premises Topology

```text
 ┌─────────────────────────────────────────────────────────────┐
 │ AIR-GAPPED INDUSTRIAL FACILITY                              │
 │                                                             │
 │  ┌───────────────────────┐        ┌───────────────────────┐ │
 │  │ blackbox-sentinel (1) │        │ blackbox-sentinel (N) │ │
 │  └───────────┬───────────┘        └───────────┬───────────┘ │
 │              │ Internal Network (10.240.0.0/24)│            │
 │              └────────────────┬───────────────┘             │
 │                               ▼                             │
 │               ┌───────────────────────────────┐             │
 │               │ sentinel-nexus Fleet Hub      │             │
 │               │ • Port 50051: Fleet gRPC      │             │
 │               │ • Port 9443: Web Console      │             │
 │               │ • Port 9444: Real-Time SSE    │             │
 │               │ • 100% On-Premises Execution  │             │
 │               └───────────────────────────────┘             │
 └─────────────────────────────────────────────────────────────┘
                                 ║
                                 ╫ PHYSICAL AIR-GAP (NO INTERNET ROUTE)
                                 ║
                         [ PUBLIC CLOUD ]
```

---

## 2. Data Sovereignty Guarantees

* **Zero Telemetry Leakage:** Ingress network flows, raw PCAP files, and asset metadata remain inside the customer's on-premises boundary.
* **$0.00 Cloud Egress Fees:** Fleet coordination, active learning curation, and Canary model rollouts execute entirely over local Ethernet/fiber links.
* **Zero External Dependencies:** The web command center embeds all HTML, CSS, JavaScript, and SVG vector graphics directly in the C++ binary—making zero outbound requests to external CDNs.
```

---

### File: `sentinel-nexus/docs/architecture/high-availability-clustering.md`

```markdown
# High-Availability Clustering & Failover Topology

In mission-critical infrastructure, a single point of failure in the fleet command plane is unacceptable. `sentinel-nexus` supports **Active-Standby High-Availability (HA) Clustering** managed via a Virtual IP (VIP) and state replication.

---

## 1. Active-Standby Failover Architecture

```text
                           [ Virtual IP (VIP): 10.240.0.10 ]
                                         │
                 ┌───────────────────────┴───────────────────────┐
                 ▼ (Active Node)                                 ▼ (Standby Node)
 ┌───────────────────────────────┐               ┌───────────────────────────────┐
 │ sentinel-nexus Primary        │               │ sentinel-nexus Secondary      │
 │ • State: LEADER (Active)      │               │ • State: FOLLOWER (Warm)      │
 │ • Holds VIP (10.240.0.10)     │               │ • Monitors Primary Heartbeat  │
 └───────────────┬───────────────┘               └───────────────▲───────────────┘
                 │                                               │
                 └──────── Synchronous State Journaling ─────────┘
                   (nexus_state.json synced over private link)
```

---

## 2. Failover Protocol Sequence

1. **Health Monitoring:** The secondary Nexus instance polls the primary instance over a dedicated heartbeat link every $500\,\text{ms}$.
2. **Failure Detection:** If the primary instance fails to respond for $1{,}500\,\text{ms}$ (3 missed heartbeats):
   * The secondary instance promotes itself to **`LEADER`**.
   * The secondary instance issues an **Arp Gratuitous** broadcast claiming the Virtual IP (`10.240.0.10`).
3. **Seamless Appliance Reconnection:** All 5,000 edge appliances (`blackbox-sentinel`) reconnect to the VIP within $2.0\text{ seconds}$ without configuration changes.
```


### Part 8: Air-Gapped Web Command Center (Ports 9443 & 9444) (`web-command-center/*`)

This section contains 7 technical implementation guides detailing the air-gapped web management interface in `sentinel-nexus`: the zero-CDN Single-Page Application (SPA) architecture, the HTML5 Canvas radial topology visualizer, the dynamic MITRE ATT&CK heatmap, the sub-10ms Server-Sent Events (SSE) telemetry stream, active KPI summary cards, the manual collective defense broadcast UI, and interactive Canary OTA rollout controls.

---

### File: `sentinel-nexus/docs/web-command-center/web-console-architecture.md`

```markdown
# Air-Gapped Web Console Architecture (Port 9443 HTTPS)

The `sentinel-nexus` Web Command Center provides security operations centers (SOCs) and industrial plant managers with centralized visibility over 5,000 edge appliances. Built as an embedded Single-Page Application (SPA), it operates with **zero external CDN dependencies**, eliminating external data leakage and loading delays in air-gapped environments.

---

## 1. Web Serving & Data Flow Architecture

```text
 [ Web Browser / SOC Video Wall ]
                 │
                 ├── HTTPS TLS 1.3 (Port 9443): REST API & HTML5 Assets
                 │
                 └── HTTP EventStream (Port 9444): Real-Time SSE Stream (< 10ms Push)
                 │
                 ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ sentinel-nexus Embedded Web Engine                          │
 ├─────────────────────────────────────────────────────────────┤
 │ • Port 9443: Static Asset Router & REST API (/api/v1/*)     │
 │ • Port 9444: Dedicated SSE Push Server (SseBroadcaster.cpp)│
 │ • Security : Strict Content-Security-Policy (CSP)           │
 └──────────────────────────────┬──────────────────────────────┘
                                │ In-Process State Access
                                ▼
 ┌─────────────────────────────────────────────────────────────┐
 │ NodeRegistry, CollectiveDefenseBus, and XaiAggregator       │
 └─────────────────────────────────────────────────────────────┘
```

---

## 2. The Zero-CDN Guarantee

In compliance with sovereign defense and critical infrastructure standards:
* **No External Scripts:** All client scripts (`app.js`, `fleet_topology.js`, `threat_matrix.js`) are compiled directly into the C++ binary as gzip-compressed byte arrays.
* **No Remote Fonts or Stylesheets:** Typography relies on system font stacks (`system-ui`, `-apple-system`, `Segoe UI`, `Roboto`), avoiding requests to Google Fonts or Typekit.
* **Vector Graphics:** Icons and device markers use inline SVG vectors, requiring no font-based icon CDNs (e.g., FontAwesome).
```

---

### File: `sentinel-nexus/docs/web-command-center/radial-topology-canvas.md`

```markdown
# Real-Time Radial Topology Canvas (`fleet_topology.js`)

The fleet topology is rendered on an HTML5 Canvas using a **multi-ring radial layout**. This visualization maps the 4-tier asset hierarchy, rendering up to 5,000 appliances and 25,000 industrial sensors at 60 FPS without DOM overhead.

---

## 1. Multi-Ring Radial Geometry

```text
                            RING 3: Sensors & Field PLCs (Outer Perimeter)
                                    • • • • • • • • • •
                                 •                       •
                         RING 2: Blackbox-Sentinel Appliances
                                 ┌───────────────┐
                              •  │  Appliance 1  │  •
                                 └───────┬───────┘
                                         │
                             RING 1:  [NEXUS] (Central Hub)
                                         │
                                 ┌───────┴───────┐
                              •  │  Appliance 2  │  •
                                 └───────────────┘
                                 •                       •
                                    • • • • • • • • • •
```

* **Center Node (Ring 0):** `sentinel-nexus` Central Hub.
* **Intermediate Ring (Ring 1):** Edge Appliances (`blackbox-sentinel`), colored by health state (`#00E5FF` Online, `#FFB300` Degraded, `#FF1744` Unreachable).
* **Outer Cluster (Ring 2):** Field Sensors (Modbus PLCs, S7 controllers, DICOM scanners).
* **Animated Threat Arcs:** When a node mitigates an attack, a red vector pulse arcs toward the center hub, followed by a cyan fanout wave across the fleet ring.

---

## 2. Canvas Rendering Engine (`static/js/fleet_topology.js`)

```javascript
class FleetRadialCanvas {
    constructor(canvasId) {
        this.canvas = document.getElementById(canvasId);
        this.ctx = this.canvas.getContext('2d');
        this.nodes = [];
        this.threatArcs = [];
        this.resize();
        window.addEventListener('resize', () => this.resize());
    }

    resize() {
        this.canvas.width = this.canvas.parentElement.clientWidth;
        this.canvas.height = this.canvas.parentElement.clientHeight;
        this.centerX = this.canvas.width / 2;
        this.centerY = this.canvas.height / 2;
        this.radius = Math.min(this.centerX, this.centerY) * 0.8;
    }

    draw(nodes) {
        this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);

        // Draw Central Hub Node
        this.ctx.beginPath();
        this.ctx.arc(this.centerX, this.centerY, 18, 0, 2 * Math.PI);
        this.ctx.fillStyle = '#00E5FF';
        this.ctx.fill();

        // Draw Fleet Radial Nodes
        const total = nodes.length;
        nodes.forEach((node, idx) => {
            const angle = (idx / total) * 2 * Math.PI;
            const x = this.centerX + this.radius * Math.cos(angle);
            const y = this.centerY + this.radius * Math.sin(angle);

            // Connective Line
            this.ctx.beginPath();
            this.ctx.moveTo(this.centerX, this.centerY);
            this.ctx.lineTo(x, y);
            this.ctx.strokeStyle = 'rgba(0, 229, 255, 0.15)';
            this.ctx.stroke();

            // Edge Node Marker
            this.ctx.beginPath();
            this.ctx.arc(x, y, 6, 0, 2 * Math.PI);
            this.ctx.fillStyle = node.status === 'ONLINE' ? '#00E5FF' : '#FF1744';
            this.ctx.fill();
        });
    }
}
```
```

---

### File: `sentinel-nexus/docs/web-command-center/mitre-attack-heatmap.md`

```markdown
# Dynamic MITRE ATT&CK Matrix Heatmap (`threat_matrix.js`)

The Web Command Center provides a dynamic **MITRE ATT&CK for Enterprise and ICS** tactical matrix. Cells update in real time as attack vectors are intercepted across the fleet.

---

## 1. Tactical Matrix Layout

```text
 ┌─────────────────┬─────────────────┬─────────────────┬─────────────────┐
 │ Initial Access  │ Execution       │ Persistence     │ Impair Control  │
 ├─────────────────┼─────────────────┼─────────────────┼─────────────────┤
 │ T1190           │ T1059           │ T1543           │ T0855           │
 │ Exploit Public  │ Command Script  │ Create Service  │ Unauthorized Cmd│
 │ [ 12 Events ]   │ [ 4 Events ]    │ [ 0 Events ]    │ [ 1,420 Drops ] │
 │                 │                 │                 │ (HOT RED CELL)  │
 ├─────────────────┼─────────────────┼─────────────────┼─────────────────┤
 │ T1133           │ T1203           │ T1547           │ T0831           │
 │ External Remote │ Client Exploit  │ Boot Autostart  │ Manipulate Ctrl │
 │ [ 2 Events ]    │ [ 0 Events ]    │ [ 0 Events ]    │ [ 842 Drops ]   │
 └─────────────────┴─────────────────┴─────────────────┴─────────────────┘
```

---

## 2. Dynamic Heatmap Scaling

Matrix cells are shaded dynamically based on rolling incident frequency ($N_{\text{incidents}}$):

$$\text{Shade Intensity} = \min\left(1.0,\, \frac{N_{\text{incidents}}}{50}\right)$$

* **Zero Incidents:** Dark Slate Gray (`#1E222B`).
* **Low Activity ($1 - 10$ events):** Amber Warning (`#FFB300`).
* **High Activity ($> 50$ events):** Saturated Red Alert (`#FF1744`).

Clicking any cell opens the **XAI Root-Cause Drawer**, displaying the top-3 feature attributions for that specific MITRE technique.
```

---

### File: `sentinel-nexus/docs/web-command-center/real-time-sse-stream.md`

```markdown
# Real-Time Telemetry Push Engine (Port 9444 SSE)

Instead of using resource-intensive client polling, `sentinel-nexus` streams real-time updates over **Server-Sent Events (SSE)** on port **9444**.

---

## 1. SSE Stream Protocol Specification

* **Endpoint:** `GET http://<NEXUS_HOST>:9444/stream`
* **Content-Type:** `text/event-stream; charset=utf-8`
* **Cache-Control:** `no-cache`
* **Connection:** `keep-alive`

```text
event: fleet_tick
data: {"timestamp_ns":1791172800184000000,"online_nodes":142,"drops_today":41209,"fleet_sla_us":0.82}

event: threat_drop
data: {"incident_id":"inc-1802","src_ip":"198.51.100.42","mitre_id":"T0855","action":"XDP_DROP","latency_us":0.81}

event: collective_rule_injected
data: {"rule_id":1042,"target_ip":"198.51.100.42","ttl_seconds":3600,"fanout_nodes":141}
```

---

## 2. C++20 Server-Side Broadcaster (`SseBroadcaster.cpp`)

```cpp
#include <string>
#include <vector>
#include <mutex>
#include <sys/socket.h>

namespace sentinel::nexus {

class SseBroadcaster {
public:
    void register_client(int client_socket) {
        std::lock_guard lock(mutex_);
        clients_.push_back(client_socket);
    }

    void broadcast(std::string_view event_type, std::string_view json_data) {
        std::string payload = "event: " + std::string(event_type) + "\n" +
                              "data: " + std::string(json_data) + "\n\n";

        std::lock_guard lock(mutex_);
        for (auto it = clients_.begin(); it != clients_.end();) {
            ssize_t sent = ::send(*it, payload.data(), payload.size(), MSG_NOSIGNAL);
            if (sent < 0) {
                // Client disconnected: clean up socket
                ::close(*it);
                it = clients_.erase(it);
            } else {
                ++it;
            }
        }
    }

private:
    std::mutex mutex_;
    std::vector<int> clients_;
};

} // namespace sentinel::nexus
```
```

---

### File: `sentinel-nexus/docs/web-command-center/active-kpi-telemetry-cards.md`

```markdown
# Executive KPI Telemetry Cards & Live Metrics

The top navigation of the Web Command Center renders active Key Performance Indicator (KPI) telemetry cards, refreshed at 10 Hz from the SSE stream.

---

## 1. Dashboard KPI Cards Layout

```text
 ┌────────────────────┐ ┌────────────────────┐ ┌────────────────────┐ ┌────────────────────┐
 │ FLEET APPLIANCES   │ │ IN-KERNEL DROPS    │ │ MITIGATION SLA     │ │ ACTIVE MODEL       │
 │ 4,992 / 5,000      │ │ 1,420,891 pkts     │ │ 0.82 µs (Median)   │ │ network_threat_v2  │
 │ Status: 99.8% UP   │ │ Rate: 42,100 pps   │ │ p99 SLA: < 0.84 µs │ │ Status: FLEET-WIDE │
 └────────────────────┘ └────────────────────┘ └────────────────────┘ └────────────────────┘
```

---

## 2. Telemetry Card Invariants

* **Fleet Appliances:** Displays total registered nodes, active online connections, and degraded instances.
* **In-Kernel Drops:** Aggregates cumulative packets purged by Tier 2 `xdp_filter.o` across all edge nodes.
* **Mitigation SLA:** Displays median ($p50$) and 99th-percentile ($p99$) response times. Turns red if the fleet SLA exceeds $1.0\,\mu\text{s}$.
* **Active Model:** Displays the currently enforced ONNX model version, SHA-256 fingerprint, and rollout state (`SHADOW`, `CANARY`, `FLEET_WIDE`).
```

---

### File: `sentinel-nexus/docs/web-command-center/collective-defense-injector-ui.md`

```markdown
# Collective Defense Manual Injector UI

The Web Command Center includes a manual **Collective Defense Injector**, allowing security operators to broadcast emergency IP block rules to all 5,000 edge appliances with a single click.

---

## 1. Injector User Interface

```text
 ┌─────────────────────────────────────────────────────────────────────────────┐
 │ MANUAL COLLECTIVE DEFENSE INJECTOR                                          │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ Target IPv4 Address    : [ 198.51.100.42                 ]                  │
 │ Ephemeral TTL (Seconds): [ 3600                          ] (1 Hour)         │
 │ Threat Classification  : [ EMERGENCY_OPERATOR_BLOCK      ]                  │
 │ MITRE Technique ID     : [ T0855                         ]                  │
 │ Justification / Ticket : [ INC-8941: Active Wellhead Attack ]              │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ [ BROADCAST IMMUNITY FLEET-WIDE (< 50ms) ]        [ PURGE / UNBLOCK IP ]    │
 └─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Client-Side Submission Handler (`app.js`)

```javascript
async function broadcastManualThreat() {
    const payload = {
        target_ip: document.getElementById('target_ip').value,
        ttl_seconds: parseInt(document.getElementById('ttl_seconds').value, 10),
        threat_name: document.getElementById('threat_name').value,
        mitre_id: document.getElementById('mitre_id').value,
        justification: document.getElementById('justification').value
    };

    const response = await fetch('/api/v1/threats/broadcast', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${getAuthToken()}`
        },
        body: JSON.stringify(payload)
    });

    const result = await response.json();
    if (response.ok) {
        showToast(`Rule broadcast to ${result.dispatched_nodes} appliances in ${result.elapsed_ms}ms!`, 'success');
    } else {
        showToast(`Broadcast failed: ${result.error}`, 'error');
    }
}
```
```

---

### File: `sentinel-nexus/docs/web-command-center/ota-canary-management-ui.md`

```markdown
# Interactive OTA Canary Management & Staging UI

The **Model Rollout Panel** allows operators to stage, evaluate, advance, and roll back neural network weights across the fleet.

---

## 1. Rollout Management Console

```text
 ┌─────────────────────────────────────────────────────────────────────────────┐
 │ NEURAL MODEL ROLLOUT CONTROLLER                                             │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ Candidate Version : network_threat_v2_canary.onnx                           │
 │ SHA-256 Checksum  : e9a2c31e847b2c94b13a7b41e2d9010000000000000000...       │
 │ Current Stage     : STAGE 2: CANARY_5_PCT (250 / 5,000 Appliances)          │
 │ Canary Health     : Latency p50: 0.82 µs | p99: 0.84 µs (0 SLA Breaches)   │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ PROGRESS: [████████████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░] 5% Cohort Active  │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ ACTIONS:                                                                    │
 │  [ PROMOTE TO FLEET-WIDE (100%) ]       [ EMERGENCY ABORT & ROLLBACK ]      │
 └─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Interactive Rollback Protection

* **1-Click Rollback:** Clicking **`[EMERGENCY ABORT & ROLLBACK]`** instantly triggers `RollbackGuard`, sending a rollback instruction across all active Canary appliances in $< 50\,\text{ms}$.
* **Health Safeguard:** If Canary nodes report latency exceeding $1{,}000\,\mu\text{s}$, the UI displays an alert banner, disables manual promotion buttons, and initiates an automatic rollback.
```

---

### Complete in Part 8
- `sentinel-nexus/docs/web-command-center/web-console-architecture.md`
- `sentinel-nexus/docs/web-command-center/radial-topology-canvas.md`
- `sentinel-nexus/docs/web-command-center/mitre-attack-heatmap.md`
- `sentinel-nexus/docs/web-command-center/real-time-sse-stream.md`
- `sentinel-nexus/docs/web-command-center/active-kpi-telemetry-cards.md`
- `sentinel-nexus/docs/web-command-center/collective-defense-injector-ui.md`
- `sentinel-nexus/docs/web-command-center/ota-canary-management-ui.md`

All 7 Web Command Center documentation files for `sentinel-nexus` are now generated.

---

### Files to be Generated in Part 9

The next phase covers the **Operations CLI (`nexus-ctl`)** (`operations-cli-nexus-ctl/` - 6 files):

1. `operations-cli-nexus-ctl/nexus-ctl-overview.md` (C++20 terminal admin tool syntax and flags)
2. `operations-cli-nexus-ctl/command-fleet-list.md` (`nexus-ctl fleet list`: Node health, CPU, drops, latency SLA)
3. `operations-cli-nexus-ctl/command-threat-drop.md` (`nexus-ctl threat drop <IP>`: Manual fleet-wide kernel drop)
4. `operations-cli-nexus-ctl/command-ota-management.md` (`nexus-ctl ota [status|stage|advance|rollback]`)
5. `operations-cli-nexus-ctl/command-compliance-reports.md` (`nexus-ctl report [cmmc|scada]`: Audit evaluation checks)
6. `operations-cli-nexus-ctl/command-auth-login.md` (`nexus-ctl auth login [email] [pass]`: Cloud JWT acquisition)

Confirm when you are ready to proceed with Part 9.
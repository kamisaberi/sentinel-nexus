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


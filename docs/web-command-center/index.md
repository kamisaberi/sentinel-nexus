# Web Command Center (Port 9443 HTTPS & 9444 SSE)

The Web Command Center serves an embedded administrative management interface on port 9443 with an air-gapped **Zero-CDN guarantee**.

## Section Navigation

- [Web Console Architecture](./web-console-architecture.md): Embedded C++20 web engine, TLS 1.3 termination, and zero-CDN asset compilation.
- [Radial Topology Canvas](./radial-topology-canvas.md): HTML5 Canvas multi-ring radial rendering engine (`fleet_topology.js`) running at 60 FPS.
- [MITRE ATT&CK Heatmap](./mitre-attack-heatmap.md): Dynamic tactical matrix (`threat_matrix.js`) shaded by rolling incident frequency.
- [Real-Time SSE Stream](./real-time-sse-stream.md): Server-Sent Events push engine (`SseBroadcaster.cpp`) streaming 100Hz telemetry over port 9444.
- [Active KPI Telemetry Cards](./active-kpi-telemetry-cards.md): Top-level KPI cards displaying node counts, drop rates, and p99 SLA status.
- [Collective Defense Injector UI](./collective-defense-injector-ui.md): 1-click manual threat rule broadcast interface.
- [OTA Canary Management UI](./ota-canary-management-ui.md): Interactive model staging, cohort progress tracking, and emergency rollback controls.

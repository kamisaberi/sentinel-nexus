# 4-Tier Hierarchical Enterprise Asset Model

`sentinel-nexus` models physical and logical infrastructure using a 4-tier relational topology (`Tenant` $\to$ `Nexus Hub` $\to$ `Sentinel Node` $\to$ `Field Sensor`).

## Section Navigation

- [4-Tier Hierarchy Model](./four-tier-hierarchy-model.md): Enterprise relational tree, node isolation rules, and cascading rollups.
- [Deterministic Sensor Identifiers](./deterministic-sensor-identifiers.md): Deriving persistent sensor IDs from physical network coordinates and protocol attributes.
- [Cascading Health Engine](./cascading-health-engine.md): Node health states (`ONLINE`, `DEGRADED`, `UNREACHABLE`, `OFFLINE`) and tree propagation.
- [Liveness Heartbeat Tracking](./liveness-heartbeat-tracking.md): Non-blocking liveness sweeper (`LivenessTracker.cpp`) enforcing 15s timeout windows.
- [Instant 0ms Disconnect](./instant-0ms-graceful-disconnect.md): Processing synchronous `DeregisterAppliance` RPCs to eliminate phantom SOC alerts.
- [Fleet Sync Payload Schema](./fleet-sync-payload-schema.md): Nested JSON specification transmitted every 5 seconds via `POST /api/v1/fleet/sync`.

# Cloud SaaS Uplink (`SaaSConnector.cpp`)

`SaaSConnector` is an optional, non-blocking outbound HTTPS client that connects on-premises `sentinel-nexus` hubs to `app.aryorithm.com`.

## Section Navigation

- [SaaS Connector Architecture](./saas-connector-architecture.md): Outbound-only pipeline, socket timeout invariants, and deadlock prevention.
- [JWT Authentication & Renewal](./jwt-authentication-and-renewal.md): Authentication sequence, local session caching, and 401 token renewal.
- [Cloud Fleet Sync](./cloud-saas-uplink/cloud-fleet-sync.md): High-frequency 5-second fleet sync (`POST /api/v1/fleet/sync`).
- [Inbound Global Threat Feed](./inbound-global-threat-feed.md): Polling zero-day threat indicators from the Aryorithm Global Defense Consortium.
- [Remote CISO Commands](./remote-ciso-commands.md): Polling and executing cryptographically signed administrative commands (`/commands/pending`).

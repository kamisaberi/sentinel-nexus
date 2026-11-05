# REST API Reference

The `sentinel-nexus` REST API operates over HTTPS TLS 1.3 on port 9443, providing integration interfaces for SOAR platforms and web dashboards.

## Section Navigation

- [REST API Overview](./api-overview.md): Base URLs, standard HTTP request headers, and JSON error response schemas.
- [Fleet Endpoints](./endpoints-fleet.md): `GET /api/v1/fleet/nodes` and `/api/v1/fleet/nodes/{uuid}` for fleet inventory queries.
- [Threat Endpoints](./endpoints-threats.md): `POST /api/v1/threats/broadcast` and `/api/v1/threats/mitre` for collective defense operations.
- [OTA & Model Endpoints](./endpoints-ota-models.md): `POST /api/v1/ota/stage`, `/api/v1/ota/advance`, and `/api/v1/ota/rollback`.
- [Compliance Endpoints](./endpoints-compliance.md): `GET /api/v1/reports/compliance`, `/api/v1/reports/cmmc`, and `/api/v1/reports/scada`.
- [SSE Stream Endpoint](./endpoints-sse-stream.md): Persistent real-time telemetry stream (`GET /api/v1/telemetry/stream` on port 9444).

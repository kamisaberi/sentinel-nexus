# Troubleshooting & Diagnostic Help Desk

Diagnostic procedures, error code remediations, and support channels for resolving `sentinel-nexus` operational issues.

## Section Navigation

- [Port Binding & Socket Errors](./port-binding-and-socket-errors.md): Diagnosing `EADDRINUSE` port collisions across ports 50051, 9443, and 9444.
- [Registration Rejections](./appliance-registration-rejections.md): Resolving gRPC unauthenticated errors and TPM PCR 0 quote rejections.
- [JWT Auth 401 Failures](./jwt-auth-401-failures.md): Troubleshooting REST API authentication errors and `SaaSConnector` mutex re-entrancy.
- [SSE Stream Disconnects](./sse-stream-disconnects.md): Fixing proxy buffering issues (Nginx `proxy_buffering off`) and keepalive timeouts.
- [Config Parsing Errors](./config-file-parsing-errors.md): Resolving YAML parser inline comment stripping bugs in `ConfigManager.cpp`.
- [Technical FAQ](./faq.md): Frequently asked technical questions regarding sub-50ms fanouts, air-gapped operation, and HA failovers.
- [Support & SLAs](./support.md): Generating automated diagnostic bundles (`nexus-ctl diag --bundle`) and enterprise commercial SLAs.

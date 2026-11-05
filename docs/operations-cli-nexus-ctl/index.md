# Operations CLI (`nexus-ctl`)

`nexus-ctl` is a native ISO C++20 command-line administration utility providing programmatic control over the fleet orchestrator and collective defense grid.

## Section Navigation

- [nexus-ctl Overview](./nexus-ctl-overview.md): Global options, command tree summary, and local session caching (`~/.nexus/session.json`).
- [nexus-ctl fleet list](./command-fleet-list.md): Querying active managed node inventories, status filters, and JSON output formatting.
- [nexus-ctl threat drop](./command-threat-drop.md): Broadcasting emergency IP drop rules fleet-wide via the sub-50ms defense bus.
- [nexus-ctl ota](./command-ota-management.md): Inspecting Canary rollouts, staging models, advancing phases, and executing rollbacks.
- [nexus-ctl report](./command-compliance-reports.md): Compiling automated compliance scorecards (CMMC Level 2, IEC 62443, EU NIS 2).
- [nexus-ctl auth login](./command-auth-login.md): Authenticating operators, processing MFA tokens, and caching Bearer JWT leases.

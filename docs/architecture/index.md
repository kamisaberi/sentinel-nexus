# Architecture & Low-Level Design

`sentinel-nexus` is implemented in native ISO C++20 and designed to manage up to 5,000 concurrent edge appliances without thread contention or dynamic runtime allocations in the routing fast path.

## Section Navigation

- [Command Plane Architecture](./command-plane-architecture.md): Multi-threaded core engine, gRPC completion queue worker pool, and thread isolation rules.
- [NEXUS_ENV Security Gate](./nexus-env-runtime-gate.md): Mode isolation (`production` vs `simulation`), DOM-level attack button stripping, and 403 Forbidden enforcement.
- [Ports & Protocols Matrix](./ports-and-protocols-matrix.md): Port allocations (50051 gRPC, 9443 HTTPS REST, 9444 SSE Stream) and transport security.
- [In-Memory State Engine](./in-memory-state-engine.md): Microsecond state access via `NodeRegistry` and `std::shared_mutex` reader-writer locks.
- [Data Persistence Model](./data-persistence-model.md): Atomic JSON state journaling (`nexus_state.json`) and the 24-hour circular `TimeSeriesEngine`.
- [Air-Gapped Sovereignty](./air-gapped-sovereignty.md): On-premises operational invariants, zero external network calls, and $0.00 cloud egress.
- [High-Availability Clustering](./high-availability-clustering.md): Active-Standby HA clustering, Virtual IP (VIP) failover, and gratuitous ARP.

# Getting Started

Welcome to the **Sentinel-Nexus (`sentinel-nexus`)** Getting Started guide. This section provides complete walkthroughs for deploying the Tier 6 central fleet command plane and collective defense grid.

## Section Navigation

- [Overview & Mission](./overview.md): Central command plane introduction, $0.00 cloud egress, and collective immunity principles.
- [System Requirements](./system-requirements.md): Hardware sizing guidelines for up to 5,000 nodes, Linux kernel requirements, and port matrices.
- [Installation & Build](./installation-and-build.md): Compiling `sentinel-nexus`, `nexus-ctl`, and dependencies using CMake and Ninja.
- [10-Minute Quickstart](./ten-minute-quickstart.md): Generating test mTLS certificates, booting the daemon, and connecting an edge node.
- [Systemd Deployment](./systemd-deployment.md): Deploying the hardened production systemd unit file with real-time priority (`Nice=-10`).
- [Docker Deployment](./docker-deployment.md): Running containerized `sentinel-nexus` inside digital-twin testbeds (`sentinel-matrix`).
- [Verifying Services](./verifying-services.md): Validating port bindings (50051, 9443, 9444), REST APIs, and real-time SSE streams.
- [Architecture at a Glance](./architecture-at-a-glance.md): Visual topology diagram illustrating the 4-tier asset hierarchy and cloud uplink.

# Active Learning Pipeline

`sentinel-nexus` acts as the telemetry curator in the Aryorithm active learning flywheel, collecting ambiguous vectors ($0.40 \le f(x) \le 0.60$) and coordinating continual retraining with `xinfer-forge`.

## Section Navigation

- [Active Learning Architecture](./active-learning-architecture.md): Edge vector ingestion, dataset curation, and closed-loop retraining flywheel.
- [Vector Ingest Queue](./vector-ingest-queue.md): Multi-Producer Single-Consumer (MPSC) lock-free ring buffer (200k capacity).
- [Uncertainty Sampling Rules](./uncertainty-sampling-rules.md): Information entropy gating selecting high-gain boundary vectors ($0.40 - 0.60$).
- [Dataset Curator Engine](./dataset-curator-engine.md): Batch packaging engine (`DatasetCurator.cpp`) emitting CSV batches and SHA-256 manifests.
- [Forge Trigger Automation](./forge-trigger-automation.md): Linux `inotify` kernel triggers alerting `xinfer-forge` to launch adaptation passes.
- [Closed-Loop Flywheel Testing](./closed-loop-flywheel-testing.md): Automated python integration test verifying the full flywheel cycle.

# Explainable AI (XAI) Attribution Aggregator

`sentinel-nexus` aggregates Microsecond Residual Decomposition (MRD) records computed in under $80\,\text{ns}$ by edge appliances, translating tensor deviations into human-readable physical parameters.

## Section Navigation

- [XAI Aggregator Overview](./xai-aggregator-overview.md): Ingestion pipeline, semantic dictionary un-scaling, and central correlation.
- [Top-3 Feature Attribution Schema](./top-3-feature-attribution-schema.md): JSON schema specification for rank-ordered feature contribution records.
- [Semantic Dictionary Mapping](./semantic-dictionary-mapping.md): Mapping 32-dim tensor coordinates to physical engineering units (PSI, RPM, pps).
- [Global Threat Cache Indexing](./global-threat-cache-indexing.md): Thread-safe bitmap index aligning XAI attributions with MITRE ATT&CK for Enterprise and ICS.
- [XAI API Endpoints](./xai-api-endpoints.md): Auditor REST endpoints (`GET /api/v1/threats/xai`) for querying explainable telemetry.
- [Rendering XAI in Web & TUI](./rendering-xai-in-web-and-tui.md): Live visualization using SVG impact bars in the Web Console and Terminal TUI.

# Collective Defense Grid

The Collective Defense Grid operates under the architectural invariant: *"Attacked Once, Immune Everywhere."* When any edge appliance drops an attack, Nexus broadcasts the threat indicator fleet-wide in $< 50\,\text{ms}$.

## Section Navigation

- [Collective Defense Overview](./collective-defense-overview.md): Sub-50ms immunity paradigm, the propagation cycle, and architectural invariants.
- [IoC Broadcaster Mechanics](./ioc-broadcaster-mechanics.md): Asynchronous parallel gRPC distribution engine (`IocBroadcaster.cpp`) over HTTP/2 streams.
- [Sub-50ms Fanout Timeline](./sub-50ms-fanout-timeline.md): Step-by-step microsecond and millisecond timing budget for global rule propagation.
- [Originator Loop Suppression](./originator-loop-suppression.md): Loopback suppression algorithms and sliding-window broadcast deduplication filters.
- [Fleet Defense Rule Schema](./fleet-defense-rule-schema.md): Protobuf specification (`sentinel_nexus.proto`) and ephemeral TTL management.
- [Emergency IP Purge](./emergency-ip-purge.md): Executing global false-positive IP purges across 5,000 edge kernel maps in $< 50\,\text{ms}$.

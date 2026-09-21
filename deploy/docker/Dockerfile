# Stage 1: Build binary in C++20 container
FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    protobuf-compiler \
    libprotobuf-dev \
    libgrpc++-dev \
    protobuf-compiler-grpc \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# Stage 2: Minimal runtime image
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libprotobuf-dev \
    libgrpc++ \
    libssl3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/sentinel-nexus

# Copy binaries and configs
COPY --from=builder /app/build/sentinel-nexus /usr/local/bin/sentinel-nexus
COPY --from=builder /app/build/nexus-ctl /usr/local/bin/nexus-ctl
COPY --from=builder /app/configs ./configs
COPY --from=builder /app/web ./web

EXPOSE 50051 9443 9444

ENTRYPOINT ["sentinel-nexus"]
CMD ["configs/nexus.yaml"]
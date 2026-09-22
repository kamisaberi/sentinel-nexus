cmake_minimum_required(VERSION 3.20)
project(sentinel-nexus VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    -Wall -Wextra -Wpedantic
    -Wno-unused-parameter
    -O3 -march=native
    -pthread
)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(PROTO_SRCS
    ${PROTO_DIR}/common.proto
    ${PROTO_DIR}/fleet.proto
    ${PROTO_DIR}/telemetry.proto
    ${PROTO_DIR}/intelligence.proto
    ${PROTO_DIR}/model_ota.proto
)

set(GENERATED_PROTO_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GENERATED_PROTO_DIR})

set(PROTO_GEN_HDRS)
set(PROTO_GEN_SRCS)
set(GRPC_GEN_HDRS)
set(GRPC_GEN_SRCS)

foreach(proto_file ${PROTO_SRCS})
    get_filename_component(proto_name ${proto_file} NAME_WLE)
    set(proto_hdr "${GENERATED_PROTO_DIR}/${proto_name}.pb.h")
    set(proto_src "${GENERATED_PROTO_DIR}/${proto_name}.pb.cc")
    set(grpc_hdr  "${GENERATED_PROTO_DIR}/${proto_name}.grpc.pb.h")
    set(grpc_src  "${GENERATED_PROTO_DIR}/${proto_name}.grpc.pb.cc")
    
    list(APPEND PROTO_GEN_HDRS ${proto_hdr})
    list(APPEND PROTO_GEN_SRCS ${proto_src})
    list(APPEND GRPC_GEN_HDRS  ${grpc_hdr})
    list(APPEND GRPC_GEN_SRCS  ${grpc_src})
    
    add_custom_command(
        OUTPUT ${proto_hdr} ${proto_src} ${grpc_hdr} ${grpc_src}
        COMMAND protobuf::protoc
            --proto_path=${PROTO_DIR}
            --cpp_out=${GENERATED_PROTO_DIR}
            --grpc_out=${GENERATED_PROTO_DIR}
            --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
            ${proto_file}
        DEPENDS ${proto_file} protobuf::protoc gRPC::grpc_cpp_plugin
        COMMENT "Compiling Protobuf & gRPC: ${proto_name}.proto"
    )
endforeach()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${GENERATED_PROTO_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

set(NEXUS_SOURCES
    src/main.cpp
    src/core/ConfigManager.cpp
    src/fleet/NodeRegistry.cpp
    src/fleet/AttestationValidator.cpp
    src/fleet/GroupManager.cpp
    src/fleet/HeartbeatMonitor.cpp
    src/telemetry/ForgeBridge.cpp
    src/telemetry/VectorIngestQueue.cpp
    src/telemetry/UncertaintySampler.cpp
    src/telemetry/DatasetCurator.cpp
    src/telemetry/ForgeTrigger.cpp
    src/intelligence/IocBroadcaster.cpp
    src/intelligence/GlobalThreatCache.cpp
    src/intelligence/MitreAggregator.cpp
    src/ota/CanaryOrchestrator.cpp
    src/ota/RollbackGuard.cpp
    src/ota/ModelRepository.cpp
    src/reporting/ReportGenerator.cpp
    src/reporting/CmmcAuditEngine.cpp
    src/reporting/ScadaAuditEngine.cpp
    src/reporting/LatencySlaReporter.cpp
    src/storage/StateDatabase.cpp
    src/storage/TimeSeriesEngine.cpp
    src/api/HttpServer.cpp
    src/api/TelemetryStreamer.cpp
    src/rpc/FleetServiceImpl.cpp
    src/rpc/TelemetryServiceImpl.cpp
    src/rpc/IntelligenceServiceImpl.cpp
    src/rpc/ModelOtaServiceImpl.cpp
    ${PROTO_GEN_SRCS}
    ${GRPC_GEN_SRCS}
)
# 1. Main Daemon Target
add_executable(sentinel-nexus ${NEXUS_SOURCES})
target_link_libraries(sentinel-nexus
    PRIVATE
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
        protobuf::libprotobuf
        gRPC::grpc++
        gRPC::grpc++_reflection
)

# 2. Operations CLI Target
add_executable(nexus-ctl tools/nexus-ctl/main.cpp)
target_link_libraries(nexus-ctl PRIVATE Threads::Threads)

# 3. High-Throughput Ingestion Benchmark Target
add_executable(benchmark_ingest 
    tests/benchmark/benchmark_ingest.cpp
    src/telemetry/ForgeBridge.cpp
    ${PROTO_GEN_SRCS}
)
target_link_libraries(benchmark_ingest PRIVATE Threads::Threads protobuf::libprotobuf)

# 4. Core Unit Test Suite Target
add_executable(test_nexus_core
    tests/unit/test_nexus_core.cpp
    src/fleet/NodeRegistry.cpp
    src/fleet/AttestationValidator.cpp
    src/fleet/GroupManager.cpp
    src/ota/CanaryOrchestrator.cpp
    src/ota/RollbackGuard.cpp
    ${PROTO_GEN_SRCS}
)
target_link_libraries(test_nexus_core PRIVATE Threads::Threads OpenSSL::Crypto protobuf::libprotobuf)

install(TARGETS sentinel-nexus nexus-ctl DESTINATION /usr/local/bin)
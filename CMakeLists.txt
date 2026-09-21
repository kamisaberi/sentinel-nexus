cmake_minimum_required(VERSION 3.20)
project(sentinel-nexus VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optimization & Compiler Flags
add_compile_options(
    -Wall -Wextra -Wpedantic
    -Wno-unused-parameter
    -O3 -march=native
    -pthread
)

# Find Required Dependencies
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Protobuf & gRPC Code Generation
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

# Include Paths
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${GENERATED_PROTO_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

# Source Files
set(NEXUS_SOURCES
src/main.cpp
    src/core/ConfigManager.cpp
    src/fleet/NodeRegistry.cpp
    src/fleet/AttestationValidator.cpp
    src/telemetry/ForgeBridge.cpp
    src/intelligence/IocBroadcaster.cpp
    src/intelligence/GlobalThreatCache.cpp
    src/ota/CanaryOrchestrator.cpp
    src/reporting/ReportGenerator.cpp
    src/storage/StateDatabase.cpp
    src/api/HttpServer.cpp
    src/rpc/FleetServiceImpl.cpp
    src/rpc/TelemetryServiceImpl.cpp
    src/rpc/IntelligenceServiceImpl.cpp
    src/rpc/ModelOtaServiceImpl.cpp
    ${PROTO_GEN_SRCS}
    ${GRPC_GEN_SRCS}
)

# Executable Target
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

install(TARGETS sentinel-nexus DESTINATION /usr/local/bin)
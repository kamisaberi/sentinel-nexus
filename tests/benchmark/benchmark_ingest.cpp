#include <iostream>
#include <chrono>
#include <vector>
#include "telemetry/ForgeBridge.hpp"

int main() {
    std::cout << "========================================================\n"
              << "  SENTINEL-NEXUS FORGE BRIDGE INGESTION BENCHMARK\n"
              << "========================================================\n";

    auto& bridge = sentinel::nexus::telemetry::ForgeBridge::instance();
    bridge.initialize("/tmp/benchmark_forge_buffer", 0.40f, 0.60f, 5000);

    const size_t TOTAL_VECTORS = 1'000'000;
    std::vector<::sentinel::nexus::CandidateVector> test_data;
    test_data.reserve(TOTAL_VECTORS);

    std::cout << "[*] Pre-generating " << TOTAL_VECTORS << " synthetic NetFlow vectors..." << std::endl;
    for (size_t i = 0; i < TOTAL_VECTORS; ++i) {
        ::sentinel::nexus::CandidateVector vec;
        vec.set_event_id(i);
        vec.set_timestamp_ns(1700000000ULL + i);
        vec.set_inference_uncertainty(0.45f); // Falls into active learning range
        vec.set_autoencoder_recon_loss(0.80f);
        vec.set_triggered_kernel_drop(i % 10 == 0);
        for (int f = 0; f < 32; ++f) {
            vec.add_features(0.123f);
        }
        test_data.push_back(std::move(vec));
    }

    std::cout << "[*] Commencing zero-overhead ingestion loop..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    size_t ingested = 0;
    for (const auto& vec : test_data) {
        if (bridge.ingest_vector(vec)) {
            ingested++;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    double vectors_per_sec = static_cast<double>(ingested) / elapsed.count();

    std::cout << "\n[+] Benchmark Results:\n"
              << "    Total Vectors Ingested : " << ingested << "\n"
              << "    Elapsed Time           : " << elapsed.count() << " seconds\n"
              << "    Sustained Throughput   : " << static_cast<uint64_t>(vectors_per_sec) << " vectors/sec\n"
              << "========================================================\n";

    return 0;
}
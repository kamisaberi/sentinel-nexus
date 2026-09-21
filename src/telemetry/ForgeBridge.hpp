#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <filesystem>
#include "telemetry.pb.h"

namespace sentinel::nexus::telemetry {

struct ForgeSample {
    uint64_t timestamp_ns;
    std::vector<float> features;
    float uncertainty;
    float recon_loss;
    bool kernel_drop;
};

class ForgeBridge {
public:
    static ForgeBridge& instance() {
        static ForgeBridge inst;
        return inst;
    }

    void initialize(const std::string& buffer_directory, 
                    float min_uncertainty = 0.40f, 
                    float max_uncertainty = 0.60f,
                    size_t batch_flush_size = 1000);

    bool ingest_vector(const ::sentinel::nexus::CandidateVector& vector);
    void flush_batch_to_disk();

    size_t buffered_count() const;
    uint64_t total_forwarded_to_forge() const { return total_forwarded_; }

private:
    ForgeBridge() = default;
    
    std::filesystem::path buffer_dir_;
    float min_uncertainty_{0.40f};
    float max_uncertainty_{0.60f};
    size_t batch_flush_size_{1000};

    mutable std::mutex buffer_mutex_;
    std::vector<ForgeSample> sample_buffer_;
    uint64_t total_forwarded_{0};
};

} // namespace sentinel::nexus::telemetry
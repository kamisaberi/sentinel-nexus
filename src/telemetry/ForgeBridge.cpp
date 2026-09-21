#include "ForgeBridge.hpp"
#include "core/Logger.hpp"
#include <chrono>

namespace sentinel::nexus::telemetry {

void ForgeBridge::initialize(const std::string& buffer_directory, 
                             float min_uncertainty, 
                             float max_uncertainty,
                             size_t batch_flush_size) {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    buffer_dir_ = buffer_directory;
    min_uncertainty_ = min_uncertainty;
    max_uncertainty_ = max_uncertainty;
    batch_flush_size_ = batch_flush_size;

    std::error_code ec;
    std::filesystem::create_directories(buffer_dir_, ec);
    if (ec) {
        NEXUS_LOG_ERROR("Failed to create Forge buffer directory: " + ec.message());
    } else {
        NEXUS_LOG_INFO("ForgeBridge initialized. Buffer path: " + buffer_dir_.string());
    }
}

bool ForgeBridge::ingest_vector(const CandidateVector& vector) {
    // Selection Criteria:
    // 1. Prediction uncertainty is within the active learning window [min_unc, max_unc]
    // 2. High autoencoder reconstruction loss (novelty/outlier)
    // 3. Traffic caused an eBPF kernel drop
    bool is_uncertain = (vector.inference_uncertainty() >= min_uncertainty_ &&
                         vector.inference_uncertainty() <= max_uncertainty_);
    bool is_outlier = (vector.autoencoder_recon_loss() > 0.75f);
    bool is_verified_drop = vector.triggered_kernel_drop();

    if (!is_uncertain && !is_outlier && !is_verified_drop) {
        return false; // Skip redundant, standard benign vectors
    }

    std::unique_lock<std::mutex> lock(buffer_mutex_);
    
    ForgeSample sample;
    sample.timestamp_ns = vector.timestamp_ns();
    sample.features.assign(vector.features().begin(), vector.features().end());
    sample.uncertainty = vector.inference_uncertainty();
    sample.recon_loss = vector.autoencoder_recon_loss();
    sample.kernel_drop = vector.triggered_kernel_drop();

    sample_buffer_.push_back(std::move(sample));
    total_forwarded_++;

    if (sample_buffer_.size() >= batch_flush_size_) {
        lock.unlock();
        flush_batch_to_disk();
    }

    return true;
}

void ForgeBridge::flush_batch_to_disk() {
    std::vector<ForgeSample> to_flush;
    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);
        if (sample_buffer_.empty()) return;
        to_flush.swap(sample_buffer_);
    }

    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    std::string filename = "candidate_batch_" + std::to_string(now) + ".bin";
    std::filesystem::path filepath = buffer_dir_ / filename;

    std::ofstream out(filepath, std::ios::binary);
    if (!out.is_open()) {
        NEXUS_LOG_ERROR("ForgeBridge could not write batch to " + filepath.string());
        return;
    }

    // Binary wire layout per sample:
    // [uint64 timestamp] [uint32 num_features] [float32 * num_features] [float uncertainty] [float recon_loss] [uint8 drop]
    for (const auto& sample : to_flush) {
        uint64_t ts = sample.timestamp_ns;
        uint32_t num_feat = static_cast<uint32_t>(sample.features.size());
        uint8_t drop = sample.kernel_drop ? 1 : 0;

        out.write(reinterpret_cast<const char*>(&ts), sizeof(ts));
        out.write(reinterpret_cast<const char*>(&num_feat), sizeof(num_feat));
        if (num_feat > 0) {
            out.write(reinterpret_cast<const char*>(sample.features.data()), num_feat * sizeof(float));
        }
        out.write(reinterpret_cast<const char*>(&sample.uncertainty), sizeof(sample.uncertainty));
        out.write(reinterpret_cast<const char*>(&sample.recon_loss), sizeof(sample.recon_loss));
        out.write(reinterpret_cast<const char*>(&drop), sizeof(drop));
    }

    out.close();
    NEXUS_LOG_INFO("Flushed " + std::to_string(to_flush.size()) + " active-learning vectors to: " + filepath.string());
}

size_t ForgeBridge::buffered_count() const {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    return sample_buffer_.size();
}

} // namespace sentinel::nexus::telemetry
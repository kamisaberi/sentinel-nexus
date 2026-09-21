#include "DatasetCurator.hpp"
#include "core/Logger.hpp"
#include <fstream>
#include <sstream>
#include <chrono>

namespace sentinel::nexus::telemetry {

void DatasetCurator::initialize(const std::string& buffer_dir, const std::string& output_dir) {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_dir_ = buffer_dir;
    output_dir_ = output_dir;

    std::error_code ec;
    std::filesystem::create_directories(output_dir_, ec);
    NEXUS_LOG_INFO("DatasetCurator initialized. Output directory: " + output_dir_.string());
}

bool DatasetCurator::curate_training_dataset(CuratedDatasetInfo& out_info) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::filesystem::path> batch_files;
    for (const auto& entry : std::filesystem::directory_iterator(buffer_dir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin") {
            batch_files.push_back(entry.path());
        }
    }

    if (batch_files.empty()) {
        NEXUS_LOG_INFO("No new candidate batch files available for curation.");
        return false;
    }

    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    std::string dataset_filename = "forge_dataset_" + std::to_string(now_ms) + ".csv";
    std::string manifest_filename = "forge_dataset_" + std::to_string(now_ms) + ".manifest.json";

    std::filesystem::path dataset_path = output_dir_ / dataset_filename;
    std::filesystem::path manifest_path = output_dir_ / manifest_filename;

    std::ofstream csv(dataset_path);
    if (!csv.is_open()) {
        NEXUS_LOG_ERROR("Could not open " + dataset_path.string() + " for writing.");
        return false;
    }

    // CSV Header (32 features + metadata)
    for (int f = 0; f < 32; ++f) csv << "f" << f << ",";
    csv << "uncertainty,recon_loss,kernel_drop\n";

    size_t total_samples = 0;
    size_t high_unc = 0;
    size_t drop_count = 0;

    for (const auto& file_path : batch_files) {
        std::ifstream bin(file_path, std::ios::binary);
        if (!bin.is_open()) continue;

        while (bin.peek() != EOF) {
            uint64_t ts;
            uint32_t num_feat;
            bin.read(reinterpret_cast<char*>(&ts), sizeof(ts));
            bin.read(reinterpret_cast<char*>(&num_feat), sizeof(num_feat));

            if (num_feat != 32) break; // Consistency check

            std::vector<float> features(num_feat);
            bin.read(reinterpret_cast<char*>(features.data()), num_feat * sizeof(float));

            float uncertainty, recon_loss;
            uint8_t kernel_drop;
            bin.read(reinterpret_cast<char*>(&uncertainty), sizeof(uncertainty));
            bin.read(reinterpret_cast<char*>(&recon_loss), sizeof(recon_loss));
            bin.read(reinterpret_cast<char*>(&kernel_drop), sizeof(kernel_drop));

            for (float val : features) csv << val << ",";
            csv << uncertainty << "," << recon_loss << "," << static_cast<int>(kernel_drop) << "\n";

            total_samples++;
            if (uncertainty >= 0.40f && uncertainty <= 0.60f) high_unc++;
            if (kernel_drop == 1) drop_count++;
        }
        bin.close();
        
        // Remove processed batch file
        std::error_code ec;
        std::filesystem::remove(file_path, ec);
    }
    csv.close();

    // Write manifest JSON
    std::ofstream manifest(manifest_path);
    manifest << "{\n"
             << "  \"dataset_file\": \"" << dataset_path.string() << "\",\n"
             << "  \"total_samples\": " << total_samples << ",\n"
             << "  \"high_uncertainty_samples\": " << high_unc << ",\n"
             << "  \"kernel_drop_samples\": " << drop_count << ",\n"
             << "  \"feature_dim\": 32,\n"
             << "  \"ready_for_forge\": true\n"
             << "}\n";
    manifest.close();

    out_info.dataset_file = dataset_path.string();
    out_info.manifest_file = manifest_path.string();
    out_info.total_samples = total_samples;
    out_info.high_uncertainty_count = high_unc;
    out_info.kernel_drop_count = drop_count;

    NEXUS_LOG_INFO("Curated " + std::to_string(total_samples) + 
                   " samples for xinfer-forge at: " + dataset_path.string());
    return true;
}

} // namespace sentinel::nexus::telemetry
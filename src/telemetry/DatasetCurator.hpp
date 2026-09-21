#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <mutex>

namespace sentinel::nexus::telemetry {

struct CuratedDatasetInfo {
    std::string dataset_file;
    std::string manifest_file;
    size_t total_samples;
    size_t high_uncertainty_count;
    size_t kernel_drop_count;
};

class DatasetCurator {
public:
    static DatasetCurator& instance() {
        static DatasetCurator inst;
        return inst;
    }

    void initialize(const std::string& buffer_dir, const std::string& output_dir);

    // Converts accumulated .bin batch files into a standardized CSV/manifest dataset for xinfer-forge
    bool curate_training_dataset(CuratedDatasetInfo& out_info);

private:
    DatasetCurator() = default;

    std::filesystem::path buffer_dir_{"/tmp/sentinel_nexus_forge"};
    std::filesystem::path output_dir_{"/var/lib/sentinel-nexus/forge_datasets"};
    mutable std::mutex mutex_;
};

} // namespace sentinel::nexus::telemetry
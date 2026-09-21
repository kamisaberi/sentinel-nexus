#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <mutex>

namespace sentinel::nexus::ota {

struct StoredModel {
    std::string filename;
    std::string full_path;
    std::string sha256_hash;
    uint64_t file_size_bytes;
    bool is_valid_onnx;
};

class ModelRepository {
public:
    static ModelRepository& instance() {
        static ModelRepository inst;
        return inst;
    }

    void initialize(const std::string& repository_dir = "models");
    bool scan_repository();

    std::vector<StoredModel> list_models() const;
    bool get_model(const std::string& filename, StoredModel& out_model) const;
    std::string calculate_file_sha256(const std::filesystem::path& file_path) const;
    std::string generate_models_json() const;

private:
    ModelRepository() = default;

    std::filesystem::path repo_dir_{"models"};
    mutable std::mutex mutex_;
    std::vector<StoredModel> cached_models_;
};

} // namespace sentinel::nexus::ota
#include "ModelRepository.hpp"
#include "core/Logger.hpp"
#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace sentinel::nexus::ota {

void ModelRepository::initialize(const std::string& repository_dir) {
    std::lock_guard<std::mutex> lock(mutex_);
    repo_dir_ = repository_dir;

    std::error_code ec;
    std::filesystem::create_directories(repo_dir_, ec);

    // Create a default placeholder model if directory is empty
    std::filesystem::path sample_v1 = repo_dir_ / "network_threat_v1.onnx";
    if (!std::filesystem::exists(sample_v1)) {
        std::ofstream dummy(sample_v1, std::ios::binary);
        dummy << "ONNX_SENTINEL_WEIGHTS_V1_STABLE";
        dummy.close();
    }

    scan_repository();
    NEXUS_LOG_INFO("ModelRepository initialized at: " + repo_dir_.string());
}

std::string ModelRepository::calculate_file_sha256(const std::filesystem::path& file_path) const {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) return "";

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    char buffer[32768];
    while (file.read(buffer, sizeof(buffer))) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }
    if (file.gcount() > 0) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    std::ostringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

bool ModelRepository::scan_repository() {
    std::vector<StoredModel> found;

    if (!std::filesystem::exists(repo_dir_)) return false;

    for (const auto& entry : std::filesystem::directory_iterator(repo_dir_)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            std::string sha = calculate_file_sha256(entry.path());
            uint64_t size = std::filesystem::file_size(entry.path());

            found.push_back({
                .filename = filename,
                .full_path = entry.path().string(),
                .sha256_hash = sha,
                .file_size_bytes = size,
                .is_valid_onnx = true
            });
        }
    }

    cached_models_ = std::move(found);
    return true;
}

std::vector<StoredModel> ModelRepository::list_models() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return cached_models_;
}

bool ModelRepository::get_model(const std::string& filename, StoredModel& out_model) const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& m : cached_models_) {
        if (m.filename == filename) {
            out_model = m;
            return true;
        }
    }
    return false;
}

std::string ModelRepository::generate_models_json() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < cached_models_.size(); ++i) {
        const auto& m = cached_models_[i];
        ss << "  {\n"
           << "    \"filename\": \"" << m.filename << "\",\n"
           << "    \"sha256\": \"" << m.sha256_hash << "\",\n"
           << "    \"size_bytes\": " << m.file_size_bytes << ",\n"
           << "    \"download_url\": \"/models/" << m.filename << "\"\n"
           << "  }" << (i + 1 < cached_models_.size() ? ",\n" : "\n");
    }
    ss << "]";
    return ss.str();
}

} // namespace sentinel::nexus::ota
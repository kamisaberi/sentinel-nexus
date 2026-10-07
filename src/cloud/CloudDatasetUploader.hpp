#pragma once
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <filesystem>

namespace sentinel::nexus::cloud {

struct UploadTask {
    std::string dataset_csv_path;
    std::string manifest_json_path;
    bool is_exclusive_tenant;
    std::string tenant_id;
};

struct UploadResult {
    bool success;
    std::string task_id;
    size_t bytes_uploaded;
    std::string server_response;
};

class CloudDatasetUploader {
public:
    static CloudDatasetUploader& instance() {
        static CloudDatasetUploader inst;
        return inst;
    }

    void initialize(const std::string& cloud_endpoint, 
                    const std::string& api_key, 
                    const std::string& tenant_id);

    // Enqueue an active learning dataset for asynchronous cloud upload
    bool enqueue_dataset(const std::string& csv_file, 
                         const std::string& manifest_file, 
                         bool is_exclusive = true);

    size_t pending_queue_size() const;
    void stop();

private:
    CloudDatasetUploader() = default;
    void worker_loop(std::stop_token st);
    UploadResult execute_multipart_upload(const UploadTask& task);

    std::string cloud_endpoint_{"http://127.0.0.1:8000/api/v1"};
    std::string api_key_{""};
    std::string default_tenant_id_{"tenant-dev-local"};

    std::atomic<bool> running_{false};
    mutable std::mutex queue_mutex_;
    std::queue<UploadTask> task_queue_;
    std::jthread worker_thread_;
};

} // namespace sentinel::nexus::cloud
#include "CloudDatasetUploader.hpp"
#include "core/Logger.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cstdlib>

namespace sentinel::nexus::cloud {

void CloudDatasetUploader::initialize(const std::string& cloud_endpoint, 
                                     const std::string& api_key, 
                                     const std::string& tenant_id) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    cloud_endpoint_ = cloud_endpoint;
    api_key_ = api_key;
    default_tenant_id_ = tenant_id;

    if (!running_.load()) {
        running_.store(true);
        worker_thread_ = std::jthread([this](std::stop_token st) { worker_loop(st); });
        NEXUS_LOG_INFO("CloudDatasetUploader initialized for destination: " + cloud_endpoint_);
    }
}

void CloudDatasetUploader::stop() {
    running_.store(false);
}

bool CloudDatasetUploader::enqueue_dataset(const std::string& dataset_file, 
                                          const std::string& manifest_file, 
                                          bool is_exclusive) {
    if (!std::filesystem::exists(dataset_file)) {
        NEXUS_LOG_WARN("Cannot enqueue non-existent dataset: " + dataset_file);
        return false;
    }

    UploadTask task{
        .dataset_file_path = dataset_file,
        .manifest_json_path = manifest_file,
        .is_exclusive_tenant = is_exclusive,
        .tenant_id = default_tenant_id_
    };

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        task_queue_.push(std::move(task));
    }

    NEXUS_LOG_INFO("Queued dataset for Remote GPU Forge training: " + dataset_file);
    return true;
}

size_t CloudDatasetUploader::pending_queue_size() const {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    return task_queue_.size();
}

void CloudDatasetUploader::worker_loop(std::stop_token st) {
    while (!st.stop_requested() && running_.load()) {
        UploadTask current_task;
        bool has_task = false;

        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            if (!task_queue_.empty()) {
                current_task = task_queue_.front();
                task_queue_.pop();
                has_task = true;
            }
        }

        if (has_task) {
            UploadResult res = execute_multipart_upload(current_task);
            if (res.success) {
                NEXUS_LOG_INFO("Remote upload complete for: " + current_task.dataset_file_path + 
                               " (" + std::to_string(res.bytes_uploaded) + " bytes)");
            } else {
                NEXUS_LOG_WARN("Remote upload failed for: " + current_task.dataset_file_path + 
                               " -> " + res.server_response.substr(0, 100));
            }
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
}

UploadResult CloudDatasetUploader::execute_multipart_upload(const UploadTask& task) {
    UploadResult result{false, "", 0, ""};

    if (!std::filesystem::exists(task.dataset_file_path)) {
        result.server_response = "Dataset file not found";
        return result;
    }

    size_t file_size = std::filesystem::file_size(task.dataset_file_path);
    result.bytes_uploaded = file_size;

    // Use robust curl multipart dispatch supporting both HTTP and HTTPS endpoints
    std::string resp_file = "/tmp/nexus_uploader_resp.txt";
    std::string auth_header = !api_key_.empty() ? "-H 'Authorization: Bearer " + api_key_ + "' " : "";

    std::string curl_cmd = "curl -sSL -w '\n%{http_code}' -X POST '" + cloud_endpoint_ + "' " +
                           auth_header +
                           "-H 'X-Tenant-ID: " + task.tenant_id + "' " +
                           "-F 'dataset=@" + task.dataset_file_path + "' " +
                           "-F 'is_exclusive=" + (task.is_exclusive_tenant ? "true" : "false") + "' " +
                           "> " + resp_file + " 2>&1";

    int ret = std::system(curl_cmd.c_str());
    (void)ret;

    std::ifstream in(resp_file);
    std::string line, last_line, full_body;
    while (std::getline(in, line)) {
        if (!line.empty()) {
            last_line = line;
            full_body += line + "\n";
        }
    }
    in.close();
    std::filesystem::remove(resp_file);

    int http_code = 0;
    try {
        http_code = std::stoi(last_line);
    } catch (...) {}

    result.server_response = full_body;
    result.success = (http_code == 200 || http_code == 201);
    return result;
}

} // namespace sentinel::nexus::cloud
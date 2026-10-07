#include "CloudDatasetUploader.hpp"
#include "core/Logger.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

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

bool CloudDatasetUploader::enqueue_dataset(const std::string& csv_file, 
                                          const std::string& manifest_file, 
                                          bool is_exclusive) {
    if (!std::filesystem::exists(csv_file)) {
        NEXUS_LOG_WARN("Cannot enqueue non-existent dataset: " + csv_file);
        return false;
    }

    UploadTask task{
        .dataset_csv_path = csv_file,
        .manifest_json_path = manifest_file,
        .is_exclusive_tenant = is_exclusive,
        .tenant_id = default_tenant_id_
    };

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        task_queue_.push(std::move(task));
    }

    NEXUS_LOG_INFO("Queued dataset for Cloud GPU Forge training: " + csv_file);
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
                NEXUS_LOG_INFO("Cloud upload complete for: " + current_task.dataset_csv_path + 
                               " (" + std::to_string(res.bytes_uploaded) + " bytes)");
            } else {
                NEXUS_LOG_WARN("Cloud upload failed for: " + current_task.dataset_csv_path + 
                               " -> " + res.server_response.substr(0, 100));
            }
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
}

UploadResult CloudDatasetUploader::execute_multipart_upload(const UploadTask& task) {
    UploadResult result{false, "", 0, ""};

    // Read CSV file contents
    std::ifstream csv_in(task.dataset_csv_path, std::ios::binary);
    if (!csv_in.is_open()) {
        result.server_response = "Cannot open CSV file";
        return result;
    }
    std::string csv_content((std::istreambuf_iterator<char>(csv_in)), std::istreambuf_iterator<char>());
    csv_in.close();

    // Read Manifest if present
    std::string manifest_content = "{}";
    if (std::filesystem::exists(task.manifest_json_path)) {
        std::ifstream mf_in(task.manifest_json_path);
        if (mf_in.is_open()) {
            manifest_content.assign((std::istreambuf_iterator<char>(mf_in)), std::istreambuf_iterator<char>());
        }
    }

    std::string boundary = "----AryorithmFormBoundary7MA4YWxkTrZu0gW";
    std::string filename = std::filesystem::path(task.dataset_csv_path).filename().string();

    // Build Multipart Body
    std::ostringstream body;
    // Field: is_exclusive
    body << "--" << boundary << "\r\n"
         << "Content-Disposition: form-data; name=\"is_exclusive\"\r\n\r\n"
         << (task.is_exclusive_tenant ? "true" : "false") << "\r\n";
    // Field: manifest
    body << "--" << boundary << "\r\n"
         << "Content-Disposition: form-data; name=\"manifest_json\"\r\n\r\n"
         << manifest_content << "\r\n";
    // Field: file
    body << "--" << boundary << "\r\n"
         << "Content-Disposition: form-data; name=\"dataset_file\"; filename=\"" << filename << "\"\r\n"
         << "Content-Type: text/csv\r\n\r\n"
         << csv_content << "\r\n"
         << "--" << boundary << "--\r\n";

    std::string body_str = body.str();
    result.bytes_uploaded = body_str.size();

    // Parse endpoint host/port
    std::string host = "127.0.0.1";
    uint16_t port = 8000;
    std::string path = "/api/v1/ai/forge/datasets/upload";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        result.server_response = "Socket creation failed";
        return result;
    }

    struct timeval tv{5, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr);
    serv_addr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sock);
        result.server_response = "Connection to cloud backend failed";
        return result;
    }

    std::ostringstream req;
    req << "POST " << path << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << "X-Tenant-ID: " << task.tenant_id << "\r\n"
        << "Content-Type: multipart/form-data; boundary=" << boundary << "\r\n"
        << "Content-Length: " << body_str.size() << "\r\n"
        << "Connection: close\r\n\r\n"
        << body_str;

    std::string req_str = req.str();
    send(sock, req_str.data(), req_str.size(), 0);

    char buf[4096];
    std::string resp;
    ssize_t bytes;
    while ((bytes = recv(sock, buf, sizeof(buf) - 1, 0)) > 0) {
        buf[bytes] = '\0';
        resp.append(buf, bytes);
        if (resp.find("\r\n\r\n") != std::string::npos && resp.size() > 500) break;
    }
    close(sock);

    result.server_response = resp;
    result.success = (resp.find("200 OK") != std::string::npos || resp.find("201 Created") != std::string::npos);
    return result;
}

} // namespace sentinel::nexus::cloud
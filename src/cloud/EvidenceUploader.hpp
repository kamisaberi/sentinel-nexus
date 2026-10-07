#pragma once
#include <string>
#include <mutex>
#include <queue>
#include <thread>
#include <atomic>

namespace sentinel::nexus::cloud {

struct EvidencePackage {
    std::string incident_id;
    std::string pcap_filepath;
    std::string sha256_hash;
    std::string tenant_id;
};

class EvidenceUploader {
public:
    static EvidenceUploader& instance() {
        static EvidenceUploader inst;
        return inst;
    }

    void initialize(const std::string& cloud_url, const std::string& tenant_id);
    void queue_evidence(const std::string& incident_id, const std::string& filepath, const std::string& sha256);
    void stop();

private:
    EvidenceUploader() = default;
    void worker_loop(std::stop_token st);

    std::string cloud_url_{"http://127.0.0.1:8000/api/v1"};
    std::string tenant_id_{"tenant-dev-local"};
    std::atomic<bool> running_{false};

    mutable std::mutex queue_mutex_;
    std::queue<EvidencePackage> pending_queue_;
    std::jthread worker_thread_;
};

} // namespace sentinel::nexus::cloud
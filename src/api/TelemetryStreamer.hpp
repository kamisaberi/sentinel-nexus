#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <queue>
#include <sys/socket.h>

namespace sentinel::nexus::api {

class TelemetryStreamer {
public:
    static TelemetryStreamer& instance() {
        static TelemetryStreamer inst;
        return inst;
    }

    // Registers an open HTTP client socket for Server-Sent Events (SSE)
    void add_client(int client_sock);
    void remove_client(int client_sock);

    // Broadcasts real-time events to all connected browser tabs
    void broadcast_event(const std::string& event_type, const std::string& json_payload);

    size_t active_clients() const;

private:
    TelemetryStreamer() = default;

    mutable std::mutex mutex_;
    std::vector<int> client_sockets_;
};

} // namespace sentinel::nexus::api
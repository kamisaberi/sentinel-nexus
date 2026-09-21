#include "TelemetryStreamer.hpp"
#include "core/Logger.hpp"
#include <unistd.h>
#include <sstream>

namespace sentinel::nexus::api {

void TelemetryStreamer::add_client(int client_sock) {
    std::lock_guard<std::mutex> lock(mutex_);
    client_sockets_.push_back(client_sock);

    // Initial SSE handshake headers
    std::string sse_header = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/event-stream\r\n"
        "Cache-Control: no-cache\r\n"
        "Connection: keep-alive\r\n"
        "Access-Control-Allow-Origin: *\r\n\r\n"
        "data: {\"status\":\"connected\",\"stream\":\"sentinel-nexus-sse\"}\n\n";

    send(client_sock, sse_header.data(), sse_header.size(), MSG_NOSIGNAL);
    NEXUS_LOG_INFO("Browser attached to real-time telemetry stream. Total active: " + 
                   std::to_string(client_sockets_.size()));
}

void TelemetryStreamer::remove_client(int client_sock) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = client_sockets_.begin(); it != client_sockets_.end(); ++it) {
        if (*it == client_sock) {
            close(*it);
            client_sockets_.erase(it);
            break;
        }
    }
}

void TelemetryStreamer::broadcast_event(const std::string& event_type, const std::string& json_payload) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (client_sockets_.empty()) return;

    std::ostringstream msg;
    msg << "event: " << event_type << "\n"
        << "data: " << json_payload << "\n\n";

    std::string payload = msg.str();

    for (auto it = client_sockets_.begin(); it != client_sockets_.end();) {
        ssize_t bytes_sent = send(*it, payload.data(), payload.size(), MSG_NOSIGNAL);
        if (bytes_sent <= 0) {
            close(*it);
            it = client_sockets_.erase(it);
        } else {
            ++it;
        }
    }
}

size_t TelemetryStreamer::active_clients() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return client_sockets_.size();
}

} // namespace sentinel::nexus::api
#include <iostream>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

std::string http_request(const std::string& method, const std::string& path, const std::string& payload = "") {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return "Error: Failed to create socket.";

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9443);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        return "Error: Could not connect to Sentinel Nexus on localhost:9443";
    }

    std::ostringstream req;
    req << method << " " << path << " HTTP/1.1\r\n"
        << "Host: localhost:9443\r\n"
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << payload.size() << "\r\n"
        << "Connection: close\r\n\r\n"
        << payload;

    std::string req_str = req.str();
    send(sock, req_str.data(), req_str.size(), 0);

    char buffer[16384];
    std::string response;
    ssize_t bytes;
    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        response += buffer;
    }
    close(sock);

    size_t body_pos = response.find("\r\n\r\n");
    return (body_pos != std::string::npos) ? response.substr(body_pos + 4) : response;
}

void print_help() {
    std::cout << R"(
nexus-ctl - Sentinel Nexus Fleet Administration Tool

Usage:
  nexus-ctl <command> [arguments]

Commands:
  fleet list                List all connected appliances and metrics
  threat drop <IP>          Broadcast instant sub-50ms eBPF drop across fleet
  ota status                Check current ONNX model rollout stage
  ota stage                 Stage new candidate model into SHADOW_MODE
  ota advance               Advance model rollout stage (Shadow -> 5% -> Fleet)
  ota rollback              Execute emergency model rollback
  report cmmc               Print CMMC 2.0 / NIST SP 800-171 audit status
  report scada              Print IEC 62443 industrial audit status
)" << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_help();
        return 0;
    }

    std::string cmd = argv[1];

    if (cmd == "fleet" && argc >= 3 && std::string(argv[2]) == "list") {
        std::cout << http_request("GET", "/api/v1/fleet/nodes") << std::endl;
    } else if (cmd == "threat" && argc >= 4 && std::string(argv[2]) == "drop") {
        std::string ip = argv[3];
        std::string payload = "{\"ip\":\"" + ip + "\"}";
        std::cout << http_request("POST", "/api/v1/threats/broadcast", payload) << std::endl;
    } else if (cmd == "ota" && argc >= 3) {
        std::string sub = argv[2];
        if (sub == "status") std::cout << http_request("GET", "/api/v1/ota/status") << std::endl;
        else if (sub == "stage") std::cout << http_request("POST", "/api/v1/ota/stage", "{}") << std::endl;
        else if (sub == "advance") std::cout << http_request("POST", "/api/v1/ota/advance", "{}") << std::endl;
        else if (sub == "rollback") std::cout << http_request("POST", "/api/v1/ota/rollback", "{}") << std::endl;
    } else if (cmd == "report" && argc >= 3) {
        std::string sub = argv[2];
        if (sub == "cmmc") std::cout << http_request("GET", "/api/v1/reports/cmmc") << std::endl;
        else if (sub == "scada") std::cout << http_request("GET", "/api/v1/reports/scada") << std::endl;
    } else {
        print_help();
    }

    return 0;
}
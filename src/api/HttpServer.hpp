#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <unordered_map>

namespace sentinel::nexus::api
{

    class HttpServer
    {
    public:
        static HttpServer &instance()
        {
            static HttpServer inst;
            return inst;
        }

        /// Load dynamic configuration from INI/conf file
        bool load_config_file(const std::string &config_path = "configs/nexus_server.conf");

        bool start(const std::string &bind_ip = "0.0.0.0", uint16_t port = 9443, const std::string &web_root = "web");
        void stop();

        [[nodiscard]] uint16_t get_port() const { return port_; }
        [[nodiscard]] uint16_t get_sse_port() const { return sse_port_; }
        [[nodiscard]] uint16_t get_grpc_port() const { return grpc_port_; }

    private:
        HttpServer() = default;
        void server_loop();
        void handle_client(int client_sock);

        std::string route_request(const std::string &method,
                                  const std::string &path,
                                  const std::string &body,
                                  std::string &content_type,
                                  int &status_code);

        std::string handle_get_nodes();
        std::string handle_get_compliance();
        std::string handle_get_ota();
        std::string handle_post_broadcast(const std::string &body);
        std::string handle_post_ota_stage(const std::string &body);
        std::string handle_post_ota_advance();
        std::string handle_post_ota_rollback();

        // --- NEW EXTENSION & .SPKG HANDLERS ---
        std::string handle_get_extensions_status();
        std::string handle_post_extension_deploy(const std::string &body);

        std::string serve_static_file(const std::string &path, std::string &content_type, int &status_code);

        std::string bind_ip_{"0.0.0.0"};
        uint16_t port_{9443};
        uint16_t sse_port_{9444};
        uint16_t grpc_port_{50051};
        std::string web_root_{"web"};

        std::atomic<bool> running_{false};
        int server_fd_{-1};
        std::jthread worker_thread_;
    };

} // namespace sentinel::nexus::api
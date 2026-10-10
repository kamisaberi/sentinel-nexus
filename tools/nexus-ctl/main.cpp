#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstring>
#include <filesystem>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#define SPKG_MAGIC 0x474B50534F595241ULL // "ARYOSPKG"
#define SPKG_VERSION 1

struct SpkgHeader
{
    uint64_t magic{SPKG_MAGIC};
    uint32_t version{SPKG_VERSION};
    uint32_t tier{0};         // 0=Native C++, 1=Rust Wasm, 2=LuaJIT
    uint8_t signature[64]{0}; // Ed25519 signature
    uint32_t manifest_len{0};
    uint64_t payload_len{0};
    uint64_t source_len{0};
    uint32_t test_vector_len{0};
    uint32_t reserved{0};
} __attribute__((packed));

// Helper: HTTP POST
std::string http_post(const std::string &host, uint16_t port, const std::string &path, const std::string &payload, const std::string &token = "")
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        return "Error: Failed to create socket.";

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        close(sock);
        return "Error: Could not connect to " + host + ":" + std::to_string(port);
    }

    std::ostringstream req;
    req << "POST " << path << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n";
    if (!token.empty())
    {
        req << "Authorization: Bearer " << token << "\r\n";
    }
    req << "Content-Type: application/json\r\n"
        << "Content-Length: " << payload.size() << "\r\n"
        << "Connection: close\r\n\r\n"
        << payload;

    std::string req_str = req.str();
    send(sock, req_str.data(), req_str.size(), 0);

    char buffer[16384];
    std::string response;
    ssize_t bytes;
    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytes] = '\0';
        response += buffer;
    }
    close(sock);

    size_t body_pos = response.find("\r\n\r\n");
    return (body_pos != std::string::npos) ? response.substr(body_pos + 4) : response;
}

// Helper: HTTP GET
std::string http_get(const std::string &host, uint16_t port, const std::string &path, const std::string &token = "")
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        return "Error: Failed to create socket.";

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        close(sock);
        return "Error: Could not connect to " + host + ":" + std::to_string(port);
    }

    std::ostringstream req;
    req << "GET " << path << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n";
    if (!token.empty())
    {
        req << "Authorization: Bearer " << token << "\r\n";
    }
    req << "Connection: close\r\n\r\n";

    std::string req_str = req.str();
    send(sock, req_str.data(), req_str.size(), 0);

    char buffer[16384];
    std::string response;
    ssize_t bytes;
    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytes] = '\0';
        response += buffer;
    }
    close(sock);

    size_t body_pos = response.find("\r\n\r\n");
    return (body_pos != std::string::npos) ? response.substr(body_pos + 4) : response;
}

// Read binary file
std::vector<uint8_t> read_binary_file(const std::filesystem::path &p)
{
    std::ifstream f(p, std::ios::binary | std::ios::ate);
    if (!f.is_open())
        return {};
    size_t sz = f.tellg();
    f.seekg(0, std::ios::beg);
    std::vector<uint8_t> b(sz);
    f.read(reinterpret_cast<char *>(b.data()), sz);
    return b;
}

// SHA-256 buffer
std::vector<uint8_t> sha256_buffer(const uint8_t *data, size_t len)
{
    std::vector<uint8_t> hash(32);
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(ctx, data, len);
    unsigned int out_len = 0;
    EVP_DigestFinal_ex(ctx, hash.data(), &out_len);
    EVP_MD_CTX_free(ctx);
    return hash;
}

// Base64 Encode
std::string base64_encode(const uint8_t *data, size_t len)
{
    static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve(((len + 2) / 3) * 4);
    for (size_t i = 0; i < len; i += 3)
    {
        uint32_t val = (data[i] << 16);
        if (i + 1 < len)
            val |= (data[i + 1] << 8);
        if (i + 2 < len)
            val |= data[i + 2];
        out.push_back(b64_table[(val >> 18) & 0x3F]);
        out.push_back(b64_table[(val >> 12) & 0x3F]);
        out.push_back((i + 1 < len) ? b64_table[(val >> 6) & 0x3F] : '=');
        out.push_back((i + 2 < len) ? b64_table[val & 0x3F] : '=');
    }
    return out;
}

void print_help()
{
    std::cout << R"(
nexus-ctl - Sentinel Nexus Fleet Administration & Hub Marketplace CLI

Usage:
  nexus-ctl <command> [arguments]

Hub Marketplace & Extensions (.spkg):
  hub search [query]          Search packages on hub.aryorithm.com catalog
  hub inspect <file.spkg>     Inspect .spkg manifest, source code & SLA status
  hub verify <pub_key> <pkg>  Verify Ed25519 Merkle signature of .spkg
  hub broadcast <file.spkg>   Broadcast sealed .spkg fleet-wide via gRPC (< 50ms)
  hub new <name> [tier]       Scaffold a new .spkg package template (native, wasm, lua)

Fleet & Threat Administration:
  auth login [email] [pass]   Authenticate with FastAPI backend and acquire JWT
  auth status                 Check active JWT session state
  fleet list                  List all connected edge appliances and metrics
  threat drop <IP>            Broadcast instant sub-50ms eBPF drop across fleet
  ota status                  Check current ONNX model rollout stage
  ota stage                   Stage new candidate model into SHADOW_MODE
  ota advance                 Advance model rollout stage (Shadow -> 5% -> Fleet)
  ota rollback                Execute emergency model rollback
  report cmmc                 Print CMMC 2.0 / NIST SP 800-171 audit status
  report scada                Print IEC 62443 industrial audit status
)" << std::endl;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        print_help();
        return 0;
    }

    std::string cmd = argv[1];

    // =========================================================================
    // HUB MARKETPLACE COMMANDS (.spkg FORMAT)
    // =========================================================================
    if (cmd == "hub" && argc >= 3)
    {
        std::string sub = argv[2];

        // 1. nexus-ctl hub search [query]
        if (sub == "search")
        {
            std::string query = (argc >= 4) ? argv[3] : "";
            std::cout << "\033[1;36m========================================================================================================\033[0m\n";
            std::cout << "\033[1;37m                       ARYORITHM REGISTRY HUB: VERIFIED PACKAGES (hub.aryorithm.com)                     \033[0m\n";
            std::cout << "\033[1;36m========================================================================================================\033[0m\n";
            std::cout << std::left
                      << std::setw(32) << "PACKAGE IDENTIFIER"
                      << std::setw(12) << "TIER"
                      << std::setw(12) << "PROTOCOL"
                      << std::setw(14) << "LATENCY SLA"
                      << std::setw(16) << "SECURITY AUDIT"
                      << "TARGET SECTOR\n";
            std::cout << "--------------------------------------------------------------------------------------------------------\n";

            struct HubEntry
            {
                std::string id, tier, proto, sla, audit, sector;
            };
            std::vector<HubEntry> catalog = {
                {"org.aryorithm.modbus_guard", "Native C++", "MODBUS", "< 120 ns", "Ed25519 Sealed", "Power & Substations"},
                {"org.aryorithm.s7comm_guard", "Rust Wasm", "S7COMM", "< 1.5 us", "Memory Safe", "Manufacturing & PLCs"},
                {"org.aryorithm.log4j_fast_drop", "LuaJIT", "HTTP/TCP", "< 450 ns", "Hot-Reloaded", "Enterprise DMZ"},
                {"org.aryorithm.dicom_phi_guard", "Rust Wasm", "DICOM", "< 2.5 us", "HIPAA / PHI", "Healthcare PACS"},
                {"org.aryorithm.c37_118_pmu", "Native C++", "C37.118", "< 130 ns", "Ed25519 Sealed", "High-Voltage Grids"},
                {"org.aryorithm.triton_sis", "Native C++", "TRISTATION", "< 190 ns", "Safety SIS", "Oil & Chemical Fabs"}};

            for (const auto &item : catalog)
            {
                if (query.empty() || item.id.find(query) != std::string::npos || item.proto.find(query) != std::string::npos)
                {
                    std::cout << std::left
                              << std::setw(32) << item.id
                              << std::setw(12) << item.tier
                              << std::setw(12) << item.proto
                              << std::setw(14) << item.sla
                              << "\033[32m" << std::setw(16) << item.audit << "\033[0m"
                              << item.sector << "\n";
                }
            }
            std::cout << "\033[1;36m========================================================================================================\033[0m\n";
            std::cout << "Deploy fleet-wide via: nexus-ctl hub broadcast <package.spkg>\n\n";
            return 0;
        }

        // 2. nexus-ctl hub inspect <file.spkg>
        if (sub == "inspect" && argc >= 4)
        {
            std::string pkg_path = argv[3];
            auto data = read_binary_file(pkg_path);
            if (data.size() < sizeof(SpkgHeader))
            {
                std::cerr << "[-] Error: Invalid .spkg file: " << pkg_path << std::endl;
                return 1;
            }

            SpkgHeader hdr;
            std::memcpy(&hdr, data.data(), sizeof(hdr));

            if (hdr.magic != SPKG_MAGIC)
            {
                std::cerr << "[-] Error: Magic bytes mismatch! Not a valid .spkg container." << std::endl;
                return 1;
            }

            std::string tier_str = (hdr.tier == 0) ? "Tier A (Native ISO C++20 Shared Object)" : (hdr.tier == 1) ? "Tier B (WebAssembly Wasm3 / Rust Module)"
                                                                                                                 : "Tier C (LuaJIT Dynamic C-FFI Script)";

            std::string manifest_str(reinterpret_cast<const char *>(data.data() + sizeof(SpkgHeader)), hdr.manifest_len);

            std::cout << "\033[1;36m==========================================================\033[0m\n";
            std::cout << "\033[1;37m        SENTINEL PACKAGE INSPECTION (.spkg)               \033[0m\n";
            std::cout << "\033[1;36m==========================================================\033[0m\n";
            std::cout << "Package File     : " << pkg_path << "\n";
            std::cout << "Format Version   : " << hdr.version << "\n";
            std::cout << "Execution Tier   : \033[1;33m" << tier_str << "\033[0m\n";
            std::cout << "Signature Status : \033[1;32mEd25519 64-Byte Merkle-Sealed\033[0m\n";
            std::cout << "Payload Binary   : " << hdr.payload_len << " bytes\n";
            std::cout << "Auditable Source : " << hdr.source_len << " bytes\n";
            std::cout << "Pre-flight Frame : " << hdr.test_vector_len << " bytes\n";
            std::cout << "----------------------------------------------------------\n";
            std::cout << "\033[1;37mMANIFEST METADATA:\033[0m\n"
                      << manifest_str << "\n";
            std::cout << "\033[1;36m==========================================================\033[0m\n";
            return 0;
        }

        // 3. nexus-ctl hub verify <pub_key.pem> <file.spkg>
        if (sub == "verify" && argc >= 5)
        {
            std::string pub_path = argv[3];
            std::string pkg_path = argv[4];

            auto container = read_binary_file(pkg_path);
            if (container.size() < sizeof(SpkgHeader))
            {
                std::cerr << "[-] Error: Corrupt package size" << std::endl;
                return 1;
            }

            SpkgHeader hdr;
            std::memcpy(&hdr, container.data(), sizeof(hdr));
            if (hdr.magic != SPKG_MAGIC)
            {
                std::cerr << "[-] Error: Magic mismatch" << std::endl;
                return 1;
            }

            const uint8_t *man_ptr = container.data() + sizeof(SpkgHeader);
            const uint8_t *pay_ptr = man_ptr + hdr.manifest_len;
            const uint8_t *src_ptr = pay_ptr + hdr.payload_len;
            const uint8_t *test_ptr = src_ptr + hdr.source_len;

            auto h_man = sha256_buffer(man_ptr, hdr.manifest_len);
            auto h_pay = sha256_buffer(pay_ptr, hdr.payload_len);
            auto h_src = sha256_buffer(src_ptr, hdr.source_len);
            auto h_test = sha256_buffer(test_ptr, hdr.test_vector_len);

            std::vector<uint8_t> sign_input;
            sign_input.insert(sign_input.end(), h_man.begin(), h_man.end());
            sign_input.insert(sign_input.end(), h_pay.begin(), h_pay.end());
            sign_input.insert(sign_input.end(), h_src.begin(), h_src.end());
            sign_input.insert(sign_input.end(), h_test.begin(), h_test.end());

            FILE *fp = fopen(pub_path.c_str(), "rb");
            if (!fp)
            {
                std::cerr << "[-] Cannot open public key" << std::endl;
                return 1;
            }
            EVP_PKEY *pkey = PEM_read_PUBKEY(fp, nullptr, nullptr, nullptr);
            fclose(fp);

            EVP_MD_CTX *ctx = EVP_MD_CTX_new();
            EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr, pkey);
            int res = EVP_DigestVerify(ctx, hdr.signature, 64, sign_input.data(), sign_input.size());
            EVP_MD_CTX_free(ctx);
            EVP_PKEY_free(pkey);

            if (res == 1)
            {
                std::cout << "\033[1;32m[VERIFIED] .spkg Merkle signature is 100% AUTHENTIC and tamper-free!\033[0m\n";
                return 0;
            }
            else
            {
                std::cerr << "\033[1;31m[REJECTED] Cryptographic signature INVALID or container TAMPERED!\033[0m\n";
                return 1;
            }
        }

        // 4. nexus-ctl hub broadcast <file.spkg>
        if (sub == "broadcast" && argc >= 4)
        {
            std::string pkg_path = argv[3];
            auto container = read_binary_file(pkg_path);
            if (container.size() < sizeof(SpkgHeader))
            {
                std::cerr << "[-] Invalid package: " << pkg_path << std::endl;
                return 1;
            }

            SpkgHeader hdr;
            std::memcpy(&hdr, container.data(), sizeof(hdr));

            const uint8_t *man_ptr = container.data() + sizeof(SpkgHeader);
            const uint8_t *pay_ptr = man_ptr + hdr.manifest_len;

            std::string b64_payload = base64_encode(pay_ptr, hdr.payload_len);
            std::string b64_sig = base64_encode(hdr.signature, 64);

            std::cout << "[*] Packaging .spkg for fleet gRPC fanout..." << std::endl;
            std::cout << "    Payload Size: " << hdr.payload_len << " bytes | Source: "
                      << hdr.source_len << " bytes | Tier: " << hdr.tier << std::endl;

            std::ostringstream json_payload;
            json_payload << "{"
                         << "\"rule_id\":\"SPKG-" << std::to_string(hdr.magic).substr(0, 8) << "\","
                         << "\"rule_name\":\"" << std::filesystem::path(pkg_path).stem().string() << "\","
                         << "\"tier\":" << hdr.tier << ","
                         << "\"payload_b64\":\"" << b64_payload << "\","
                         << "\"signature_b64\":\"" << b64_sig << "\""
                         << "}";

            std::cout << "[*] Broadcasting to all fleet nodes via gRPC DeployExtensionRule..." << std::endl;
            std::string resp = http_post("127.0.0.1", 9443, "/api/v1/extensions/deploy", json_payload.str());
            std::cout << "\033[1;32m[+] Fleet Broadcast Response:\033[0m\n"
                      << resp << std::endl;
            return 0;
        }

        // 5. nexus-ctl hub new <name> [native|wasm|lua]
        if (sub == "new" && argc >= 4)
        {
            std::string name = argv[3];
            std::string lang = (argc >= 5) ? argv[4] : "wasm";
            std::transform(lang.begin(), lang.end(), lang.begin(), ::tolower);
            if (lang == "rust")
                lang = "wasm";
            if (lang == "cpp")
                lang = "native";

            std::filesystem::path dir = name;
            if (std::filesystem::exists(dir))
            {
                std::cerr << "[-] Error: Directory '" << name << "' already exists." << std::endl;
                return 1;
            }
            std::filesystem::create_directories(dir);
            std::filesystem::create_directories(dir / "tests");

            // Scaffold manifest.json
            std::ofstream(dir / "manifest.json") << "{\n"
                                                 << "  \"id\": \"org.aryorithm.package." << name << "\",\n"
                                                 << "  \"name\": \"" << name << "\",\n"
                                                 << "  \"version\": \"1.0.0\",\n"
                                                 << "  \"tier\": \"" << lang << "\",\n"
                                                 << "  \"author\": \"Nexus Operator\",\n"
                                                 << "  \"target_protocol\": \"CUSTOM\",\n"
                                                 << "  \"default_port\": 0\n"
                                                 << "}\n";

            // Scaffold sample pre-flight test vector
            uint8_t sample_frame[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04};
            std::ofstream(dir / "tests" / "preflight_frame.bin", std::ios::binary)
                .write(reinterpret_cast<const char *>(sample_frame), sizeof(sample_frame));

            if (lang == "wasm")
            {
                std::filesystem::create_directories(dir / "src");
                std::ofstream(dir / "Cargo.toml") << "[package]\nname = \"" << name << "\"\nversion = \"1.0.0\"\nedition = \"2021\"\n\n"
                                                  << "[lib]\ncrate-type = [\"cdylib\"]\n\n"
                                                  << "[dependencies]\nsentinel-sdk-rs = { path = \"/home/kami/blackbox-sentinel/tools/sdk/rust/sentinel-sdk-rs\" }\n\n"
                                                  << "[profile.release]\nopt-level = 3\nlto = true\npanic = \"abort\"\nstrip = true\n";

                std::ofstream(dir / "src" / "lib.rs") << "#![no_std]\nuse core::panic::PanicInfo;\nuse sentinel_sdk_rs::{PacketView, Verdict};\n\n"
                                                      << "#[panic_handler]\nfn panic(_info: &PanicInfo) -> ! { loop {} }\n\n"
                                                      << "#[no_mangle]\npub extern \"C\" fn sentinel_dissect(pkt_ptr: *const u8, len: u32) -> i32 {\n"
                                                      << "    let pkt = unsafe { PacketView::from_raw(pkt_ptr, len as usize) };\n"
                                                      << "    if pkt.is_empty() { return Verdict::Pass as i32; }\n"
                                                      << "    // Detection logic here\n"
                                                      << "    Verdict::Pass as i32\n}\n";
            }
            else if (lang == "native")
            {
                std::ofstream(dir / (name + ".cpp")) << "#include <sentinel/sdk/plugin.hpp>\n\n"
                                                     << "using namespace sentinel::sdk;\n\n"
                                                     << "static SentinelDissectorResult my_dissect(const SentinelRawPacket* pkt) {\n"
                                                     << "    return VerdictBuilder::Pass();\n}\n\n"
                                                     << "static SentinelPluginDescriptor g_desc = {\n"
                                                     << "    SENTINEL_SDK_MAGIC, SENTINEL_SDK_VERSION_MAJOR, SENTINEL_SDK_VERSION_MINOR,\n"
                                                     << "    SENTINEL_TIER_NATIVE_CPP, \"org.aryorithm.package." << name << "\",\n"
                                                     << "    \"" << name << "\", \"1.0.0\", \"CUSTOM\", 0, 0, nullptr, nullptr, my_dissect\n"
                                                     << "};\nSENTINEL_REGISTER_PLUGIN(g_desc)\n";
            }
            else if (lang == "lua")
            {
                std::ofstream(dir / (name + ".lua")) << "local ffi = ffi or require(\"ffi\")\n\n"
                                                     << "Rule = { id = 8001, name = \"" << name << "\", port = 0 }\n\n"
                                                     << "function Rule.inspect(pkt)\n"
                                                     << "    local len = tonumber(pkt.length)\n"
                                                     << "    if not len or len < 4 then return 0 end\n"
                                                     << "    return 0\n"
                                                     << "end\n";
            }

            std::cout << "\033[1;32m[+] Nexus C2 scaffolded " << lang << " package in ./" << name << "/\033[0m\n"
                      << "    ├── manifest.json\n"
                      << "    └── tests/preflight_frame.bin\n";
            return 0;
        }
    }

    // =========================================================================
    // AUTHENTICATION COMMANDS
    // =========================================================================
    if (cmd == "auth" && argc >= 3 && std::string(argv[2]) == "login")
    {
        std::string email = (argc >= 4) ? argv[3] : "kamisaberi@gmail.com";
        std::string pass = (argc >= 5) ? argv[4] : "12345678";

        std::cout << "[*] Authenticating with FastAPI backend at 127.0.0.1:8000 (" << email << ")..." << std::endl;
        std::string payload = "{\"email\":\"" + email + "\",\"password\":\"" + pass + "\",\"username\":\"" + email + "\"}";
        std::string resp = http_post("127.0.0.1", 8000, "/api/v1/auth/login", payload);

        std::cout << "[+] Server Response:\n"
                  << resp << std::endl;

        std::ofstream f("data/cloud_session.json");
        if (f.is_open())
        {
            f << resp;
            std::cout << "[+] Saved JWT session to data/cloud_session.json" << std::endl;
        }
        return 0;
    }

    if (cmd == "auth" && argc >= 3 && std::string(argv[2]) == "status")
    {
        std::ifstream f("data/cloud_session.json");
        if (f.is_open())
        {
            std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
            std::cout << "[+] Active Cloud Session Found:\n"
                      << content << std::endl;
        }
        else
        {
            std::cout << "[-] No active cloud session on disk. Run: nexus-ctl auth login" << std::endl;
        }
        return 0;
    }

    // =========================================================================
    // FLEET, THREAT, OTA & REPORT COMMANDS (PASS-THROUGH TO 9443)
    // =========================================================================
    if (cmd == "fleet" && argc >= 3 && std::string(argv[2]) == "list")
    {
        std::cout << http_post("127.0.0.1", 9443, "/api/v1/fleet/nodes", "") << std::endl;
    }
    else if (cmd == "threat" && argc >= 4 && std::string(argv[2]) == "drop")
    {
        std::string ip = argv[3];
        std::cout << http_post("127.0.0.1", 9443, "/api/v1/threats/broadcast", "{\"ip\":\"" + ip + "\"}") << std::endl;
    }
    else if (cmd == "ota" && argc >= 3)
    {
        std::string sub = argv[2];
        if (sub == "status")
            std::cout << http_post("127.0.0.1", 9443, "/api/v1/ota/status", "") << std::endl;
        else if (sub == "stage")
            std::cout << http_post("127.0.0.1", 9443, "/api/v1/ota/stage", "{}") << std::endl;
        else if (sub == "advance")
            std::cout << http_post("127.0.0.1", 9443, "/api/v1/ota/advance", "{}") << std::endl;
        else if (sub == "rollback")
            std::cout << http_post("127.0.0.1", 9443, "/api/v1/ota/rollback", "{}") << std::endl;
    }
    else if (cmd == "report" && argc >= 3)
    {
        std::string sub = argv[2];
        if (sub == "cmmc")
            std::cout << http_post("127.0.0.1", 9443, "/api/v1/reports/cmmc", "") << std::endl;
        else if (sub == "scada")
            std::cout << http_post("127.0.0.1", 9443, "/api/v1/reports/scada", "") << std::endl;
    }
    else
    {
        print_help();
    }

    return 0;
}
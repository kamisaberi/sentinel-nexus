#include "NodeRegistry.hpp"
#include "core/Logger.hpp"
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

namespace sentinel::nexus::fleet
{

    static std::string generate_node_hash(const HardwareIdentity &id)
    {
        std::string seed = id.machine_uuid() + id.hostname() + id.kernel_version();
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char *>(seed.data()), seed.size(), hash);

        std::ostringstream ss;
        for (int i = 0; i < 8; ++i)
        { // 16-char hex prefix
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return "NODE-" + ss.str();
    }

    std::string NodeRegistry::register_node(const RegistrationRequest &req)
    {
        std::unique_lock lock(mutex_);

        std::string node_id = generate_node_hash(req.identity());

        RegisteredNode node{
            .node_id = node_id,
            .site_identifier = req.site_identifier(),
            .software_version = req.software_version(),
            .identity = req.identity(),
            .latest_metrics = {},
            .last_heartbeat = std::chrono::system_clock::now(),
            .status = NodeHealthStatus::ONLINE};

        nodes_[node_id] = std::move(node);

        NEXUS_LOG_INFO("Registered appliance: " + node_id + " [Site: " + req.site_identifier() +
                       ", Host: " + req.identity().hostname() + "]");
        return node_id;
    }

    bool NodeRegistry::update_heartbeat(const std::string &node_id, const DeviceMetrics &metrics)
    {
        std::unique_lock lock(mutex_);
        auto it = nodes_.find(node_id);
        if (it == nodes_.end())
        {
            return false;
        }

        it->second.latest_metrics = metrics;
        it->second.last_heartbeat = std::chrono::system_clock::now();
        it->second.status = NodeHealthStatus::ONLINE;
        return true;
    }

    bool NodeRegistry::mark_node_offline(const std::string &node_id, const std::string &reason)
    {
        std::unique_lock lock(mutex_);
        auto it = nodes_.find(node_id);
        if (it == nodes_.end())
        {
            return false;
        }

        it->second.status = NodeHealthStatus::OFFLINE;
        NEXUS_LOG_WARN("Appliance disconnected (" + reason + "): " + node_id +
                       " (" + it->second.site_identifier + ")");
        return true;
    }

    void NodeRegistry::evaluate_node_health(uint32_t timeout_seconds)
    {
        std::unique_lock lock(mutex_);
        auto now = std::chrono::system_clock::now();

        for (auto &[id, node] : nodes_)
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - node.last_heartbeat).count();
            if (elapsed > timeout_seconds)
            {
                if (node.status != NodeHealthStatus::OFFLINE)
                {
                    node.status = NodeHealthStatus::OFFLINE;
                    NEXUS_LOG_WARN("Appliance went OFFLINE: " + id + " (" + node.site_identifier + ")");
                }
            }
        }
    }

    size_t NodeRegistry::active_node_count() const
    {
        std::shared_lock lock(mutex_);
        size_t count = 0;
        for (const auto &[_, node] : nodes_)
        {
            if (node.status == NodeHealthStatus::ONLINE)
                count++;
        }
        return count;
    }

    std::vector<RegisteredNode> NodeRegistry::get_all_nodes() const
    {
        std::shared_lock lock(mutex_);
        std::vector<RegisteredNode> result;
        result.reserve(nodes_.size());
        for (const auto &[_, node] : nodes_)
        {
            result.push_back(node);
        }
        return result;
    }

} // namespace sentinel::nexus::fleet
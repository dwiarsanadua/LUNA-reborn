#include "DistributeServer.h"
#include "engine/network/NetworkLayer.h"
#include <spdlog/spdlog.h>
#include <sstream>

DistributeServer::DistributeServer()
    : network_(std::make_unique<NetworkLayer>())
    , last_heartbeat_check_(std::chrono::steady_clock::now())
{}

DistributeServer::~DistributeServer() { Shutdown(); }

bool DistributeServer::Initialize(uint16_t port) {
    if (!network_->Initialize(port)) {
        spdlog::error("DistributeServer: network init failed on port {}", port);
        return false;
    }

    network_->SetReceiveCallback([](uint16_t type, const uint8_t*, size_t) {
        spdlog::debug("DistributeServer: packet type=0x{:04X}", type);
    });

    spdlog::info("DistributeServer: initialized on port {}", port);
    return true;
}

void DistributeServer::Shutdown() {
    network_->Shutdown();
    servers_.clear();
    player_server_map_.clear();
}

void DistributeServer::Update() {
    network_->Update();

    // Heartbeat check every 30 seconds
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_heartbeat_check_);
    if (elapsed.count() >= 30) {
        last_heartbeat_check_ = now;
        for (auto it = servers_.begin(); it != servers_.end(); ) {
            auto age = std::chrono::duration_cast<std::chrono::seconds>(
                now - it->second.last_heartbeat);
            if (age.count() > 60) {
                spdlog::warn("DistributeServer: server {} ({} heartbeat timeout",
                             it->second.server_id, it->second.name);
                it->second.online = false;
                if (age.count() > 120) {
                    it = servers_.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }
}

void DistributeServer::HandleChat(const std::string& sender, int channel, const std::string& message) {
    spdlog::info("Chat [ch{}] {}: {}", channel, sender, message);

    // Route to all map servers for broadcast to players
    for (auto& [id, sv] : servers_) {
        if (sv.online) {
            // Would send chat packet via network layer
        }
    }
}

void DistributeServer::HandleWhisper(const std::string& sender, const std::string& target, const std::string& message) {
    auto it = player_server_map_.find(target);
    if (it == player_server_map_.end()) {
        spdlog::warn("Whisper: target '{}' not found", target);
        return;
    }

    int server_id = it->second;
    auto sv_it = servers_.find(server_id);
    if (sv_it != servers_.end() && sv_it->second.online) {
        // Route whisper packet to correct server
        spdlog::info("Whisper {} -> {} via server {}", sender, target, server_id);
    }
}

void DistributeServer::BroadcastServerList(const std::vector<ServerInfo>& servers) {
    spdlog::info("DistributeServer: broadcasting {} server entries", servers.size());
}

void DistributeServer::RoutePacket(int target_server_id, const uint8_t* data, size_t size) {
    auto it = servers_.find(target_server_id);
    if (it == servers_.end() || !it->second.online) {
        spdlog::warn("RoutePacket: server {} not found or offline", target_server_id);
        return;
    }
    // Would send data via network layer connection to the target server
}

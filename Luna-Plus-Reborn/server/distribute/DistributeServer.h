// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>

class NetworkLayer;

struct ServerInfo {
    int server_id;
    uint16_t map_id;
    std::string name;
    std::string host;
    uint16_t port;
    int player_count = 0;
    int max_players = 200;
    bool online = false;
    std::chrono::steady_clock::time_point last_heartbeat;
};

class DistributeServer {
public:
    DistributeServer();
    ~DistributeServer();

    bool Initialize(uint16_t port);
    void Shutdown();
    void Update();

    void HandleChat(const std::string& sender, int channel, const std::string& message);
    void HandleWhisper(const std::string& sender, const std::string& target, const std::string& message);
    void BroadcastServerList(const std::vector<ServerInfo>& servers);
    void RoutePacket(int target_server_id, const uint8_t* data, size_t size);
    void RegisterMap(uint16_t map_id, uint16_t port, const std::string& name);
    const ServerInfo* FindMapServer(uint16_t map_id) const;

private:
    void HandlePacket(uint16_t type, const uint8_t* payload, size_t len);
    void RegisterDefaultMaps();
    std::unique_ptr<NetworkLayer> network_;
    std::unordered_map<int, ServerInfo> servers_;
    std::unordered_map<std::string, int> player_server_map_; // player_name -> server_id
    int next_server_id_ = 1;
    std::chrono::steady_clock::time_point last_heartbeat_check_;
};

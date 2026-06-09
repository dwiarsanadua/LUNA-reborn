#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>

const int MAX_CHANNELS_PER_MAP = 10;
const int MAX_PLAYERS_PER_CHANNEL = 200;

enum class ChannelStatus : uint8_t {
    Online = 0,
    Busy = 1,
    Maintenance = 2,
    Offline = 3,
};

struct ChannelInfo {
    uint16_t map_id;
    uint8_t channel_id;
    std::string name;
    int player_count = 0;
    int max_players = MAX_PLAYERS_PER_CHANNEL;
    ChannelStatus status = ChannelStatus::Online;
    std::string host;
    uint16_t port;
    std::chrono::steady_clock::time_point last_heartbeat;
};

struct ChannelStats {
    int total_channels = 0;
    int total_players = 0;
    int max_capacity = 0;
    float load_percent = 0.0f;
    int maps_active = 0;
};

class ChannelManager {
public:
    ChannelManager();

    void Update();

    // Channel lifecycle
    bool CreateChannel(uint16_t map_id, uint8_t channel_id,
                       const std::string& name, const std::string& host, uint16_t port);
    bool RemoveChannel(uint16_t map_id, uint8_t channel_id);
    void RemoveAllChannelsForMap(uint16_t map_id);

    // Player management
    bool JoinChannel(uint16_t map_id, uint8_t channel_id, const std::string& player_name);
    bool LeaveChannel(uint16_t map_id, uint8_t channel_id, const std::string& player_name);
    bool LeaveAllChannels(const std::string& player_name);

    // Query
    const ChannelInfo* GetChannel(uint16_t map_id, uint8_t channel_id) const;
    std::vector<const ChannelInfo*> GetChannelsForMap(uint16_t map_id) const;
    std::vector<const ChannelInfo*> GetAllChannels() const;
    ChannelInfo* FindLeastLoadedChannel(uint16_t map_id);
    int GetChannelCount(uint16_t map_id) const;
    ChannelStats GetStats() const;

    // Heartbeat
    bool Heartbeat(uint16_t map_id, uint8_t channel_id);
    void PruneDeadChannels(int timeout_seconds = 120);

    // Callbacks
    using ChannelUpdateCallback = std::function<void(uint16_t map_id, uint8_t channel_id, int player_count)>;
    void SetChannelUpdateCallback(ChannelUpdateCallback cb) { update_cb_ = cb; }

private:
    struct ChannelKey {
        uint16_t map_id;
        uint8_t channel_id;
        bool operator==(const ChannelKey& o) const {
            return map_id == o.map_id && channel_id == o.channel_id;
        }
    };

    struct ChannelKeyHash {
        size_t operator()(const ChannelKey& k) const {
            return ((size_t)k.map_id << 8) | k.channel_id;
        }
    };

    std::unordered_map<ChannelKey, ChannelInfo, ChannelKeyHash> channels_;
    std::unordered_map<std::string, ChannelKey> player_map_; // player_name -> channel
    ChannelUpdateCallback update_cb_;
    int next_channel_id_ = 1;
};

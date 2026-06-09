#include "ChannelManager.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

ChannelManager::ChannelManager() {
    spdlog::info("ChannelManager: initialized");
}

void ChannelManager::Update() {
    PruneDeadChannels(120);
    if (update_cb_) {
        for (const auto& [key, ch] : channels_) {
            update_cb_(key.map_id, key.channel_id, ch.player_count);
        }
    }
}

bool ChannelManager::CreateChannel(uint16_t map_id, uint8_t channel_id,
                                    const std::string& name,
                                    const std::string& host, uint16_t port) {
    ChannelKey key{map_id, channel_id};
    if (channels_.find(key) != channels_.end()) {
        spdlog::warn("ChannelManager: channel already exists for map {} channel {}",
                     map_id, channel_id);
        return false;
    }

    ChannelInfo info;
    info.map_id = map_id;
    info.channel_id = channel_id;
    info.name = name.empty() ? "Channel " + std::to_string(channel_id) : name;
    info.player_count = 0;
    info.max_players = MAX_PLAYERS_PER_CHANNEL;
    info.status = ChannelStatus::Online;
    info.host = host;
    info.port = port;
    info.last_heartbeat = std::chrono::steady_clock::now();

    channels_[key] = info;
    spdlog::info("ChannelManager: created channel map={} ch={} on {}:{}",
                 map_id, channel_id, host, port);
    return true;
}

bool ChannelManager::RemoveChannel(uint16_t map_id, uint8_t channel_id) {
    ChannelKey key{map_id, channel_id};
    auto it = channels_.find(key);
    if (it == channels_.end()) return false;

    // Remove all players from this channel
    for (auto pit = player_map_.begin(); pit != player_map_.end(); ) {
        if (pit->second.map_id == map_id && pit->second.channel_id == channel_id)
            pit = player_map_.erase(pit);
        else
            ++pit;
    }

    channels_.erase(it);
    spdlog::info("ChannelManager: removed channel map={} ch={}", map_id, channel_id);
    return true;
}

void ChannelManager::RemoveAllChannelsForMap(uint16_t map_id) {
    std::vector<uint8_t> to_remove;
    for (const auto& [key, ch] : channels_) {
        if (key.map_id == map_id) to_remove.push_back(key.channel_id);
    }
    for (uint8_t ch_id : to_remove) RemoveChannel(map_id, ch_id);
}

bool ChannelManager::JoinChannel(uint16_t map_id, uint8_t channel_id,
                                  const std::string& player_name) {
    // Leave current channel if any
    LeaveAllChannels(player_name);

    ChannelKey key{map_id, channel_id};
    auto it = channels_.find(key);
    if (it == channels_.end()) {
        spdlog::warn("ChannelManager: channel not found for join map={} ch={}",
                     map_id, channel_id);
        return false;
    }

    if (it->second.player_count >= it->second.max_players) {
        spdlog::warn("ChannelManager: channel full map={} ch={} ({} players)",
                     map_id, channel_id, it->second.player_count);
        return false;
    }

    it->second.player_count++;
    player_map_[player_name] = key;
    return true;
}

bool ChannelManager::LeaveChannel(uint16_t map_id, uint8_t channel_id,
                                   const std::string& player_name) {
    // Verify player is actually on this channel before decrementing
    auto pit = player_map_.find(player_name);
    if (pit == player_map_.end() || pit->second.map_id != map_id
                                  || pit->second.channel_id != channel_id) {
        return false;
    }

    ChannelKey key{map_id, channel_id};
    auto it = channels_.find(key);
    if (it == channels_.end()) {
        // Channel gone, just remove player mapping
        player_map_.erase(pit);
        return false;
    }

    it->second.player_count = std::max(0, it->second.player_count - 1);
    player_map_.erase(pit);
    return true;
}

bool ChannelManager::LeaveAllChannels(const std::string& player_name) {
    auto pit = player_map_.find(player_name);
    if (pit == player_map_.end()) return false;

    LeaveChannel(pit->second.map_id, pit->second.channel_id, player_name);
    return true;
}

const ChannelInfo* ChannelManager::GetChannel(uint16_t map_id, uint8_t channel_id) const {
    ChannelKey key{map_id, channel_id};
    auto it = channels_.find(key);
    return it != channels_.end() ? &it->second : nullptr;
}

std::vector<const ChannelInfo*> ChannelManager::GetChannelsForMap(uint16_t map_id) const {
    std::vector<const ChannelInfo*> result;
    for (const auto& [key, ch] : channels_) {
        if (key.map_id == map_id) result.push_back(&ch);
    }
    return result;
}

std::vector<const ChannelInfo*> ChannelManager::GetAllChannels() const {
    std::vector<const ChannelInfo*> result;
    for (const auto& [key, ch] : channels_) result.push_back(&ch);
    return result;
}

ChannelInfo* ChannelManager::FindLeastLoadedChannel(uint16_t map_id) {
    ChannelInfo* best = nullptr;
    float best_load = 1.0f;

    for (auto& [key, ch] : channels_) {
        if (key.map_id != map_id) continue;
        if (ch.status != ChannelStatus::Online) continue;

        float load = (float)ch.player_count / (float)ch.max_players;
        if (load < best_load) {
            best_load = load;
            best = &ch;
        }
    }

    return best;
}

int ChannelManager::GetChannelCount(uint16_t map_id) const {
    int count = 0;
    for (const auto& [key, ch] : channels_) {
        if (key.map_id == map_id) count++;
    }
    return count;
}

ChannelStats ChannelManager::GetStats() const {
    ChannelStats stats;
    std::unordered_map<uint16_t, bool> active_maps;

    stats.total_channels = static_cast<int>(channels_.size());
    stats.max_capacity = stats.total_channels * MAX_PLAYERS_PER_CHANNEL;

    for (const auto& [key, ch] : channels_) {
        stats.total_players += ch.player_count;
        active_maps[key.map_id] = true;
    }

    stats.maps_active = static_cast<int>(active_maps.size());
    stats.load_percent = stats.max_capacity > 0
        ? (float)stats.total_players / (float)stats.max_capacity * 100.0f
        : 0.0f;

    return stats;
}

bool ChannelManager::Heartbeat(uint16_t map_id, uint8_t channel_id) {
    ChannelKey key{map_id, channel_id};
    auto it = channels_.find(key);
    if (it == channels_.end()) return false;

    it->second.last_heartbeat = std::chrono::steady_clock::now();
    it->second.status = ChannelStatus::Online;
    return true;
}

void ChannelManager::PruneDeadChannels(int timeout_seconds) {
    auto now = std::chrono::steady_clock::now();
    std::vector<std::pair<uint16_t, uint8_t>> dead;

    for (const auto& [key, ch] : channels_) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - ch.last_heartbeat).count();
        if (elapsed > timeout_seconds) {
            dead.push_back({key.map_id, key.channel_id});
        }
    }

    for (const auto& [map_id, ch_id] : dead) {
        spdlog::warn("ChannelManager: pruning dead channel map={} ch={}",
                     map_id, ch_id);
        RemoveChannel(map_id, ch_id);
    }

    if (!dead.empty()) {
        spdlog::info("ChannelManager: pruned {} dead channels", dead.size());
    }
}

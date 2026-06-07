#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct GuildMember {
    uint32_t character_id;
    std::string name;
    uint8_t rank;
    uint32_t joined_at;
    uint32_t contribution;
    bool is_online;
};

struct Guild {
    uint32_t guild_id;
    std::string name;
    std::string emblem;
    std::string notice;
    uint32_t master_id;
    std::vector<GuildMember> members;
    uint32_t gold = 0;
    uint32_t level = 1;
    uint32_t exp = 0;
    uint32_t war_wins = 0;
    uint32_t war_losses = 0;
};

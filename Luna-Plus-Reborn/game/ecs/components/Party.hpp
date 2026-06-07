#pragma once
#include <cstdint>
#include <vector>
#include <string>

struct PartyMember {
    uint32_t entity_id;
    uint32_t character_id;
    std::string name;
    uint16_t level;
    int32_t hp, max_hp, mp, max_mp;
    uint16_t map_id;
    bool is_online;
};

struct Party {
    uint32_t party_id;
    uint32_t leader_id;
    std::vector<PartyMember> members;
    bool is_distribution_free;
    float total_xp_earned = 0.0f;
    uint16_t member_count() const { return static_cast<uint16_t>(members.size()); }
    bool IsLeader(uint32_t entity_id) const { return leader_id == entity_id; }
    bool IsMember(uint32_t entity_id) const;
};

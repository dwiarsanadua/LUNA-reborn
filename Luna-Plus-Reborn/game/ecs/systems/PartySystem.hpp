#pragma once
#include <entt/entt.hpp>
#include <unordered_map>
#include <random>
#include <chrono>

enum class LootMode : uint8_t {
    FREE_FOR_ALL = 0,
    ROUND_ROBIN = 1,
    MASTER_LOOT = 2,
    NEED_GREED = 3
};

struct InviteEntry {
    entt::entity inviter;
    entt::entity invitee;
    float timer = 0.0f;
    float timeout = 30.0f;
    bool expired = false;
};

class PartySystem {
public:
    PartySystem();
    bool CreateParty(entt::registry& reg, entt::entity leader);
    bool InviteToParty(entt::registry& reg, entt::entity inviter, entt::entity invitee);
    bool AcceptInvite(entt::registry& reg, entt::entity player);
    void DeclineInvite(entt::registry& reg, entt::entity player);
    void LeaveParty(entt::registry& reg, entt::entity player);
    void KickMember(entt::registry& reg, entt::entity leader, uint32_t target_id);
    uint32_t DistributeXP(entt::registry& reg, entt::entity party_entity, uint32_t total_xp);
    bool DistributeLoot(entt::registry& reg, entt::entity party_entity, uint32_t item_id);
    void Update(entt::registry& reg, float dt);

    void SetLootMode(entt::registry& reg, entt::entity leader, LootMode mode);
    LootMode GetLootMode(entt::registry& reg, entt::entity party_entity) const;

    bool TransferLeadership(entt::registry& reg, entt::entity leader, uint32_t target_id);
    void SetPartyProperty(entt::registry& reg, entt::entity leader,
                          const std::string& key, int32_t value);

private:
    uint32_t next_party_id_ = 1;
    std::unordered_map<entt::entity, entt::entity> invites_;
    std::vector<InviteEntry> invite_timers_;
    std::mt19937 rng_;

    size_t round_robin_index_ = 0;
    float leader_activity_timer_ = 0.0f;
    float leader_timeout_ = 300.0f;
};

#pragma once
#include <entt/entt.hpp>
#include <unordered_map>
#include <random>

class PartySystem {
public:
    PartySystem();
    bool CreateParty(entt::registry& reg, entt::entity leader);
    bool InviteToParty(entt::registry& reg, entt::entity inviter, entt::entity invitee);
    bool AcceptInvite(entt::registry& reg, entt::entity player);
    void LeaveParty(entt::registry& reg, entt::entity player);
    void KickMember(entt::registry& reg, entt::entity leader, uint32_t target_id);
    uint32_t DistributeXP(entt::registry& reg, entt::entity party_entity, uint32_t total_xp);
    bool DistributeLoot(entt::registry& reg, entt::entity party_entity, uint32_t item_id);

private:
    uint32_t next_party_id_ = 1;
    std::unordered_map<entt::entity, entt::entity> invites_;
    std::mt19937 rng_;
};

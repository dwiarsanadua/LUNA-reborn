#pragma once
#include <entt/entt.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>

class GuildSystem {
public:
    bool CreateGuild(entt::registry& reg, entt::entity founder, const std::string& name);
    bool InviteToGuild(entt::registry& reg, entt::entity inviter, entt::entity invitee);
    bool AcceptInvite(entt::registry& reg, entt::entity player);
    void LeaveGuild(entt::registry& reg, entt::entity player);
    void KickMember(entt::registry& reg, entt::entity kicker, uint32_t target_id);
    bool SetRank(entt::registry& reg, entt::entity actor, uint32_t target_id, uint8_t rank);
    void SetNotice(entt::registry& reg, entt::entity actor, const std::string& notice);
    bool DepositGold(entt::registry& reg, entt::entity player, uint32_t amount);
    bool WithdrawGold(entt::registry& reg, entt::entity player, uint32_t amount);
    void Update(entt::registry& reg, float dt);

private:
    uint32_t next_guild_id_ = 1;
    std::unordered_map<entt::entity, entt::entity> invites_;
};

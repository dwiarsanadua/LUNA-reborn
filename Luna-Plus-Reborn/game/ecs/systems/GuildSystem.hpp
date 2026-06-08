#pragma once
#include <entt/entt.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <cstdint>
#include "../components/Inventory.hpp"

struct GuildSkill {
    uint32_t skill_id;
    uint32_t level;
    std::string name;
};

struct AuditEntry {
    uint32_t timestamp;
    uint32_t character_id;
    std::string action;
    int32_t amount;
    uint32_t balance_after;
};

struct GuildLevelTableEntry {
    uint32_t level;
    uint32_t exp_required;
    uint32_t max_members;
};

class GuildSystem {
public:
    GuildSystem();

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

    // Guild level-up
    uint32_t GetRequiredExp(uint32_t level) const;
    bool AddExp(entt::registry& reg, entt::entity guild_entity, uint32_t amount);
    bool LevelUp(entt::registry& reg, entt::entity guild_entity);

    // Guild skills
    bool LearnGuildSkill(entt::registry& reg, entt::entity actor, uint32_t guild_entity, uint32_t skill_id);
    bool UpgradeGuildSkill(entt::registry& reg, entt::entity actor, uint32_t guild_entity, uint32_t skill_id);
    std::vector<GuildSkill> GetGuildSkills(entt::registry& reg, entt::entity guild_entity) const;

    // Guild warehouse audit
    bool DepositItem(entt::registry& reg, entt::entity player, uint32_t guild_entity,
                     uint32_t item_id, uint16_t count);
    bool WithdrawItem(entt::registry& reg, entt::entity player, uint32_t guild_entity,
                      uint32_t item_id, uint16_t count);
    std::vector<AuditEntry> GetAuditLog(entt::registry& reg, entt::entity guild_entity,
                                        uint32_t limit = 50) const;

private:
    uint32_t next_guild_id_ = 1;
    std::unordered_map<entt::entity, entt::entity> invites_;

    std::vector<GuildLevelTableEntry> level_table_;
    std::unordered_map<uint32_t, std::vector<GuildSkill>> guild_skills_;
    std::unordered_map<uint32_t, std::vector<AuditEntry>> guild_audit_logs_;
    std::unordered_map<uint32_t, std::vector<ItemSlot>> guild_warehouses_;

    void InitLevelTable();
    void AddAuditEntry(uint32_t guild_id, uint32_t character_id,
                       const std::string& action, int32_t amount, uint32_t balance);
};

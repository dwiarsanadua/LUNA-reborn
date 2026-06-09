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

struct GuildWarScore {
    uint32_t guild_id = 0;
    uint32_t kill_score = 0;
    uint32_t death_score = 0;
    uint32_t war_score = 0;
    uint32_t hunted_monster_score = 0;
};

struct GuildMarkData {
    std::vector<uint8_t> data;
    uint32_t len = 0;
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

    bool DisbandGuild(entt::registry& reg, entt::entity actor);
    bool ChangeMaster(entt::registry& reg, entt::entity actor, uint32_t target_id);

    bool SetNickname(entt::registry& reg, entt::entity actor, uint32_t target_id, const std::string& nickname);
    std::string GetNickname(uint32_t guild_id, uint32_t character_id) const;

    bool SetMark(entt::registry& reg, entt::entity actor, const std::vector<uint8_t>& mark_data, uint32_t mark_len);
    bool GetMark(uint32_t guild_id, GuildMarkData& out_mark) const;

    void GuildChat(entt::registry& reg, entt::entity actor, const std::string& message);
    void BroadcastToGuild(entt::registry& reg, uint32_t guild_id, const std::string& message);

    void AddWarKillScore(uint32_t guild_id, uint32_t score);
    void AddWarDeathScore(uint32_t guild_id, uint32_t score);
    void AddHuntedMonsterScore(uint32_t guild_id, uint32_t score);
    uint32_t GetWarScore(uint32_t guild_id) const;
    const GuildWarScore* GetGuildWarScore(uint32_t guild_id) const;

    void SetWarehouseRankAccess(uint32_t guild_id, uint8_t min_rank);
    uint8_t GetWarehouseRankAccess(uint32_t guild_id) const;

    // Member management
    void NotifyMemberLevelUp(entt::registry& reg, entt::entity player);

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

    std::unordered_map<uint32_t, GuildWarScore> war_scores_;
    std::unordered_map<uint32_t, GuildMarkData> guild_marks_;
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::string>> nicknames_;
    std::unordered_map<uint32_t, uint8_t> warehouse_rank_access_;

    void InitLevelTable();
    void AddAuditEntry(uint32_t guild_id, uint32_t character_id,
                       const std::string& action, int32_t amount, uint32_t balance);
};

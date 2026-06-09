// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <entt/entt.hpp>

struct GuildMember {
    uint32_t character_id = 0;
    std::string name;
    std::string nickname;
    uint8_t rank = 5;
    bool is_online = false;
    uint32_t joined_at = 0;
    uint32_t contribution = 0;
    uint16_t level = 1;
    std::string job_name;
};

struct GuildData {
    uint32_t guild_id = 0;
    std::string name;
    uint32_t master_id = 0;
    uint32_t level = 1;
    uint32_t exp = 0;
    uint32_t gold = 0;
    uint32_t score = 0;
    std::string notice;
    std::vector<GuildMember> members;
};

struct GuildSkillInfo {
    uint32_t skill_id = 0;
    uint32_t level = 0;
    std::string name;
};

struct GuildWarehouseSlot {
    uint32_t item_id = 0;
    uint16_t count = 0;
};

enum GuildRank : uint8_t {
    GUILD_MASTER = 1,
    GUILD_VICEMASTER = 2,
    GUILD_SENIOR = 3,
    GUILD_JUNIOR = 4,
    GUILD_MEMBER = 5,
    GUILD_STUDENT = 6,
};

class GuildSystem {
public:
    GuildSystem();

    using PacketSender = std::function<void(uint32_t, const uint8_t*, size_t)>;
    void SetPacketSender(PacketSender sender);

    bool CreateGuild(entt::registry& reg, uint32_t founder_id, const std::string& founder_name, const std::string& name);
    bool DisbandGuild(entt::registry& reg, uint32_t actor_id);
    bool InviteMember(entt::registry& reg, uint32_t inviter_id, uint32_t invitee_id, const std::string& invitee_name);
    bool AcceptInvite(entt::registry& reg, uint32_t player_id);
    void DeclineInvite(uint32_t player_id);
    bool KickMember(entt::registry& reg, uint32_t kicker_id, uint32_t target_id);
    bool LeaveGuild(entt::registry& reg, uint32_t player_id);
    bool ChangeMaster(entt::registry& reg, uint32_t actor_id, uint32_t target_id);
    bool SetMemberRank(entt::registry& reg, uint32_t actor_id, uint32_t target_id, uint8_t new_rank);
    bool SetNotice(entt::registry& reg, uint32_t actor_id, const std::string& notice);
    bool SetNickname(entt::registry& reg, uint32_t actor_id, uint32_t target_id, const std::string& nickname);
    bool SetMark(entt::registry& reg, uint32_t actor_id, const std::vector<uint8_t>& mark_data);

    bool DepositGold(entt::registry& reg, uint32_t player_id, uint32_t amount);
    bool WithdrawGold(entt::registry& reg, uint32_t player_id, uint32_t amount);

    bool LearnSkill(entt::registry& reg, uint32_t actor_id, uint32_t skill_id);
    bool UpgradeSkill(entt::registry& reg, uint32_t actor_id, uint32_t skill_id);
    std::vector<GuildSkillInfo> GetGuildSkills(uint32_t guild_id) const;

    void GuildChat(uint32_t actor_id, const std::string& message);
    void BroadcastToGuild(uint32_t guild_id, uint16_t packet_type, const uint8_t* data, size_t len);
    void BroadcastMessage(uint32_t guild_id, const std::string& message);

    void AddWarKillScore(uint32_t guild_id, uint32_t score);
    void AddWarDeathScore(uint32_t guild_id, uint32_t score);
    uint32_t GetWarScore(uint32_t guild_id) const;

    void SetWarehouseRankAccess(uint32_t guild_id, uint8_t min_rank);
    bool OpenWarehouse(entt::registry& reg, uint32_t player_id);
    void CloseWarehouse(uint32_t player_id);

    void AddPlayer(entt::registry& reg, uint32_t character_id);
    void RemovePlayer(entt::registry& reg, uint32_t character_id);
    void Update(entt::registry& reg, float dt);

    uint32_t GetGuildId(uint32_t character_id) const;
    GuildData* GetGuild(uint32_t guild_id);
    const GuildData* GetGuildByMember(uint32_t character_id) const;
    std::string GetGuildName(uint32_t guild_id) const;

private:
    struct PendingInvite {
        uint32_t inviter_id = 0;
        uint32_t invitee_id = 0;
        uint32_t guild_id = 0;
        float timer = 0.0f;
        float timeout = 30.0f;
    };

    struct GuildWarScore {
        uint32_t kill_score = 0;
        uint32_t death_score = 0;
        uint32_t war_score = 0;
    };

    uint32_t next_guild_id_ = 1;
    std::unordered_map<uint32_t, GuildData> guilds_;
    std::unordered_map<uint32_t, uint32_t> member_to_guild_;
    std::vector<PendingInvite> pending_invites_;
    PacketSender send_packet_;

    std::unordered_map<uint32_t, std::vector<GuildSkillInfo>> guild_skills_;
    std::unordered_map<uint32_t, GuildWarScore> war_scores_;
    std::unordered_map<uint32_t, uint8_t> warehouse_rank_access_;
    std::unordered_map<uint32_t, std::vector<uint32_t>> warehouse_viewers_;

    static constexpr size_t MAX_WAREHOUSE_SLOTS = 60;
    static constexpr uint32_t MAX_GUILD_MEMBERS = 30;
    static constexpr float REGEN_SCORE_INTERVAL = 30.0f;

    void BroadcastMemberUpdate(uint32_t guild_id);
    uint32_t GetCharacterId(entt::entity entity) const;
    bool HasRankAccess(uint32_t guild_id, uint32_t character_id, uint8_t min_rank) const;
};

extern GuildSystem* g_guild_system;

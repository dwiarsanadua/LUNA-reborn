// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <entt/entt.hpp>

enum class LootMode : uint8_t {
    FREE_FOR_ALL = 0,
    ROUND_ROBIN = 1,
    MASTER_LOOT = 2,
    NEED_GREED = 3
};

struct PartyMemberData {
    entt::entity entity = entt::null;
    uint32_t character_id = 0;
    std::string name;
    uint16_t level = 1;
    int hp = 500;
    int max_hp = 500;
    uint16_t map_id = 0;
    bool is_leader = false;
    bool online = true;
};

struct PartyData {
    uint32_t party_id = 0;
    entt::entity leader_entity = entt::null;
    uint32_t leader_id = 0;
    std::vector<PartyMemberData> members;
    LootMode loot_mode = LootMode::FREE_FOR_ALL;
    bool share_exp = true;
    bool share_loot = false;
    float formed_at = 0.0f;
};

class PartySystem {
public:
    PartySystem();

    using PacketSender = std::function<void(uint32_t, const uint8_t*, size_t)>;

    void SetPacketSender(PacketSender sender);

    bool CreateParty(entt::registry& reg, entt::entity leader, uint32_t leader_char_id, const std::string& leader_name);
    bool InviteToParty(entt::registry& reg, entt::entity inviter, entt::entity invitee, uint32_t invitee_char_id);
    bool AcceptInvite(entt::registry& reg, entt::entity player);
    void DeclineInvite(entt::registry& reg, entt::entity player);
    void LeaveParty(entt::registry& reg, entt::entity player);
    bool KickMember(entt::registry& reg, entt::entity leader, uint32_t target_id);

    uint32_t GetPartyId(entt::entity player) const;
    PartyData* GetParty(uint32_t party_id);
    const PartyData* GetPartyByMember(entt::entity player) const;

    uint32_t DistributeXP(entt::registry& reg, uint32_t party_id, uint32_t total_xp, entt::entity killer);
    bool DistributeLoot(entt::registry& reg, uint32_t party_id, uint32_t item_id);
    void DistributeGold(entt::registry& reg, uint32_t party_id, uint32_t total_gold, entt::entity killer);

    void SetLootMode(entt::registry& reg, entt::entity leader, LootMode mode);
    bool TransferLeadership(entt::registry& reg, entt::entity new_leader);

    void BroadcastToParty(uint32_t party_id, uint16_t packet_type, const uint8_t* data, size_t len);
    void UpdatePartyMember(entt::registry& reg, entt::entity player);

    void Update(entt::registry& reg, float dt);

private:
    struct PendingInvite {
        uint32_t inviter_id = 0;
        uint32_t invitee_id = 0;
        uint32_t party_id = 0;
        float timer = 0.0f;
        float timeout = 30.0f;
    };

    uint32_t next_party_id_ = 1;
    std::unordered_map<uint32_t, PartyData> parties_;
    std::unordered_map<uint32_t, uint32_t> member_to_party_;
    std::vector<PendingInvite> pending_invites_;
    PacketSender send_packet_;
    size_t round_robin_idx_ = 0;

    void RemoveMember(uint32_t party_id, uint32_t character_id);
    void BroadcastPartyUpdate(uint32_t party_id);
    uint32_t GetCharacterId(entt::entity entity) const;
};

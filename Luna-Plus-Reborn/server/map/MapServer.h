// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include "systems/GridSystem.hpp"
#include "systems/TriggerSystem.hpp"
#include "systems/MapScriptRuntime.hpp"
#include <ecs/systems/SkillSystem.hpp>
#include <ecs/components/QuestLog.hpp>

// entt::entity used for connected player tracking

class NetworkLayer;
class Database;

struct PlayerData {
    int id;
    std::string name;
    int level;
    int class_id;
    float pos_x, pos_y, pos_z;
    int hp, max_hp;
};

enum class DungeonState : uint8_t {
    WAITING = 0,
    ACTIVE = 1,
    BOSS_ACTIVE = 2,
    COMPLETED = 3,
    TIMEOUT = 4
};

struct DungeonRewardEntry {
    uint32_t item_id;
    uint16_t count;
    float drop_chance;
};

struct DungeonInstance {
    uint32_t instance_id;
    uint32_t dungeon_template_id;
    DungeonState state = DungeonState::WAITING;
    std::vector<uint32_t> party_member_ids;
    uint32_t boss_entity_id = 0;
    bool boss_defeated = false;
    float elapsed_time = 0.0f;
    float time_limit = 1800.0f;
    std::vector<DungeonRewardEntry> reward_table;
    entt::entity dungeon_root = entt::null;
};

class CombatSystem;
class AISystem;
class MovementSystem;
class ItemSystem;
class QuestSystem;
class SpawnSystem;

class MapServer {
public:
    MapServer();
    ~MapServer();

    bool Initialize(int map_id, uint16_t port);
    void Shutdown();
    void Update(float dt);
    bool IsRunning() const { return running_; }

    void SpawnPlayer(int entity_id, const PlayerData& data);
    void DespawnPlayer(int entity_id);
    void SpawnMonster(int template_id, const glm::vec3& pos);
    void DespawnMonster(int entity_id);
    void SendNPCList(int player_entity_id);
    bool CheckEncounterTrigger(float player_x, float player_z);
    void SavePlayerPosition(int entity_id, float x, float y, float z);
    void HandlePacket(uint16_t type, const uint8_t* payload, size_t len);
    void SendWorldSnapshot();

    CombatSystem& GetCombatSystem();
    AISystem& GetAISystem();
    MovementSystem& GetMovementSystem();
    ItemSystem& GetItemSystem();
    QuestSystem& GetQuestSystem();
    entt::registry& GetRegistry() { return *registry_; }
    int GetMapId() const { return map_id_; }

    void TeleportPlayer(uint32_t map_id, float x, float y, float z);
    void RunMapScript(const std::string& path, entt::entity player);
    void SendTriggerNotify(uint32_t trigger_id, uint8_t trigger_type,
                           uint32_t param0, uint32_t param1, const std::string& message = {});

    Database& GetDatabase();
    NetworkLayer& GetNetworkLayer();

    // Dungeon management
    uint32_t CreateDungeonInstance(uint32_t template_id, const std::vector<uint32_t>& party_ids);
    bool EnterDungeon(uint32_t instance_id, entt::entity player);
    bool TriggerBossEncounter(uint32_t instance_id, uint32_t boss_template_id);
    void CompleteDungeon(uint32_t instance_id);
    void CleanupDungeon(uint32_t instance_id);
    DungeonInstance* GetDungeonInstance(uint32_t instance_id);

private:
    bool running_ = false;
    int map_id_ = 0;
    uint16_t port_ = 0;
    std::unique_ptr<NetworkLayer> network_;
    std::unique_ptr<Database> db_;
    std::unique_ptr<entt::registry> registry_;
    std::unique_ptr<CombatSystem> combat_;
    std::unique_ptr<AISystem> ai_;
    std::unique_ptr<MovementSystem> movement_;
    std::unique_ptr<ItemSystem> item_;
    std::unique_ptr<QuestSystem> quest_;
    std::unique_ptr<SpawnSystem> spawn_sys_;
    GridSystem grid_;
    TriggerSystem triggers_;
    MapScriptRuntime script_runtime_;
    SkillSystem skill_sys_;

    float auto_save_timer_ = 0.0f;
    float respawn_timer_ = 0.0f;

    // Dungeon instances
    std::unordered_map<uint32_t, DungeonInstance> dungeons_;
    uint32_t next_dungeon_id_ = 1;

    void DistributeDungeonRewards(uint32_t instance_id);
    void SendEntitySpawn(uint32_t entity_id, int8_t entity_type,
                         const std::string& model_id, const std::string& name,
                         uint16_t level, float x, float y, float z, float hp_pct);
    void SendEntityTransform(uint32_t entity_id, float x, float y, float z, const char* anim = "walk");

    bool player_joined_ = false;
    PlayerData connected_player_{};
    entt::entity player_entity_ = entt::null;

    void SendEntityDespawn(uint32_t entity_id, int8_t reason);
    void HandleCombatAttack(uint16_t ack_type, const uint8_t* payload, size_t len);
    void HandleChat(const uint8_t* payload, size_t len);
    void BroadcastMonsterMovement(float dt);
    void BroadcastNewMonsterSpawns();
    void SendCharLifeUpdate();
    void SendInventorySync();
    void GrantLootToPlayer(uint32_t item_id, uint16_t count);
    entt::entity FindMonsterEntity(uint32_t entity_id) const;
    void RegisterWithDistribute();
    void HandleChangeMap(const uint8_t* payload, size_t len);
    static glm::vec3 GetMapSpawnPosition(uint16_t map_id);
    void HandlePartyCreate(const uint8_t* payload, size_t len);
    void HandlePartyInvite(const uint8_t* payload, size_t len);
    void HandlePartyLeave(const uint8_t* payload, size_t len);
    void SendPartyInfo(uint8_t result, uint16_t ack_type);
    void HandleStorageList(const uint8_t* payload, size_t len);
    void HandleStorageDeposit(const uint8_t* payload, size_t len);
    void HandleStorageWithdraw(const uint8_t* payload, size_t len);
    void SendStorageSync(uint8_t result, uint16_t ack_type);
    void HandlePartyChat(const uint8_t* payload, size_t len);
    void HandleWhisper(const uint8_t* payload, size_t len);
    void HandleFriendList(const uint8_t* payload, size_t len);
    void HandleFriendAdd(const uint8_t* payload, size_t len);
    void HandleFriendDelete(const uint8_t* payload, size_t len);
    void SendFriendList(uint8_t result, uint16_t ack_type);
    void HandleGuildCreate(const uint8_t* payload, size_t len);
    void HandleGuildInfo(const uint8_t* payload, size_t len);
    void HandleGuildInvite(const uint8_t* payload, size_t len);
    void HandleGuildLeave(const uint8_t* payload, size_t len);
    void SendGuildInfo(uint8_t result, uint16_t ack_type);
    void HandleGuildChat(const uint8_t* payload, size_t len);
    void SendTradeState(uint8_t result, uint16_t ack_type, bool completed = false);
    void HandleTradeApply(const uint8_t* payload, size_t len);
    void HandleTradeCancel(const uint8_t* payload, size_t len);
    void HandleTradeAddItem(const uint8_t* payload, size_t len);
    void HandleTradeSetGold(const uint8_t* payload, size_t len);
    void HandleTradeConfirm(const uint8_t* payload, size_t len);
    void ExecuteTrade();
    void SeedMarketData();
    void SendConsignmentList(uint8_t result, uint16_t ack_type,
        const std::string& query, bool mine_only, bool bids_only);
    void HandleConsignmentList(const uint8_t* payload, size_t len, uint16_t ack_type);
    void HandleConsignmentRegister(const uint8_t* payload, size_t len);
    void HandleConsignmentTrade(const uint8_t* payload, size_t len);
    void HandleConsignmentCancel(const uint8_t* payload, size_t len);
    void SendStreetStallState(uint8_t result, uint16_t ack_type);
    void HandleStreetStallOpen(const uint8_t* payload, size_t len);
    void HandleStreetStallAddItem(const uint8_t* payload, size_t len);
    void HandleStreetStallBuy(const uint8_t* payload, size_t len);
    void HandleStreetStallClose(const uint8_t* payload, size_t len);
    void HandleStreetStallList(const uint8_t* payload, size_t len);
    void ReturnStallItems(uint32_t owner_id);
    void HandleQuestStart(const uint8_t* payload, size_t len);
    void HandleQuestEnd(const uint8_t* payload, size_t len);
    void HandleQuestList(const uint8_t* payload, size_t len);
    void HandleDungeonEntrance(const uint8_t* payload, size_t len);
    void HandleDungeonInfo(const uint8_t* payload, size_t len);
    void SendQuestList(uint8_t result);
    void SendQuestUpdate(uint32_t quest_id, uint8_t obj_index, uint16_t current, uint16_t required);
    void OnMonsterKilled(uint32_t monster_template_id);
    void ApplyQuestProgress(QuestObjective::Type type, uint32_t target_id, uint16_t amount = 1);
    void LoadPlayerQuests(int character_id);
    void SavePlayerQuests(int character_id);
    void UpdatePlayerVisibility();

    struct PlayerInvSlot {
        uint8_t slot = 0;
        uint32_t item_id = 0;
        uint16_t count = 0;
    };
    PlayerInvSlot* FindInvSlot(uint8_t slot);
    const PlayerInvSlot* FindInvSlot(uint8_t slot) const;

    std::vector<PlayerInvSlot> player_inventory_;
    std::vector<PlayerInvSlot> player_storage_;
    uint32_t player_gold_ = 100;
    uint32_t storage_gold_ = 0;
    uint64_t player_exp_ = 0;
    int last_sent_hp_ = -1;

    struct PartyMemberState {
        uint32_t character_id = 0;
        std::string name;
        uint16_t level = 1;
        int hp = 500;
        int max_hp = 500;
        uint16_t map_id = 51;
        bool is_leader = false;
    };
    struct PartyState {
        uint32_t party_id = 0;
        uint32_t leader_id = 0;
        std::vector<PartyMemberState> members;
    };
    PartyState party_;
    bool has_party_ = false;
    uint32_t next_party_id_ = 1;

    struct FriendState {
        uint32_t character_id = 0;
        std::string name;
        uint16_t level = 1;
        bool online = false;
        uint16_t map_id = 51;
    };
    std::vector<FriendState> friends_;
    uint32_t next_friend_id_ = 8000;

    struct GuildMemberState {
        uint32_t character_id = 0;
        std::string name;
        uint16_t level = 1;
        uint8_t rank = 0;
        bool online = true;
    };
    struct GuildState {
        uint32_t guild_id = 0;
        std::string name;
        uint8_t level = 1;
        uint32_t gp = 0;
        uint32_t master_id = 0;
        std::vector<GuildMemberState> members;
    };
    GuildState guild_;
    bool has_guild_ = false;
    uint32_t next_guild_id_ = 1;

    struct TradeOfferSlot {
        uint8_t trade_slot = 0;
        uint8_t inv_slot = 0;
        uint32_t item_id = 0;
        uint16_t count = 0;
    };
    struct TradeSessionState {
        uint32_t session_id = 0;
        uint32_t partner_id = 0;
        std::string partner_name;
        std::vector<TradeOfferSlot> player_items;
        std::vector<TradeOfferSlot> partner_items;
        uint32_t player_gold = 0;
        uint32_t partner_gold = 0;
        bool player_confirmed = false;
        bool partner_confirmed = false;
        bool active = false;
    };
    TradeSessionState trade_;
    uint32_t next_trade_id_ = 1;

    struct ConsignmentListingState {
        uint64_t id = 0;
        uint32_t seller_id = 0;
        std::string seller_name;
        uint32_t item_id = 0;
        uint16_t count = 1;
        uint8_t enchant = 0;
        uint32_t bid_price = 0;
        uint32_t buyout_price = 0;
        uint32_t current_bid = 0;
        uint32_t bidder_id = 0;
        std::string bidder_name;
        float time_remaining = 86400.0f;
        bool sold = false;
        bool active = true;
    };
    std::vector<ConsignmentListingState> consignment_listings_;
    uint64_t next_consignment_id_ = 1;

    struct StreetStallSlotState {
        uint8_t slot = 0;
        uint32_t item_id = 0;
        uint16_t count = 0;
        uint32_t price = 0;
    };
    struct StreetStallState {
        uint32_t owner_id = 0;
        std::string owner_name;
        std::string title;
        bool open = false;
        std::vector<StreetStallSlotState> items;
    };
    std::unordered_map<uint32_t, StreetStallState> street_stalls_;

    struct MonsterNetState {
        float broadcast_timer = 0.0f;
        glm::vec3 last_sent{0.0f};
        bool initialized = false;
    };
    std::unordered_map<uint32_t, MonsterNetState> monster_net_;
    std::unordered_set<uint32_t> visible_monsters_;
    float aoi_radius_ = 80.0f;
    float monster_broadcast_interval_ = 0.12f;
    uint8_t next_loot_slot_ = 10;
};

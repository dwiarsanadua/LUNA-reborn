// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

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

    CombatSystem& GetCombatSystem();
    AISystem& GetAISystem();
    MovementSystem& GetMovementSystem();
    ItemSystem& GetItemSystem();
    QuestSystem& GetQuestSystem();

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
    std::unique_ptr<NetworkLayer> network_;
    std::unique_ptr<Database> db_;
    std::unique_ptr<entt::registry> registry_;
    std::unique_ptr<CombatSystem> combat_;
    std::unique_ptr<AISystem> ai_;
    std::unique_ptr<MovementSystem> movement_;
    std::unique_ptr<ItemSystem> item_;
    std::unique_ptr<QuestSystem> quest_;

    float auto_save_timer_ = 0.0f;

    // Dungeon instances
    std::unordered_map<uint32_t, DungeonInstance> dungeons_;
    uint32_t next_dungeon_id_ = 1;

    void DistributeDungeonRewards(uint32_t instance_id);
};

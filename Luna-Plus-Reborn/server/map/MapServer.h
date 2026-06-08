// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <memory>
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
};

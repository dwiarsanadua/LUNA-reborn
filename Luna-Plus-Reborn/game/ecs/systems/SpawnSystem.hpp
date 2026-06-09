#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class SpawnMonsterType : uint8_t {
    Normal = 0,
    Elite = 1,
    Boss = 2,
    FieldBoss = 3,
    SubMonster = 4,
};

struct SpawnMonsterTemplate {
    uint32_t id = 0;
    std::string name;
    SpawnMonsterType type = SpawnMonsterType::Normal;
    uint16_t level = 1;
    int32_t hp = 100;
    int32_t attack = 10;
    int32_t defense = 5;
    float move_speed = 3.5f;
    float aggro_range = 10.0f;
    int32_t exp_reward = 0;
    int32_t gold_reward = 0;
    bool is_boss = false;
};

struct SpawnPoint {
    uint32_t id;
    int map_id;
    uint32_t monster_id;
    float x, y, z;
    float respawn_time;
    float respawn_variance; // random +/- seconds added to respawn
    int max_count;
    int current_count;
    float aggro_range;
    uint32_t patrol_radius;
    float spawn_radius; // random position within this radius
    SpawnMonsterType monster_type = SpawnMonsterType::Normal;
    bool is_boss = false;
    uint16_t level = 1;
    uint32_t group_id = 0; // 0 = no group
};

struct ScheduledRespawn {
    uint32_t spawn_point_id;
    float delay;
    float elapsed;
};

class SpawnSystem {
public:
    void Update(entt::registry& registry, float dt);
    void SpawnMonster(entt::registry& registry, uint32_t monster_id,
                      const glm::vec3& pos, uint16_t level);
    void DespawnEntity(entt::registry& registry, entt::entity entity);

    void LoadSpawnData(const std::string& json_path);
    void LoadMonsterTemplates(const std::string& json_path);
    void SpawnMonstersForMap(entt::registry& registry, int map_id);
    void SpawnGroup(entt::registry& registry, uint32_t group_id);
    void RespawnMonster(entt::registry& registry, uint32_t spawn_id, float delay);
    void DespawnAll(entt::registry& registry);

    const SpawnMonsterTemplate* GetMonsterTemplate(uint32_t monster_id) const;

private:
    std::vector<SpawnPoint> spawn_points_;
    std::unordered_map<uint32_t, std::vector<uint32_t>> map_spawn_indices_;
    std::unordered_map<uint32_t, std::vector<uint32_t>> group_spawn_indices_;
    std::vector<ScheduledRespawn> pending_respawns_;
    std::unordered_map<uint32_t, SpawnMonsterTemplate> monster_templates_;

    entt::entity SpawnSingle(entt::registry& registry, const SpawnPoint& sp);
    glm::vec3 RandomPosition(const SpawnPoint& sp) const;
    void ParseMonsterArray(const json& arr);
    void ParseSpawnItem(const json& item, uint32_t monster_id_default);
    void AddSpawnPoint(const SpawnPoint& sp);
};

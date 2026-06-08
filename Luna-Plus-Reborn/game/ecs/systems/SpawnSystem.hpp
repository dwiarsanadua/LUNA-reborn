#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct SpawnPoint {
    uint32_t id;
    int map_id;
    uint32_t monster_id;
    float x, y, z;
    float respawn_time;
    int max_count;
    int current_count;
    float aggro_range;
    uint32_t patrol_radius;
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
    void SpawnMonstersForMap(entt::registry& registry, int map_id);
    void RespawnMonster(entt::registry& registry, uint32_t spawn_id, float delay);
    void DespawnAll(entt::registry& registry);

private:
    std::vector<SpawnPoint> spawn_points_;
    std::unordered_map<uint32_t, std::vector<uint32_t>> map_spawn_indices_;
    std::vector<ScheduledRespawn> pending_respawns_;

    entt::entity SpawnSingle(entt::registry& registry, const SpawnPoint& sp);
};

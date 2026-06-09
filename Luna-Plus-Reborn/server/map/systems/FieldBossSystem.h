#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

struct BossPhase {
    uint32_t phase_id;
    float hp_threshold;
    uint32_t skill_id;
    float duration;
    std::string name;
};

struct FieldBossData {
    uint32_t id;
    uint32_t monster_id;
    int map_id;
    glm::vec3 spawn_position;
    float respawn_hours;
    uint32_t min_party_size;
    std::vector<uint32_t> loot_table;
    std::vector<BossPhase> phases;
};

class FieldBossSystem {
public:
    void LoadFieldBossData(const std::string& json_path);
    void Update(entt::registry& registry, float dt);

    void SpawnFieldBoss(uint32_t boss_id, int map_id, entt::registry& registry);
    void OnFieldBossDeath(entt::entity boss, entt::entity killer, entt::registry& registry);

private:
    std::vector<FieldBossData> field_bosses_;
    std::unordered_map<uint32_t, float> respawn_timers_;

    void BroadcastFieldBossSpawn(uint32_t boss_id, int map_id, const glm::vec3& pos);
    void BroadcastFieldBossDeath(uint32_t boss_id, const std::string& killer_name);
};

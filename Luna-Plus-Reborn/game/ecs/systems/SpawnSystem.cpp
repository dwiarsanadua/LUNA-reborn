#include "SpawnSystem.hpp"
#include "../components/Transform.hpp"
#include "../components/CharacterStats.hpp"
#include "../components/SpawnInfo.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>

void SpawnSystem::Update(entt::registry& registry, float dt) {
    auto view = registry.view<SpawnInfo, Transform>();
    for (auto entity : view) {
        auto& spawn = view.get<SpawnInfo>(entity);
        if (!spawn.is_alive) {
            spawn.respawn_timer += dt;
            if (spawn.respawn_timer >= spawn.respawn_time) {
                spawn.is_alive = true;
                spawn.respawn_timer = 0.0f;
            }
        }
    }
}

void SpawnSystem::SpawnMonster(entt::registry& registry, uint32_t monster_id,
                                const glm::vec3& pos, uint16_t level) {
    auto entity = registry.create();
    auto& xform = registry.emplace<Transform>(entity);
    xform.position = pos;
    auto& stats = registry.emplace<CharacterStats>(entity);
    stats.level = level;
    registry.emplace<TagMonster>(entity);
}

void SpawnSystem::DespawnEntity(entt::registry& registry, entt::entity entity) {
    if (registry.valid(entity)) registry.destroy(entity);
}

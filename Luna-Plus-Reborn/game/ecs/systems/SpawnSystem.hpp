#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>

class SpawnSystem {
public:
    void Update(entt::registry& registry, float dt);
    void SpawnMonster(entt::registry& registry, uint32_t monster_id,
                      const glm::vec3& pos, uint16_t level);
    void DespawnEntity(entt::registry& registry, entt::entity entity);
};

#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>

class MovementSystem {
public:
    void Update(entt::registry& registry, float dt);
    void HandleMoveRequest(entt::registry& registry, entt::entity entity,
                           const glm::vec3& destination, float speed);
    void HandleStopMove(entt::registry& registry, entt::entity entity);
    bool IsWalkable(const glm::vec3& pos) const;
};

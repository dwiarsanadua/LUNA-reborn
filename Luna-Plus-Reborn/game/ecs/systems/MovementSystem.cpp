#include "MovementSystem.hpp"
#include "../components/Transform.hpp"
#include "../components/Movement.hpp"
#include "../components/CombatState.hpp"
#include <spdlog/spdlog.h>

void MovementSystem::Update(entt::registry& registry, float dt) {
    auto view = registry.view<Transform, Movement>();
    for (auto entity : view) {
        // Animation lock check
        if (auto* combat = registry.try_get<CombatState>(entity)) {
            if (combat->is_casting || combat->is_animation_locked) {
                // Cannot move while casting or locked in animation
                continue;
            }
        }

        auto& xform = view.get<Transform>(entity);
        auto& move = view.get<Movement>(entity);

        if (!move.is_moving) continue;

        glm::vec3 diff = move.destination - xform.position;
        float dist = glm::length(diff);

        if (dist < 0.1f) {
            xform.position = move.destination;
            move.is_moving = false;
            move.velocity = {0.0f, 0.0f, 0.0f};
            continue;
        }

        glm::vec3 dir = diff / dist;
        float step = move.current_speed * dt;
        if (step > dist) step = dist;

        xform.position += dir * step;
        move.velocity = dir * move.current_speed;
    }
}

void MovementSystem::HandleMoveRequest(entt::registry& registry, entt::entity entity,
                                        const glm::vec3& destination, float speed) {
    if (!registry.valid(entity)) return;
    auto& move = registry.get<Movement>(entity);
    move.destination = destination;
    move.speed = speed;
    move.current_speed = speed;
    move.is_moving = true;
}

void MovementSystem::HandleStopMove(entt::registry& registry, entt::entity entity) {
    if (!registry.valid(entity)) return;
    auto& move = registry.get<Movement>(entity);
    move.is_moving = false;
    move.velocity = {0.0f, 0.0f, 0.0f};
}

bool MovementSystem::IsWalkable(const glm::vec3& pos) const {
    if (pos.x < -50.0f || pos.x > 50.0f || pos.z < -50.0f || pos.z > 50.0f)
        return false;
    return pos.y >= -10.0f;
}

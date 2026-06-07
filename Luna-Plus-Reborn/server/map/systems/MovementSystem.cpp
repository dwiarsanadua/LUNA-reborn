#include "MovementSystem.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <algorithm>

MovementSystem::MovementSystem() = default;

void MovementSystem::Update(entt::registry& registry, float dt) {
    auto view = registry.view<MovementComponent>();
    for (auto entity : view) {
        auto& move = view.get<MovementComponent>(entity);
        if (!move.is_moving) continue;

        // Move toward target position using linear interpolation
        // In production, would have actual Transform component
        move.last_update_time += dt;
    }
}

bool MovementSystem::ValidatePosition(const glm::vec3& pos, const glm::vec3& prev_pos, float speed_limit) {
    float dist = glm::distance(pos, prev_pos);
    return dist <= speed_limit;
}

bool MovementSystem::DetectTeleport(const glm::vec3& pos, const glm::vec3& prev_pos, float max_distance) {
    float dist = glm::distance(pos, prev_pos);
    if (dist > max_distance) {
        spdlog::warn("Movement: teleport detected - distance={}", dist);
        return true;
    }
    return false;
}

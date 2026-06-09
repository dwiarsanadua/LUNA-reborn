#include "MovementSystem.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <algorithm>

MovementSystem::MovementSystem() = default;

void MovementSystem::Update(entt::registry& registry, float dt) {
    // Process player movement from input
    auto view = registry.view<Transform, Movement, MovementBroadcast>();
    for (auto entity : view) {
        auto& xform = view.get<Transform>(entity);
        auto& move = view.get<Movement>(entity);
        auto& bc = view.get<MovementBroadcast>(entity);

        if (!move.is_moving) continue;

        // Record history for interpolation
        bc.history.push_back({xform.position, bc.broadcast_timer});
        if (bc.history.size() > 10) bc.history.erase(bc.history.begin());

        // Server-authoritative position update
        glm::vec3 prev = xform.position;

        // Apply velocity
        if (glm::length(move.velocity) > 0.1f) {
            glm::vec3 dir = glm::normalize(move.velocity);
            float speed = move.current_speed * dt;
            xform.position += dir * speed;
        }

        // Validate position
        if (!ValidatePosition(xform.position, prev, speed_limit_)) {
            spdlog::warn("Movement: speed limit exceeded for entity {}",
                         static_cast<uint32_t>(entity));
            xform.position = prev;
            continue;
        }
        if (DetectTeleport(xform.position, prev, teleport_threshold_)) {
            spdlog::warn("Movement: teleport detected for entity {}",
                         static_cast<uint32_t>(entity));
            xform.position = prev;
            continue;
        }
        if (!IsWithinMapBounds(xform.position)) {
            spdlog::warn("Movement: out of map bounds for entity {}",
                         static_cast<uint32_t>(entity));
            xform.position = prev;
            continue;
        }

        // Broadcast timer (10 Hz)
        bc.broadcast_timer += dt;
        if (bc.broadcast_timer >= bc.broadcast_interval) {
            bc.broadcast_timer = 0.0f;

            // In production, would send position update to network layer
            // for broadcast to all nearby players in same chunk
            if (registry.all_of<TagPlayer>(entity)) {
                // BroadcastTransform(entity, xform.position);
            }
        }
    }

    // Interpolate monster movement
    auto mon_view = registry.view<MovementBroadcast>();
    for (auto entity : mon_view) {
        auto& bc = mon_view.get<MovementBroadcast>(entity);
        if (bc.history.size() >= 2) {
            bc.interpolation_time += dt;
            float t = bc.interpolation_time / bc.broadcast_interval;
            auto& xform = registry.get<Transform>(entity);
            xform.position = InterpolatePosition(bc.history, std::min(t, 1.0f));
        }
    }
}

bool MovementSystem::ValidatePosition(const glm::vec3& pos, const glm::vec3& prev_pos, float speed_limit) {
    float dist = glm::distance(pos, prev_pos);
    return dist <= speed_limit;
}

bool MovementSystem::DetectTeleport(const glm::vec3& pos, const glm::vec3& prev_pos, float max_distance) {
    float dist = glm::distance(pos, prev_pos);
    return dist > max_distance;
}

glm::vec3 MovementSystem::InterpolatePosition(const std::vector<PositionHistory>& history, float t) {
    if (history.size() < 2) return history.empty() ? glm::vec3(0) : history.back().position;
    size_t idx = static_cast<size_t>((history.size() - 1) * t);
    idx = std::min(idx, history.size() - 2);
    float local_t = (history.size() - 1) * t - static_cast<float>(idx);
    return glm::mix(history[idx].position, history[idx + 1].position, local_t);
}

bool MovementSystem::IsWithinMapBounds(const glm::vec3& pos) const {
    if (pos.x < map_bounds_.min_x || pos.x > map_bounds_.max_x) return false;
    if (pos.z < map_bounds_.min_z || pos.z > map_bounds_.max_z) return false;
    if (pos.y < -50.0f || pos.y > 200.0f) return false;
    return true;
}

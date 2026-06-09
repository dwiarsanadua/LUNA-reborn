// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/components/Movement.hpp>
#include <ecs/components/Tag.hpp>

struct PositionHistory {
    glm::vec3 position;
    float timestamp;
};

struct MovementBroadcast {
    float broadcast_timer = 0.0f;
    float broadcast_interval = 0.1f; // 10 Hz
    std::vector<PositionHistory> history;
    float interpolation_time = 0.0f;
};

struct MapBounds {
    float min_x = -500.0f, max_x = 500.0f;
    float min_z = -500.0f, max_z = 500.0f;
};

class MovementSystem {
public:
    MovementSystem();
    void Update(entt::registry& registry, float dt);

    bool ValidatePosition(const glm::vec3& pos, const glm::vec3& prev_pos, float speed_limit);
    bool DetectTeleport(const glm::vec3& pos, const glm::vec3& prev_pos, float max_distance);
    bool IsWithinMapBounds(const glm::vec3& pos) const;
    glm::vec3 InterpolatePosition(const std::vector<PositionHistory>& history, float t);
    void SetMapBounds(const MapBounds& bounds) { map_bounds_ = bounds; }

private:
    float speed_limit_ = 20.0f;
    float teleport_threshold_ = 50.0f;
    MapBounds map_bounds_;
};

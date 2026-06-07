// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>

struct MovementComponent {
    glm::vec3 velocity{0};
    glm::vec3 target_position{0};
    float speed = 5.0f;
    bool is_moving = false;
    float last_update_time = 0.0f;
};

class MovementSystem {
public:
    MovementSystem();
    void Update(entt::registry& registry, float dt);

    bool ValidatePosition(const glm::vec3& pos, const glm::vec3& prev_pos, float speed_limit);
    bool DetectTeleport(const glm::vec3& pos, const glm::vec3& prev_pos, float max_distance);

private:
    float speed_limit_ = 20.0f;
    float teleport_threshold_ = 50.0f;
};

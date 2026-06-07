#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

struct Movement {
    glm::vec3 velocity{0.0f};
    glm::vec3 destination{0.0f};
    float speed = 5.0f;
    float current_speed = 0.0f;
    bool is_moving = false;
    bool is_running = false;

    std::vector<glm::vec3> path;
    size_t path_index = 0;
    bool use_pathfinding = false;

    float move_start_time = 0.0f;
    float last_update_time = 0.0f;

    glm::vec3 interpolated_position{0.0f};
    glm::vec3 server_position{0.0f};
    float interpolation_progress = 1.0f;
};

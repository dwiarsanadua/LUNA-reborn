#pragma once
#include <cstdint>
#include <glm/glm.hpp>

struct SpawnInfo {
    uint32_t spawn_rule_id;
    uint32_t monster_id;
    uint16_t level;
    uint8_t quantity;
    float respawn_time;
    float respawn_timer = 0.0f;
    bool is_alive = true;
    bool is_boss = false;
};

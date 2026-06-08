#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>
#include <utility>

struct AIComponent {
    enum State : uint8_t {
        Idle = 0, Patrol = 1, Chase = 2, Attack = 3,
        Return = 4, Flee = 5, PatrolCombat = 6,
        Stun = 7, Sleep = 8,
    };
    State state = Idle;
    uint32_t aggro_target = 0;
    std::vector<uint32_t> aggro_list;
    float aggro_range = 10.0f;
    float attack_range = 3.0f;
    float chase_range = 30.0f;
    glm::vec3 spawn_position{0.0f};
    std::vector<glm::vec3> patrol_points;
    int current_patrol_index = 0;
    float patrol_wait_time = 0.0f;
    float patrol_wait_timer = 0.0f;
    float enrage_timer = 0.0f;
    float state_timer = 0.0f;
    float skill_timer = 0.0f;
    float last_attack_time = 0.0f;
    int32_t threat = 0;
    std::vector<std::pair<uint32_t, int32_t>> threat_table;

    void AddThreat(uint32_t entity_id, int32_t amount) {
        threat_table.push_back({entity_id, amount});
        aggro_list.push_back(entity_id);
    }
    uint32_t GetHighestThreat() const {
        uint32_t highest_id = 0;
        int32_t highest = -1;
        for (auto& [id, t] : threat_table) {
            if (t > highest) { highest = t; highest_id = id; }
        }
        return highest_id;
    }
    void ClearAggro() {
        aggro_target = 0;
        aggro_list.clear();
        threat_table.clear();
        threat = 0;
    }
};

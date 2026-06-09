#pragma once
#include <cstdint>
#include <string>
#include <glm/glm.hpp>

struct CharacterStats;

class ValidationSystem {
public:
    static bool ValidateMovement(
        const glm::vec3& from,
        const glm::vec3& to,
        float delta_time,
        float max_speed,
        uint32_t entity_id
    );

    static bool ValidateDamage(
        int32_t reported_damage,
        const CharacterStats& attacker,
        const CharacterStats& defender,
        uint16_t skill_id
    );

    static bool ValidatePosition(
        const glm::vec3& pos,
        int32_t map_id
    );

    static bool ValidateItemAction(
        uint32_t character_id,
        uint32_t item_id,
        uint16_t count,
        const std::string& action
    );
};

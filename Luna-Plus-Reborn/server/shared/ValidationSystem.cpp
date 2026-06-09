#include "ValidationSystem.hpp"
#include <ecs/components/CharacterStats.hpp>
#include <spdlog/spdlog.h>
#include <cmath>
#include <chrono>
#include <unordered_map>
#include <glm/glm.hpp>

static std::unordered_map<uint32_t, glm::vec3> s_previous_positions;
static std::unordered_map<uint32_t, uint64_t> s_last_move_time;

static uint64_t NowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

bool ValidationSystem::ValidateMovement(
    const glm::vec3& from,
    const glm::vec3& to,
    float delta_time,
    float max_speed,
    uint32_t entity_id)
{
    if (delta_time <= 0.0f) {
        spdlog::warn("ValidateMovement: invalid delta_time={} for entity {}", delta_time, entity_id);
        return false;
    }

    float dx = to.x - from.x;
    float dz = to.z - from.z;
    float distance = std::sqrt(dx * dx + dz * dz);

    float max_allowed = max_speed * delta_time * 1.5f;
    if (distance > max_allowed) {
        spdlog::warn("ValidateMovement: entity {} moved {} units (max allowed {}), possible speed hack",
                     entity_id, distance, max_allowed);
        return false;
    }

    auto prev_it = s_previous_positions.find(entity_id);
    if (prev_it != s_previous_positions.end()) {
        float warp_dx = to.x - prev_it->second.x;
        float warp_dz = to.z - prev_it->second.z;
        float warp_dist = std::sqrt(warp_dx * warp_dx + warp_dz * warp_dz);

        auto time_it = s_last_move_time.find(entity_id);
        uint64_t now = NowMs();
        uint64_t elapsed = (time_it != s_last_move_time.end()) ? (now - time_it->second) : 0;

        if (elapsed > 0 && elapsed < 5000) {
            float warp_max = max_speed * (elapsed / 1000.0f) * 2.0f;
            if (warp_dist > warp_max) {
                spdlog::warn("ValidateMovement: entity {} warped {} units in {}ms, possible teleport hack",
                             entity_id, warp_dist, elapsed);
                return false;
            }
        }
    }

    s_previous_positions[entity_id] = to;
    s_last_move_time[entity_id] = NowMs();
    return true;
}

bool ValidationSystem::ValidateDamage(
    int32_t reported_damage,
    const CharacterStats& attacker,
    const CharacterStats& defender,
    uint16_t skill_id)
{
    float base_atk = attacker.physic_attack;
    float base_def = defender.physic_defense;
    float skill_bonus = 0.0f;
    if (skill_id > 0) {
        skill_bonus = base_atk * 0.3f;
    }

    float raw = (base_atk + skill_bonus) - base_def * 0.5f;
    if (raw < 1.0f) raw = 1.0f;

    int32_t expected_min = static_cast<int32_t>(raw * 0.9f);
    int32_t expected_max = static_cast<int32_t>(raw * 1.1f);
    if (expected_min < 1) expected_min = 1;
    if (expected_max < expected_min) expected_max = expected_min + 5;

    int32_t tolerance_low = static_cast<int32_t>(expected_min * 0.7f);
    int32_t tolerance_high = static_cast<int32_t>(expected_max * 1.3f);

    if (reported_damage < tolerance_low || reported_damage > tolerance_high) {
        spdlog::warn("ValidateDamage: damage {} out of range [{}, {}] (atk={}, def={}, skill={})",
                     reported_damage, tolerance_low, tolerance_high,
                     base_atk, base_def, skill_id);
        return false;
    }

    return true;
}

bool ValidationSystem::ValidatePosition(
    const glm::vec3& pos,
    int32_t map_id)
{
    if (std::isnan(pos.x) || std::isnan(pos.y) || std::isnan(pos.z)) {
        spdlog::warn("ValidatePosition: NaN position for map {}", map_id);
        return false;
    }

    float bound_min = -500.0f;
    float bound_max = 500.0f;
    switch (map_id) {
        case 13: bound_min = -50.0f; bound_max = 50.0f; break;
        case 20: bound_min = -60.0f; bound_max = 60.0f; break;
        case 51: bound_min = -80.0f; bound_max = 80.0f; break;
        default: break;
    }

    if (pos.x < bound_min || pos.x > bound_max ||
        pos.z < bound_min || pos.z > bound_max) {
        spdlog::warn("ValidatePosition: position ({},{},{}) out of map {} bounds [{}, {}]",
                     pos.x, pos.y, pos.z, map_id, bound_min, bound_max);
        return false;
    }

    return true;
}

bool ValidationSystem::ValidateItemAction(
    uint32_t character_id,
    uint32_t item_id,
    uint16_t count,
    const std::string& action)
{
    if (action.empty() || count == 0) {
        spdlog::warn("ValidateItemAction: invalid action '{}' or count={} for char {} item {}",
                     action, count, character_id, item_id);
        return false;
    }

    (void)character_id;
    (void)item_id;
    return true;
}

#pragma once
#include <cstdint>
#include <entt/entt.hpp>

struct CombatState {
    bool is_casting = false;
    float cast_time = 0.0f;
    float current_cast_time = 0.0f;
    uint16_t casting_skill_id = 0;
    entt::entity target = entt::null;

    bool is_animation_locked = false;
    float lock_duration = 0.0f;
    float current_lock_time = 0.0f;

    void StartCast(uint16_t skill, float time, entt::entity tgt) {
        is_casting = true;
        cast_time = time;
        current_cast_time = 0.0f;
        casting_skill_id = skill;
        target = tgt;
        // Casting implies animation lock as well usually, or movement restriction
    }

    void StartAnimationLock(float time) {
        is_animation_locked = true;
        lock_duration = time;
        current_lock_time = 0.0f;
    }

    void CancelAction() {
        is_casting = false;
        current_cast_time = 0.0f;
        is_animation_locked = false;
        current_lock_time = 0.0f;
    }
};

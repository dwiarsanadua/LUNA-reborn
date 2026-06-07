#pragma once
#include <cstdint>
#include <string>

enum class AnimState : uint8_t {
    Idle = 0,
    Walk = 1,
    Run = 2,
    Attack = 3,
    Skill = 4,
    Hit = 5,
    Die = 6,
    Sit = 7,
    COUNT
};

struct AnimationState {
    AnimState current = AnimState::Idle;
    AnimState previous = AnimState::Idle;
    float state_time = 0.0f;
    float speed = 1.0f;
    bool loop = true;
    std::string anim_name;

    void Transition(AnimState new_state) {
        if (current != new_state) {
            previous = current;
            current = new_state;
            state_time = 0.0f;
        }
    }
};

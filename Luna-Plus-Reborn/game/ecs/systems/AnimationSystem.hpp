#pragma once
#include <entt/entt.hpp>

class AnimationSystem {
public:
    void Update(entt::registry& reg, float dt);
    void RequestState(entt::registry& reg, entt::entity entity, uint8_t new_state);
};

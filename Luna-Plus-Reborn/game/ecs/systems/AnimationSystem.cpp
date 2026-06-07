#include "AnimationSystem.hpp"
#include "../components/AnimationState.hpp"
#include "../components/Movement.hpp"
#include "../components/CharacterStats.hpp"
#include "../components/Tag.hpp"

void AnimationSystem::Update(entt::registry& reg, float dt) {
    auto view = reg.view<AnimationState>();
    for (auto entity : view) {
        auto& anim = view.get<AnimationState>(entity);
        anim.state_time += dt;

        // Auto-transition based on components
        if (reg.all_of<Movement>(entity)) {
            auto& mv = reg.get<Movement>(entity);
            if (mv.is_moving && anim.current == AnimState::Idle)
                anim.Transition(AnimState::Walk);
            else if (!mv.is_moving && anim.current == AnimState::Walk)
                anim.Transition(AnimState::Idle);
        }

        if (reg.all_of<CharacterStats>(entity)) {
            auto& stats = reg.get<CharacterStats>(entity);
            if (stats.hp <= 0 && anim.current != AnimState::Die)
                anim.Transition(AnimState::Die);
        }
    }
}

void AnimationSystem::RequestState(entt::registry& reg, entt::entity entity, uint8_t new_state) {
    if (!reg.valid(entity) || !reg.all_of<AnimationState>(entity)) return;
    auto& anim = reg.get<AnimationState>(entity);
    auto ns = static_cast<AnimState>(new_state);
    if (ns < AnimState::COUNT) {
        anim.Transition(ns);
    }
}

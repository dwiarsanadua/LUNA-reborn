#include "AISystem.hpp"
#include "../components/Transform.hpp"
#include "../components/Movement.hpp"
#include "../components/AIComponent.hpp"
#include "../components/CombatState.hpp"
#include "../components/CharacterStats.hpp"
#include "../components/Tag.hpp"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

// 3.2B: NavMesh placeholder for pathfinding integration
static glm::vec3 QueryNavMeshPath(const glm::vec3& start, const glm::vec3& end) {
    return end; 
}

void AISystem::Update(entt::registry& registry, float dt) {
    auto view = registry.view<AIComponent, Transform, CharacterStats>();
    for (auto entity : view) {
        auto& ai = view.get<AIComponent>(entity);
        auto& xform = view.get<Transform>(entity);

        switch (ai.state) {
            case AIComponent::Idle:  UpdateIdle(registry, entity, ai, dt); break;
            case AIComponent::Patrol: {
                auto& mv = registry.get<Movement>(entity);
                UpdatePatrol(registry, entity, ai, xform, mv, dt);
                break;
            }
            case AIComponent::Chase: {
                auto& mv = registry.get<Movement>(entity);
                UpdateChase(registry, entity, ai, xform, mv, dt);
                break;
            }
            case AIComponent::Attack: UpdateAttack(registry, entity, ai, xform, dt); break;
            case AIComponent::Return: {
                auto& mv = registry.get<Movement>(entity);
                UpdateReturn(registry, entity, ai, xform, mv, dt);
                break;
            }
            default: break;
        }
        ai.state_timer += dt;
    }
}

void AISystem::UpdateIdle(entt::registry& reg, entt::entity e, AIComponent& ai, float dt) {
    ScanForTargets(reg, e, ai, reg.get<Transform>(e));
    if (!ai.patrol_points.empty() && ai.state_timer > 3.0f) {
        TransitionState(ai, AIComponent::Patrol);
    }
}

void AISystem::UpdatePatrol(entt::registry& reg, entt::entity e,
                             AIComponent& ai, Transform& xform, Movement& mv, float dt) {
    ScanForTargets(reg, e, ai, xform);
    if (ai.patrol_points.empty()) { TransitionState(ai, AIComponent::Idle); return; }
    size_t idx = ai.current_patrol_index % ai.patrol_points.size();
    glm::vec3 target = ai.patrol_points[idx];
    float dist = glm::distance(xform.position, target);
    if (dist < 1.0f) {
        ai.current_patrol_index++;
        ai.patrol_wait_timer = 0.0f;
        TransitionState(ai, AIComponent::Idle);
    } else {
        mv.destination = target;
        mv.is_moving = true;
    }
}

void AISystem::UpdateChase(entt::registry& reg, entt::entity e,
                            AIComponent& ai, Transform& xform, Movement& mv, float dt) {
    if (!reg.valid(static_cast<entt::entity>(ai.aggro_target))) {
        TransitionState(ai, AIComponent::Return);
        return;
    }
    auto& target_xform = reg.get<Transform>(static_cast<entt::entity>(ai.aggro_target));
    float dist = glm::distance(xform.position, target_xform.position);
    if (dist > ai.chase_range) {
        TransitionState(ai, AIComponent::Return);
        return;
    }
    if (dist <= ai.attack_range) {
        mv.is_moving = false;
        TransitionState(ai, AIComponent::Attack);
        return;
    }
    
    // 3.2B: Use NavMesh for pathfinding
    mv.destination = QueryNavMeshPath(xform.position, target_xform.position);
    mv.current_speed = mv.speed > 0 ? mv.speed : 5.0f;
    mv.is_moving = true;
}

void AISystem::UpdateAttack(entt::registry& reg, entt::entity e,
                             AIComponent& ai, Transform& xform, float dt) {
    if (!reg.valid(static_cast<entt::entity>(ai.aggro_target))) {
        TransitionState(ai, AIComponent::Return);
        return;
    }
    
    auto& target_xform = reg.get<Transform>(static_cast<entt::entity>(ai.aggro_target));
    float dist = glm::distance(xform.position, target_xform.position);
    
    if (dist > ai.attack_range * 1.2f) {
        TransitionState(ai, AIComponent::Chase);
        return;
    }

    // Trigger CombatSystem if not already acting
    auto* combat = reg.try_get<CombatState>(e);
    if (!combat) {
        combat = &reg.emplace<CombatState>(e);
    }

    if (!combat->is_casting && !combat->is_animation_locked) {
        // Basic attack (id 0) with 0.5s cast time
        combat->StartCast(0, 0.5f, static_cast<entt::entity>(ai.aggro_target));
        spdlog::debug("Monster {} initiated attack on player {}", static_cast<uint32_t>(e), ai.aggro_target);
    }
}

void AISystem::UpdateReturn(entt::registry& reg, entt::entity e,
                             AIComponent& ai, Transform& xform, Movement& mv, float dt) {
    float dist = glm::distance(xform.position, ai.spawn_position);
    if (dist < 1.0f) {
        xform.position = ai.spawn_position;
        mv.is_moving = false;
        ai.ClearAggro();
        TransitionState(ai, AIComponent::Patrol);
    } else {
        mv.destination = ai.spawn_position;
        mv.is_moving = true;
    }
}

void AISystem::ScanForTargets(entt::registry& reg, entt::entity e,
                               AIComponent& ai, const Transform& xform) {
    auto view = reg.view<Transform, CharacterStats, TagPlayer>();
    for (auto target : view) {
        auto& t_xform = view.get<Transform>(target);
        float dist = glm::distance(xform.position, t_xform.position);
        if (dist < ai.aggro_range) {
            ai.aggro_target = static_cast<uint32_t>(target);
            ai.AddThreat(ai.aggro_target, 100);
            spdlog::info("Monster {} aggroed on player {} (dist: {:.1f})", static_cast<uint32_t>(e), ai.aggro_target, dist);
            TransitionState(ai, AIComponent::Chase);
            return;
        }
    }
}

void AISystem::TransitionState(AIComponent& ai, AIComponent::State new_state) {
    if (ai.state != new_state) {
        spdlog::debug("Monster AI state transition: {} -> {}", static_cast<int>(ai.state), static_cast<int>(new_state));
        ai.state = new_state;
        ai.state_timer = 0.0f;
    }
}

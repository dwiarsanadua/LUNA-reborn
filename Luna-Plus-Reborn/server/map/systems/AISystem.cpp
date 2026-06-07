#include "AISystem.h"
#include <spdlog/spdlog.h>
#include <random>
#include <glm/glm.hpp>

static std::mt19937 ai_rng(std::random_device{}());

AISystem::AISystem() = default;

void AISystem::Update(entt::registry& registry, float dt) {
    auto view = registry.view<AIComponent, CharacterStats>();
    for (auto entity : view) {
        auto& ai = view.get<AIComponent>(entity);
        auto& stats = view.get<CharacterStats>(entity);

        if (stats.hp <= 0) continue; // dead, handled by respawn system

        UpdateState(registry, entity, ai, dt);
    }
}

void AISystem::UpdateState(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt) {
    ai.state_timer += dt;

    switch (static_cast<int>(ai.state)) {
    case AIComponent::Idle:
        if (ai.state_timer > 3.0f + static_cast<float>(ai_rng() % 2000) / 1000.0f) {
            ai.state = AIComponent::Patrol;
            ai.state_timer = 0.0f;
        }
        FindNearestTarget(registry, entity, ai);
        break;

    case AIComponent::Patrol:
        Patrol(registry, entity, ai, dt);
        FindNearestTarget(registry, entity, ai);
        break;

    case AIComponent::Chase:
        Chase(registry, entity, ai, dt);
        break;

    case AIComponent::Attack:
        Attack(registry, entity, ai, dt);
        break;

    case AIComponent::Flee:
        Flee(registry, entity, ai, dt);
        break;

    case AIComponent::Return:
        ReturnToSpawn(registry, entity, ai, dt);
        break;
    }
}

void AISystem::FindNearestTarget(entt::registry& registry, entt::entity entity, AIComponent& ai) {
    auto* xform = registry.try_get<Transform>(entity);
    if (!xform) return;

    float nearest_dist = ai.aggro_range;
    entt::entity nearest = entt::null;

    auto players = registry.view<Transform, CharacterStats, TagPlayer>();
    for (auto player : players) {
        auto& px = players.get<Transform>(player);
        float dist = glm::distance(xform->position, px.position);
        if (dist < nearest_dist) {
            nearest_dist = dist;
            nearest = player;
        }
    }

    if (nearest != entt::null) {
        ai.aggro_target = static_cast<uint32_t>(nearest);
        ai.state = AIComponent::Chase;
        ai.state_timer = 0.0f;
        spdlog::debug("AI: entity {} aggro on player {}", static_cast<uint32_t>(entity),
                      static_cast<uint32_t>(nearest));
    }
}

void AISystem::Patrol(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt) {
    ai.state_timer += dt;
    if (ai.state_timer > 5.0f) {
        ai.state = AIComponent::Idle;
        ai.state_timer = 0.0f;
    }
}

void AISystem::Chase(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt) {
    if (ai.aggro_target == 0 || !registry.valid(static_cast<entt::entity>(ai.aggro_target))) {
        ai.state = AIComponent::Return;
        ai.ClearAggro();
        return;
    }

    auto* my_xform = registry.try_get<Transform>(entity);
    auto* target_xform = registry.try_get<Transform>(static_cast<entt::entity>(ai.aggro_target));
    if (!my_xform || !target_xform) {
        ai.state = AIComponent::Return;
        return;
    }

    float dist = glm::distance(my_xform->position, target_xform->position);

    if (dist <= ai.attack_range) {
        ai.state = AIComponent::Attack;
        ai.state_timer = 0.0f;
    } else if (dist > ai.chase_range) {
        ai.state = AIComponent::Return;
        ai.ClearAggro();
    }
}

void AISystem::Attack(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt) {
    if (ai.aggro_target == 0 || !registry.valid(static_cast<entt::entity>(ai.aggro_target))) {
        ai.state = AIComponent::Return;
        ai.ClearAggro();
        return;
    }

    ai.state_timer += dt;
    if (ai.state_timer >= 2.0f) {
        ai.state_timer = 0.0f;
        spdlog::debug("AI: entity {} attacks target {}", static_cast<uint32_t>(entity), ai.aggro_target);
    }

    // Check flee threshold
    auto* stats = registry.try_get<CharacterStats>(entity);
    if (stats && stats->max_hp > 0) {
        float hp_pct = static_cast<float>(stats->hp) / stats->max_hp;
        if (hp_pct < 0.2f) {
            ai.state = AIComponent::Flee;
        }
    }
}

void AISystem::Flee(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt) {
    ai.state_timer += dt;
    if (ai.state_timer > 5.0f) {
        ai.state = AIComponent::Return;
        ai.state_timer = 0.0f;
        ai.ClearAggro();
    }
}

void AISystem::ReturnToSpawn(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt) {
    auto* xform = registry.try_get<Transform>(entity);
    if (!xform) return;

    float dist = glm::distance(xform->position, ai.spawn_position);
    if (dist < 1.0f) {
        ai.state = AIComponent::Idle;
        ai.state_timer = 0.0f;
    }
}

void AISystem::HandleBossPhase(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt) {
    // Phase transitions at HP thresholds
}

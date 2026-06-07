// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <ecs/components/AIComponent.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/components/Tag.hpp>

class AISystem {
public:
    AISystem();
    void Update(entt::registry& registry, float dt);

private:
    void UpdateState(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt);
    void FindNearestTarget(entt::registry& registry, entt::entity entity, AIComponent& ai);
    void Patrol(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt);
    void Chase(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt);
    void Attack(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt);
    void Flee(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt);
    void ReturnToSpawn(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt);
    void HandleBossPhase(entt::registry& registry, entt::entity entity, AIComponent& ai, float dt);
};

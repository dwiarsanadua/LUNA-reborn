#pragma once
#include <entt/entt.hpp>
#include <string>
#include <unordered_map>
#include "../components/AIComponent.hpp"
#include "../components/Transform.hpp"
#include "../components/Movement.hpp"
#include "../components/CharacterStats.hpp"
#include "BossData.hpp"

class AISystem {
public:
    void Update(entt::registry& registry, float dt);
    void LoadBossDefinitions(const std::string& json_path);

private:
    std::unordered_map<uint32_t, BossDefinition> boss_definitions_;

    void UpdateIdle(entt::registry& reg, entt::entity e, AIComponent& ai, float dt);
    void UpdatePatrol(entt::registry& reg, entt::entity e,
                      AIComponent& ai, Transform& xform, Movement& mv, float dt);
    void UpdateChase(entt::registry& reg, entt::entity e,
                     AIComponent& ai, Transform& xform, Movement& mv, float dt);
    void UpdateAttack(entt::registry& reg, entt::entity e,
                      AIComponent& ai, Transform& xform, float dt);
    void UpdateReturn(entt::registry& reg, entt::entity e,
                      AIComponent& ai, Transform& xform, Movement& mv, float dt);
    void UpdateFlee(entt::registry& reg, entt::entity e,
                    AIComponent& ai, Transform& xform, Movement& mv, float dt);
    void ScanForTargets(entt::registry& reg, entt::entity e,
                        AIComponent& ai, const Transform& xform);
    void HandleBossAI(entt::registry& reg, entt::entity e,
                      AIComponent& ai, CharacterStats& stats, float dt);
    void TransitionState(AIComponent& ai, AIComponent::State new_state);
    void RequestHelp(entt::registry& reg, entt::entity e, AIComponent& ai, uint32_t target_id);
};

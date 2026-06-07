// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <random>
#include <entt/entt.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/CombatState.hpp>

struct DamageResult {
    int32_t damage = 0;
    bool is_miss = false;
    bool is_critical = false;
    bool is_block = false;
};

class CombatSystem {
public:
    CombatSystem();

    DamageResult CalculateDamage(const CharacterStats& attacker,
                                  const CharacterStats& defender,
                                  float skill_add_damage = 0,
                                  uint8_t add_type = 1,
                                  float rate_add_value = 0,
                                  float plus_add_value = 0);

    void HandleAttack(entt::registry& registry, entt::entity attacker,
                      entt::entity target, uint16_t skill_id = 0);
    void ApplyDamage(entt::registry& registry, entt::entity target, int32_t damage);
    bool IsInRange(glm::vec3 a, glm::vec3 b, float range);
    void Update(entt::registry& registry, float dt);

private:
    std::mt19937 rng_;
};

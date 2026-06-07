#pragma once
#include <entt/entt.hpp>
#include "../components/CharacterStats.hpp"
#include "../components/Transform.hpp"

struct DamageResult {
    int32_t damage;
    bool is_critical;
    bool is_miss;
    bool is_block;
    bool is_dead;
};

class CombatSystem {
public:
    DamageResult CalculateDamage(const CharacterStats& attacker,
                                 const CharacterStats& defender,
                                 float skill_add_damage = 0.0f,
                                 uint8_t add_type = 1,
                                 float rate_add_value = 0.0f,
                                 float plus_add_value = 0.0f);
    void HandleAttack(entt::registry& registry,
                      entt::entity attacker, entt::entity target,
                      uint16_t skill_id = 0);
    void ApplyDamage(entt::registry& registry,
                     entt::entity target, int32_t damage);
    bool IsInRange(const Transform& a, const Transform& b, float range);
    
    void Update(entt::registry& registry, float dt);
};

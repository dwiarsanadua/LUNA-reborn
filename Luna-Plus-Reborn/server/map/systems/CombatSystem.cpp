#include "CombatSystem.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>

// Reference CharacterStats for damage calculation
// In production, this would include the full CharacterStats header

CombatSystem::CombatSystem() : rng_(std::random_device{}()) {}

DamageResult CombatSystem::CalculateDamage(const CharacterStats& attacker,
                                            const CharacterStats& defender,
                                            float skill_add_damage,
                                            uint8_t add_type,
                                            float rate_add_value,
                                            float plus_add_value) {
    DamageResult result{};

    // 1. Accuracy / Evasion
    float acc = 85.0f + attacker.accuracy - defender.evasion;
    if (acc < static_cast<float>(std::uniform_int_distribution<int>(1, 100)(rng_))) {
        result.is_miss = true;
        return result;
    }

    // 2. Base Damage
    float damage = 1.0f;
    float attack = attacker.physic_attack;
    float defense = defender.physic_defense;

    if (add_type == 1) {
        damage = (attack - defense) * ((1000.0f + skill_add_damage + attacker.strength) / 1000.0f);
    } else if (add_type == 2) {
        damage = (attack - defense) * ((1000.0f + skill_add_damage + attacker.physic_attack) / 1000.0f);
    } else {
        damage = attack - defense;
    }

    damage = (damage * (1.0f + (rate_add_value / 100.0f))) + plus_add_value;

    // 3. Level Penalty
    if (attacker.level < defender.level) {
        if (damage < 1.0f) damage = 1.0f;
        damage *= (1.0f + (static_cast<float>(attacker.level) - static_cast<float>(defender.level)) * 0.015f);
    }

    // 4. Block Check
    if (defender.shield_defense > 0 &&
        defender.block_rate > std::uniform_int_distribution<int>(0, 99)(rng_)) {
        result.is_block = true;
        damage = (damage * (0.6f - (defender.constitution / 4000.0f))) - defender.shield_defense;
    }
    // 5. Critical Check
    else if (attacker.critical_rate >= 100.0f ||
             attacker.critical_rate >= std::uniform_int_distribution<int>(1, 100)(rng_)) {
        result.is_critical = true;
        damage = (damage * 1.5f) * (1.0f + (attacker.critical_damage_rate / 100.0f)) + attacker.critical_damage_plus;
    }

    if (damage < 1.0f) damage = 1.0f;
    result.damage = static_cast<int32_t>(damage);
    return result;
}

void CombatSystem::HandleAttack(entt::registry& registry, entt::entity attacker,
                                 entt::entity target, uint16_t skill_id) {
    if (!registry.valid(attacker) || !registry.valid(target)) return;

    auto* atk_stats = registry.try_get<CharacterStats>(attacker);
    auto* def_stats = registry.try_get<CharacterStats>(target);
    if (!atk_stats || !def_stats) return;

    auto result = CalculateDamage(*atk_stats, *def_stats, 0, 1, 0, 0);
    if (!result.is_miss) {
        ApplyDamage(registry, target, result.damage);
        if (result.is_critical) {
            spdlog::debug("Critical hit: {} damage", result.damage);
        }
    }
}

void CombatSystem::ApplyDamage(entt::registry& registry, entt::entity target, int32_t damage) {
    auto* stats = registry.try_get<CharacterStats>(target);
    if (!stats) return;
    stats->hp = std::max(0, stats->hp - damage);

    // Check for death
    if (stats->hp <= 0) {
        spdlog::debug("Entity {} died", static_cast<uint32_t>(target));
        // XP loss, respawn, PK drop handled here
    }
}

bool CombatSystem::IsInRange(glm::vec3 a, glm::vec3 b, float range) {
    return glm::distance(a, b) <= range;
}

void CombatSystem::Update(entt::registry& registry, float dt) {
    // Process combat state timers, status effects, etc.
    auto view = registry.view<CombatState>();
    for (auto entity : view) {
        auto& combat = view.get<CombatState>(entity);

        if (combat.is_casting) {
            combat.current_cast_time += dt;
            if (combat.current_cast_time >= combat.cast_time) {
                HandleAttack(registry, entity, combat.target, combat.casting_skill_id);
                combat.is_casting = false;
                combat.current_cast_time = 0.0f;
                combat.StartAnimationLock(0.5f);
            }
        }

        if (combat.is_animation_locked) {
            combat.current_lock_time += dt;
            if (combat.current_lock_time >= combat.lock_duration) {
                combat.is_animation_locked = false;
                combat.current_lock_time = 0.0f;
            }
        }
    }
}

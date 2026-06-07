#include "CombatSystem.hpp"
#include "../components/Transform.hpp"
#include "../components/Tag.hpp"
#include "../components/CombatState.hpp"
#include <glm/glm.hpp>
#include <random>
#include <spdlog/spdlog.h>

static std::mt19937 rng(std::random_device{}());

DamageResult CombatSystem::CalculateDamage(const CharacterStats& attacker,
                                           const CharacterStats& defender,
                                           float skill_add_damage,
                                           uint8_t add_type,
                                           float rate_add_value,
                                           float plus_add_value) {
    DamageResult result{};
    
    // 1. Accuracy / Evasion Check (Legacy: ACC = 85.f + Accuracy - Avoid)
    float acc = 85.0f + attacker.accuracy - defender.evasion;
    if (acc < static_cast<float>(std::uniform_int_distribution<int>(1, 100)(rng))) {
        result.is_miss = true;
        return result;
    }

    // 2. Base Damage Calculation
    float damage = 1.0f;
    float attack = attacker.physic_attack;
    float defense = defender.physic_defense;
    
    if (add_type == 1) { // Default physical
        damage = (attack - defense) * ((1000.0f + skill_add_damage + attacker.strength) / 1000.0f);
    } else if (add_type == 2) { // Weapon dependent
        damage = (attack - defense) * ((1000.0f + skill_add_damage + attacker.physic_attack) / 1000.0f);
    } else {
        damage = attack - defense; // Fallback
    }

    // Skill rate & plus additives
    damage = (damage * (1.0f + (rate_add_value / 100.0f))) + plus_add_value;

    // 3. Level Penalty (Legacy: damage * ( 1 + ( fAttackerLevel - fTargetLevel ) * 0.015f ))
    if (attacker.level < defender.level) {
        if (damage < 1.0f) damage = 1.0f;
        damage = damage * (1.0f + (static_cast<float>(attacker.level) - static_cast<float>(defender.level)) * 0.015f);
    }

    // 4. Block Check (Legacy: if( 0 < Shield && Block > float(rand() % 100) ))
    if (defender.shield_defense > 0 && defender.block_rate > std::uniform_int_distribution<int>(0, 99)(rng)) {
        result.is_block = true;
        // Legacy: damage = ( damage * ( 0.6f - ( vitality / 4000 ) ) ) - Shield;
        damage = (damage * (0.6f - (defender.constitution / 4000.0f))) - defender.shield_defense;
    } 
    // 5. Critical Check (Legacy: if (CriticalRate >= 100.f || CriticalRate >= ( ( rand() % 100 ) + 1 )))
    else if (attacker.critical_rate >= 100.0f || attacker.critical_rate >= std::uniform_int_distribution<int>(1, 100)(rng)) {
        result.is_critical = true;
        // Legacy: damage = ( damage * 1.5f ) * ( 1 + ( CriticalDamageRate / 100 ) ) + CriticalDamagePlus;
        damage = (damage * 1.5f) * (1.0f + (attacker.critical_damage_rate / 100.0f)) + attacker.critical_damage_plus;
    }

    // 6. Minimum Damage clamp
    if (damage < 1.0f) damage = 1.0f;

    result.damage = static_cast<int32_t>(damage);
    return result;
}

void CombatSystem::HandleAttack(entt::registry& registry,
                                entt::entity attacker, entt::entity target,
                                uint16_t skill_id) {
    if (!registry.valid(attacker) || !registry.valid(target)) return;
    auto& atk_stats = registry.get<CharacterStats>(attacker);
    auto& def_stats = registry.get<CharacterStats>(target);
    
    // In a full implementation, we'd query skill_id from DB to get add_damage, type, etc.
    auto result = CalculateDamage(atk_stats, def_stats, 0, 1, 0, 0);
    
    if (!result.is_miss) {
        ApplyDamage(registry, target, result.damage);
    }
}

void CombatSystem::ApplyDamage(entt::registry& registry,
                                entt::entity target, int32_t damage) {
    auto& stats = registry.get<CharacterStats>(target);
    stats.hp = std::max(0, stats.hp - damage);
}

bool CombatSystem::IsInRange(const Transform& a, const Transform& b, float range) {
    return glm::distance(a.position, b.position) <= range;
}

void CombatSystem::Update(entt::registry& registry, float dt) {
    auto view = registry.view<CombatState>();
    for (auto entity : view) {
        auto& combat = view.get<CombatState>(entity);
        
        if (combat.is_casting) {
            combat.current_cast_time += dt;
            if (combat.current_cast_time >= combat.cast_time) {
                // Cast finished, trigger effect and start animation lock
                HandleAttack(registry, entity, combat.target, combat.casting_skill_id);
                combat.is_casting = false;
                combat.current_cast_time = 0.0f;
                // E.g., 0.5s animation lock after casting completes
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

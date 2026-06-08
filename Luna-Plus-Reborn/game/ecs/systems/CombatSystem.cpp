#include "CombatSystem.hpp"
#include "../components/Transform.hpp"
#include "../components/Tag.hpp"
#include "../components/CombatState.hpp"
#include <glm/glm.hpp>
#include <random>
#include <cmath>
#include <algorithm>
#include <spdlog/spdlog.h>

static std::mt19937 rng(std::random_device{}());

Element CombatSystem::GetAttackElement(const CharacterStats& stats) {
    float attrs[7] = { stats.attr_none, stats.attr_earth, stats.attr_water,
                       stats.attr_divine, stats.attr_wind, stats.attr_fire, stats.attr_dark };
    int max_idx = 0;
    for (int i = 1; i < 7; i++) {
        if (attrs[i] > attrs[max_idx]) max_idx = i;
    }
    return static_cast<Element>(max_idx);
}

float CombatSystem::GetElementAdvantage(Element atk_elem, Element def_elem) {
    static const uint8_t advantage[7][7] = {
        { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 1 },
        { 0, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0 },
        { 0, 0, 0, 1, 0, 0, 0 },
    };
    if (atk_elem > Element::Dark || def_elem > Element::Dark) return 1.0f;
    uint8_t a = static_cast<uint8_t>(atk_elem);
    uint8_t d = static_cast<uint8_t>(def_elem);
    if (advantage[a][d]) return 1.30f;
    if (advantage[d][a]) return 0.70f;
    return 1.0f;
}

DamageResult CombatSystem::CalculateDamage(const CharacterStats& attacker,
                                           const CharacterStats& defender,
                                           float skill_add_damage,
                                           uint8_t add_type,
                                           float rate_add_value,
                                           float plus_add_value) {
    DamageResult result{};

    // 1. Miss Check: 5% base, reduced by DEX
    float miss_chance = std::max(0.01f, 0.05f - attacker.dexterity * 0.002f);
    if (std::uniform_real_distribution<float>(0, 1)(rng) < miss_chance) {
        result.is_miss = true;
        return result;
    }

    // 2. Base Damage: (ATK * 2) - DEF
    float attack = attacker.physic_attack;
    float defense = defender.physic_defense;
    float damage = (attack * 2.0f) - defense;
    damage = std::max(1.0f, damage);

    // 3. Skill modifiers
    if (add_type == 1) {
        damage = damage * ((1000.0f + skill_add_damage + attacker.strength) / 1000.0f);
    } else if (add_type == 2) {
        damage = damage * ((1000.0f + skill_add_damage + attacker.physic_attack) / 1000.0f);
    }
    damage = (damage * (1.0f + (rate_add_value / 100.0f))) + plus_add_value;

    // 4. Element Advantage
    Element atk_elem = GetAttackElement(attacker);
    Element def_elem = GetAttackElement(defender);
    float elem_mult = GetElementAdvantage(atk_elem, def_elem);
    damage *= elem_mult;

    // 5. Level Difference Penalty: ±5% per level, cap 50%
    int32_t level_diff = static_cast<int32_t>(attacker.level) - static_cast<int32_t>(defender.level);
    float level_mod = 1.0f + std::clamp(static_cast<float>(level_diff) * 0.05f, -0.50f, 0.50f);
    damage *= level_mod;

    // 6. Block Check
    if (defender.shield_defense > 0 && defender.block_rate > std::uniform_int_distribution<int>(0, 99)(rng)) {
        result.is_block = true;
        damage = (damage * (0.6f - (defender.constitution / 4000.0f))) - defender.shield_defense;
    }

    // 7. Critical: base 5% + (DEX/100), crit damage = 150% + (STR/200)
    float crit_rate = 5.0f + attacker.dexterity / 100.0f;
    float crit_dmg = 1.50f + attacker.strength / 200.0f;
    if (!result.is_block && (attacker.critical_rate >= 100.0f ||
                             crit_rate >= std::uniform_real_distribution<float>(0, 100)(rng))) {
        result.is_critical = true;
        damage *= crit_dmg;
    }

    // 8. Damage Variance: ±10%
    float variance = 1.0f + std::uniform_real_distribution<float>(-0.10f, 0.10f)(rng);
    damage *= variance;

    // 9. Minimum Damage clamp
    damage = std::max(1.0f, damage);

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

MonsterBaseStats CombatSystem::ScaleMonsterStats(const MonsterBaseStats& base, uint16_t base_level, uint16_t target_level) {
    if (base_level == 0) base_level = 1;
    if (target_level == 0) target_level = 1;
    float ratio = static_cast<float>(target_level) / static_cast<float>(base_level);
    MonsterBaseStats scaled;
    scaled.hp = static_cast<int32_t>(base.hp * ratio * (1.0f + (ratio - 1.0f) * 0.2f));
    scaled.mp = static_cast<int32_t>(base.mp * ratio);
    scaled.attack = static_cast<int32_t>(base.attack * ratio);
    scaled.defense = static_cast<int32_t>(base.defense * ratio);
    scaled.magic_attack = static_cast<int32_t>(base.magic_attack * ratio);
    scaled.magic_defense = static_cast<int32_t>(base.magic_defense * ratio);
    scaled.exp = static_cast<uint32_t>(base.exp * ratio * ratio);
    scaled.gold_min = static_cast<uint32_t>(base.gold_min * ratio);
    scaled.gold_max = static_cast<uint32_t>(base.gold_max * ratio);
    return scaled;
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

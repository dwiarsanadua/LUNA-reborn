#include "CombatSystem.hpp"
#include "ComboSystem.hpp"
#include "../components/Transform.hpp"
#include "../components/Tag.hpp"
#include "../components/CombatState.hpp"
#include "../components/AIComponent.hpp"
#include "../components/ComboComponent.hpp"
#include <glm/glm.hpp>
#include <random>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <spdlog/spdlog.h>

static std::mt19937 rng(std::random_device{}());

// ---- ThreatTable implementation ----

void ThreatTable::AddThreat(uint32_t entity_id, int32_t amount) {
    for (auto& entry : entries) {
        if (entry.entity_id == entity_id) {
            entry.hate_amount += amount;
            return;
        }
    }
    entries.push_back({entity_id, amount});
}

uint32_t ThreatTable::GetTopThreat() const {
    if (entries.empty()) return 0;
    uint32_t top_id = 0;
    int32_t top_amount = -1;
    for (auto& entry : entries) {
        if (entry.hate_amount > top_amount) {
            top_amount = entry.hate_amount;
            top_id = entry.entity_id;
        }
    }
    return top_id;
}

void ThreatTable::Clear() {
    entries.clear();
}

// ---- Element system (DISABLED for Old accuracy) ----
/*
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
*/

// ---- Accuracy / Evasion system (Agent C) ----

float CombatSystem::GetClassAccuracyFactor(uint8_t class_id) {
    // OLD: Fighter=14, Rogue=18, Ranger=11, Mage=15
    static const float FACTOR[] = {0, 14, 18, 11, 15};
    if (class_id >= 1 && class_id <= 4) return FACTOR[class_id];
    return 14;
}

float CombatSystem::CalcAccuracy(const CharacterStats& s) {
    float dex_rate = CalcDexRate(s);
    float acc = (dex_rate * 0.6f
                + (s.base_dexterity - 30.0f) / 1000.0f
                + s.level * GetClassAccuracyFactor(s.class_id) / 15000.0f) * 100.0f;
    acc = acc * (1.0f + s.accuracy_pct / 100.0f) + s.accuracy_plus;
    return acc;
}

float CombatSystem::CalcEvasion(const CharacterStats& s) {
    float dex_rate = CalcDexRate(s);
    float eva = (dex_rate * 0.6f
                + (s.base_dexterity - 30.0f) / 1000.0f
                + s.level * GetClassAccuracyFactor(s.class_id) / 15000.0f) * 100.0f;
    eva = eva * (1.0f + s.evasion_pct / 100.0f) + s.evasion_plus;
    return eva;
}

bool CombatSystem::IsHit(const CharacterStats& attacker, const CharacterStats& defender) {
    float accuracy = CalcAccuracy(attacker);
    float evasion = CalcEvasion(defender);
    float hit_chance = 85.0f + accuracy - evasion;
    hit_chance = std::clamp(hit_chance, 10.0f, 99.0f);
    float roll = std::uniform_real_distribution<float>(0, 100)(rng);
    return roll < hit_chance;
}

// ---- PK System (FEEL-07) ----
bool CombatSystem::IsEnemy(const CharacterStats& a, const CharacterStats& b) {
    return IsEnemy(a, b, CombatContext::Normal);
}

bool CombatSystem::IsEnemy(const CharacterStats& a, const CharacterStats& b, CombatContext ctx) {
    if (&a == &b) return false;

    if (ctx == CombatContext::Siege) {
        if (a.party_id > 0 && a.party_id == b.party_id) return false;
        if (a.guild_id > 0 && a.guild_id == b.guild_id) return false;
        if (a.guild_war_id > 0 && a.guild_war_id == b.guild_id) return true;
        if (b.guild_war_id > 0 && b.guild_war_id == a.guild_id) return true;
        return true;
    }

    if (a.party_id > 0 && a.party_id == b.party_id) return false;
    if (a.pk_mode) return true;
    if (b.pk_mode) return true;
    if (a.guild_war_id > 0 && a.guild_war_id == b.guild_id) return true;
    if (b.guild_war_id > 0 && b.guild_war_id == a.guild_id) return true;

    return false;
}

// ---- Crit & Block helper formulas (Agent B) ----

float CombatSystem::CalcDexRate(const CharacterStats& s) {
    float dex_gain = std::max(0.0f, static_cast<float>(s.dexterity - s.base_dexterity));
    float level_factor = std::max(1.0f, static_cast<float>(s.level - 1) * 5.0f);
    return dex_gain / level_factor;
}

float CombatSystem::CalcIntRate(const CharacterStats& s) {
    float int_gain = std::max(0.0f, static_cast<float>(s.intelligence - s.base_intelligence));
    float level_factor = std::max(1.0f, static_cast<float>(s.level - 1) * 5.0f);
    return int_gain / level_factor;
}

float CombatSystem::CalcWisRate(const CharacterStats& s) {
    float wis_gain = std::max(0.0f, static_cast<float>(s.wisdom - s.base_wisdom));
    float level_factor = std::max(1.0f, static_cast<float>(s.level - 1) * 5.0f);
    return wis_gain / level_factor;
}

float CombatSystem::CalcCritRate(const CharacterStats& s) {
    float dex_rate = CalcDexRate(s);
    float base_contrib = (s.base_dexterity - 25.0f) / 5.0f;
    float buff_contrib = s.crit_rate_buff + s.critical_damage_plus / 45.0f;
    return 45.0f * dex_rate + base_contrib + buff_contrib;
}

float CombatSystem::CalcMagicCritRate(const CharacterStats& s) {
    float int_rate = CalcIntRate(s);
    float wis_rate = CalcWisRate(s);
    return 10.0f * int_rate + 20.0f * wis_rate + (s.base_intelligence - 25.0f) / 5.0f
           + s.crit_rate_buff + s.critical_damage_plus / 45.0f;
}

float CombatSystem::GetClassBlockBonus(uint8_t class_id) {
    static const float BONUS[] = {0, 15, 10, 5, 9};
    if (class_id >= 1 && class_id <= 4) return BONUS[class_id];
    return 0;
}

float CombatSystem::CalcBlockRate(const CharacterStats& s) {
    return s.dexterity / 27.0f + GetClassBlockBonus(s.class_id) + s.block_rate_buff;
}

// ---- HP/MP stat calculations (Old: CalcMaxLife/CalcMaxMana from CharacterCalcManager) ----

int32_t CombatSystem::CalcMaxLife(const CharacterStats& s) {
    float hp_factor = 1.0f;
    switch (s.class_id) {
        case 1: hp_factor = 1.3f; break;
        case 2: hp_factor = 0.9f; break;
        case 3: hp_factor = 1.0f; break;
        case 4: hp_factor = 0.7f; break;
    }
    float vita = s.constitution;
    float base_vita = static_cast<float>(s.base_vitality);
    float total = (static_cast<float>(s.level - 1) * 10.0f)
                + ((vita - base_vita + 4.0f) * 10.0f)
                + (hp_factor * static_cast<float>(s.level) * 7.0f);
    return std::max(1, static_cast<int32_t>(total));
}

int32_t CombatSystem::CalcMaxMana(const CharacterStats& s) {
    float mp_factor = 1.0f;
    switch (s.class_id) {
        case 1: mp_factor = 0.6f; break;
        case 2: mp_factor = 0.9f; break;
        case 3: mp_factor = 0.8f; break;
        case 4: mp_factor = 1.4f; break;
    }
    float wis = s.wisdom;
    float base_wis = static_cast<float>(s.base_wisdom);
    float total = (static_cast<float>(s.level - 1) * 10.0f)
                + ((wis - base_wis + 4.0f) * 10.0f)
                + (mp_factor * static_cast<float>(s.level) * 7.0f);
    return std::max(1, static_cast<int32_t>(total));
}

void CombatSystem::ApplyStatPoint(CharacterStats& s, uint8_t stat_type, int32_t points) {
    switch (stat_type) {
        case STAT_STR: s.strength += static_cast<float>(points); break;
        case STAT_WIS: s.wisdom += static_cast<float>(points); break;
        case STAT_DEX: s.dexterity += static_cast<float>(points); break;
        case STAT_VIT: s.constitution += static_cast<float>(points); break;
        case STAT_INT: s.intelligence += static_cast<float>(points); break;
        default: break;
    }
}

// ---- Old-accurate stat calculations ----

float CombatSystem::CalcPhysicAttack(const CharacterStats& s) {
    float base = (s.weapon_attack + s.level) * (1.0f + s.strength * 0.001f)
                 + (s.strength - s.base_strength);
    float enchant = 1.0f + (s.enchant_level * s.enchant_level) / 400.0f;
    return base * enchant * (1.0f + s.physic_attack_pct / 100.0f) + s.physic_attack_plus;
}

float CombatSystem::CalcPhysicDefense(const CharacterStats& s) {
    float base = (s.armor_defense + s.level) * (1.0f + s.constitution / 3000.0f)
                 + (s.constitution - 40.0f) / 5.0f;
    float enchant = 1.0f + s.enchant_level / 200.0f;
    return base * enchant * (1.0f + s.physic_defense_pct / 100.0f) + s.physic_defense_plus;
}

float CombatSystem::CalcMagicAttack(const CharacterStats& s) {
    float base = (s.weapon_attack + s.level) * (1.0f + s.intelligence * 0.001f)
                 + (s.intelligence - s.base_intelligence);
    float enchant = 1.0f + (s.enchant_level * s.enchant_level) / 400.0f;
    return base * enchant * (1.0f + s.magic_attack_pct / 100.0f) + s.magic_attack_plus;
}

float CombatSystem::CalcMagicDefense(const CharacterStats& s) {
    float base = (s.armor_defense + s.level) * (1.0f + s.wisdom / 3000.0f)
                 + (s.base_wisdom > 0 ? (s.wisdom - s.base_wisdom) : s.wisdom) / 5.0f;
    float enchant = 1.0f + s.enchant_level / 200.0f;
    return base * enchant * (1.0f + s.magic_defense_pct / 100.0f) + s.magic_defense_plus;
}

float CombatSystem::CalcHealAmount(const CharacterStats& healer, float skill_factor) {
    // Old: heal = ((Wisdom × 11 + Int × 4 + Level × 20) × SkillFactor) / 800 + 100
    float heal = (healer.wisdom * 11.0f + healer.intelligence * 4.0f + healer.level * 20.0f)
                 * skill_factor / 800.0f + 100.0f;
    return std::max(1.0f, heal);
}

int32_t CombatSystem::CalcRequiredExp(uint16_t level) {
    return static_cast<int32_t>(level) * level * 100 + level * 50;
}

float CombatSystem::CalcAttackSpeed(const CharacterStats& s) {
    return s.physic_skill_speed_rate;
}

float CombatSystem::CalcCastingSpeed(const CharacterStats& s) {
    return s.magic_skill_speed_rate;
}

float CombatSystem::CalcMoveSpeed(const CharacterStats& s) {
    return s.move_speed;
}

// ---- CalculateDamage (refactored with Old formulas) ----

DamageResult CombatSystem::CalculateDamage(const CharacterStats& attacker,
                                           const CharacterStats& defender,
                                           float skill_add_damage,
                                           uint8_t add_type,
                                           float rate_add_value,
                                           float plus_add_value,
                                           float combo_multiplier,
                                           CombatContext context) {
    DamageResult result{};

    // 1. Miss Check: Accuracy/Evasion system (Agent C)
    if (!IsHit(attacker, defender)) {
        result.is_miss = true;
        spdlog::debug("DAMAGE: MISS (acc={:.1f} eva={:.1f} hit_chance={:.1f}%)",
                      CalcAccuracy(attacker), CalcEvasion(defender),
                      85.0f + CalcAccuracy(attacker) - CalcEvasion(defender));
        return result;
    }

    // 2. Base Damage — Old formula
    float atk, def;
    if (add_type == 1) { // STR-based
        atk = (attacker.weapon_attack + attacker.level)
            * (1.0f + attacker.strength * 0.001f)
            + (attacker.strength - attacker.base_strength);
        atk *= 1.0f + (attacker.enchant_level * attacker.enchant_level) / 400.0f;
    } else { // Weapon-based
        atk = (attacker.weapon_attack + attacker.level)
            * (1.0f + attacker.weapon_attack * 0.001f)
            + (attacker.strength - attacker.base_strength);
        atk *= 1.0f + (attacker.enchant_level * attacker.enchant_level) / 400.0f;
    }
    atk *= 1.0f + attacker.physic_attack_pct / 100.0f;
    atk += attacker.physic_attack_plus;

    def = (defender.armor_defense + defender.level)
        * (1.0f + defender.constitution / 3000.0f)
        + (defender.constitution - 40.0f) / 5.0f;
    def *= 1.0f + defender.enchant_level / 200.0f;

    float damage = atk - def;
    damage = std::max(1.0f, damage);
    spdlog::debug("DAMAGE: base atk={:.1f} def={:.1f} raw={:.1f}", atk, def, damage);

    // 3. Skill modifiers (Old: add_type 1 = STR based, add_type 2 = ATK based)
    if (add_type == 1) {
        damage = damage * ((1000.0f + skill_add_damage + attacker.strength) / 1000.0f);
    } else if (add_type == 2) {
        damage = damage * ((1000.0f + skill_add_damage + attacker.weapon_attack) / 1000.0f);
    }
    damage = (damage * (1.0f + (rate_add_value / 100.0f))) + plus_add_value;
    spdlog::debug("DAMAGE: after skill add_type={} rate={:.1f} plus={:.1f} -> {:.1f}",
                  add_type, rate_add_value, plus_add_value, damage);

    // 4. Element Advantage — DISABLED (Old tidak punya elemental system)
    // Element atk_elem = GetAttackElement(attacker);
    // Element def_elem = GetAttackElement(defender);
    // float elem_mult = GetElementAdvantage(atk_elem, def_elem);
    // damage *= elem_mult;
    // spdlog::debug("DAMAGE: element atk={} def={} mult={:.2f} -> {:.1f}",
    //               static_cast<int>(atk_elem), static_cast<int>(def_elem), elem_mult, damage);

    // 5. Level Difference Penalty (Old: -1.5% per level ONLY if attacker < defender)
    if (attacker.level < defender.level) {
        int32_t level_diff = defender.level - attacker.level;
        float penalty = 1.0f - level_diff * 0.015f;
        penalty = std::max(penalty, 0.5f);
        damage *= penalty;
        spdlog::debug("DAMAGE: level penalty diff={} mult={:.2f} -> {:.1f}", level_diff, penalty, damage);
    }
    // If attacker.level >= defender.level: NO MODIFICATION

    // 6. Block Check: Old formula
    float block_chance = CalcBlockRate(defender);
    block_chance = std::clamp(block_chance, 0.0f, 100.0f);
    if (defender.shield_defense > 0 &&
        block_chance / 100.0f > std::uniform_real_distribution<float>(0, 1)(rng)) {
        result.is_blocked = true;
        damage = (damage * (0.6f - (defender.constitution / 4000.0f))) - defender.shield_defense;
        spdlog::debug("DAMAGE: BLOCKED block_chance={:.4f} -> {:.1f}", block_chance, damage);
    }

    // 7. Critical: Old formula
    float crit_rate = CalcCritRate(attacker);
    crit_rate = std::clamp(crit_rate, 0.0f, 100.0f);
    float crit_dmg = 1.50f * (1.0f + attacker.critical_damage_rate / 100.0f) + attacker.critical_damage_plus;
    if (!result.is_blocked && (attacker.critical_rate >= 100.0f ||
                             crit_rate / 100.0f >= std::uniform_real_distribution<float>(0, 1)(rng))) {
        result.is_critical = true;
        damage *= crit_dmg;
        spdlog::debug("DAMAGE: CRITICAL rate={:.4f} mult={:.2f} -> {:.1f}", crit_rate, crit_dmg, damage);
    }

    // 8. PvP / GT / Siege damage reduction (Old: PvP 35%, GT 10%)
    if (context == CombatContext::PvP) {
        damage *= 0.35f;
    } else if (context == CombatContext::GuildTournament) {
        damage *= 0.10f;
    } else if (context == CombatContext::Siege) {
        damage *= 0.50f;
    }

    // 9. Damage Variance: ±10%
    float variance = 1.0f + std::uniform_real_distribution<float>(-0.10f, 0.10f)(rng);
    damage *= variance;
    spdlog::debug("DAMAGE: variance={:.4f} -> {:.1f}", variance, damage);

    // 9. Combo Multiplier (Old: Hero combo chain bonus)
    damage *= combo_multiplier;
    spdlog::debug("DAMAGE: combo_mult={:.2f} -> {:.1f}", combo_multiplier, damage);

    // 10. Minimum Damage clamp
    damage = std::max(1.0f, damage);

    result.damage = static_cast<int32_t>(damage);
    result.threat_generated = result.damage;
    spdlog::debug("DAMAGE: FINAL={} threat={}", result.damage, result.threat_generated);
    return result;
}

void CombatSystem::HandleAttack(entt::registry& registry,
                                entt::entity attacker, entt::entity target,
                                uint16_t skill_id,
                                CombatContext context) {
    if (!registry.valid(attacker) || !registry.valid(target)) return;
    auto& atk_stats = registry.get<CharacterStats>(attacker);
    auto& def_stats = registry.get<CharacterStats>(target);

    float combo_mult = 1.0f;
    if (combo_system_) {
        combo_mult = combo_system_->GetComboMultiplier(registry, attacker);
    }

    // In a full implementation, we'd query skill_id from DB to get add_damage, type, etc.
    auto result = CalculateDamage(atk_stats, def_stats, 0, 1, 0, 0, combo_mult, context);

    if (!result.is_miss) {
        ApplyDamage(registry, target, result.damage);

        if (combo_system_) {
            combo_system_->RegisterHit(registry, attacker, target);
        }

        // Generate threat from damage dealt
        auto* ai = registry.try_get<AIComponent>(target);
        if (ai) {
            HandleThreatOnDamage(registry, target, attacker, result.threat_generated);
        }
    } else {
        if (combo_system_) {
            combo_system_->ResetCombo(registry, attacker);
        }
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
    auto* stats = registry.try_get<CharacterStats>(target);
    if (!stats) return;

    // Apply reflect damage
    if (stats->reflect_damage > 0) {
        // Reflected back to attacker — handled externally
    }

    // Apply absorb
    if (stats->absorb_damage > 0) {
        int32_t absorbed = static_cast<int32_t>(damage * stats->absorb_damage / 100.0f);
        damage -= absorbed;
    }

    // Apply damage reduction
    if (stats->reduce_damage > 0) {
        damage = static_cast<int32_t>(damage * (1.0f - stats->reduce_damage / 100.0f));
    }

    // Apply add damage modifier
    damage += static_cast<int32_t>(stats->add_damage);

    stats->hp = std::max(0, stats->hp - damage);
}

bool CombatSystem::IsInRange(const Transform& a, const Transform& b, float range) {
    return glm::distance(a.position, b.position) <= range;
}

// ---- Threat system (Old: CHero hate/threat from Hero.cpp) ----

void CombatSystem::AddThreat(entt::registry& registry, entt::entity monster, entt::entity attacker, int32_t amount) {
    auto* ai = registry.try_get<AIComponent>(monster);
    if (!ai || !registry.valid(attacker)) return;

    uint32_t attacker_id = static_cast<uint32_t>(attacker);
    ai->AddThreat(attacker_id, amount);
    spdlog::debug("THREAT: monster={} attacker={} +{} total={}",
                  static_cast<uint32_t>(monster), attacker_id, amount, ai->threat);
}

uint32_t CombatSystem::GetTopThreat(entt::registry& registry, entt::entity monster) {
    auto* ai = registry.try_get<AIComponent>(monster);
    if (!ai) return 0;
    return ai->GetHighestThreat();
}

void CombatSystem::ResetThreat(entt::registry& registry, entt::entity monster) {
    auto* ai = registry.try_get<AIComponent>(monster);
    if (!ai) return;
    ai->ClearAggro();
    spdlog::debug("THREAT: monster={} aggro cleared", static_cast<uint32_t>(monster));
}

void CombatSystem::HandleThreatOnDamage(entt::registry& registry, entt::entity monster, entt::entity attacker, int32_t damage) {
    // 1:1 threat from damage (Old behavior)
    AddThreat(registry, monster, attacker, damage);
}

void CombatSystem::HandleThreatOnHeal(entt::registry& registry, entt::entity monster, entt::entity healer, int32_t heal_amount) {
    // 50% threat from healing (Old behavior)
    int32_t threat = heal_amount / 2;
    AddThreat(registry, monster, healer, threat);
}

// ---- Combat input system (Old: GameIn.cpp) ----

void CombatSystem::HandleCombatInput(entt::registry& registry, entt::entity player, const CombatInput& input) {
    if (!registry.valid(player)) return;

    auto* combat = registry.try_get<CombatState>(player);
    if (!combat) {
        combat = &registry.emplace<CombatState>(player);
    }

    if (combat->is_casting || combat->is_animation_locked) return;

    if (input.target_self) {
        // Self-target skill or heal
        if (input.skill_pressed && input.pending_skill_id > 0) {
            combat->StartCast(input.pending_skill_id, 0.4f, player);
        }
        return;
    }

    if (input.attack_pressed && registry.valid(input.target_entity)) {
        // Basic attack
        combat->StartCast(0, 0.5f, input.target_entity);
        spdlog::debug("COMBAT_INPUT: player {} basic attack on target {}",
                      static_cast<uint32_t>(player), static_cast<uint32_t>(input.target_entity));
    }

    if (input.skill_pressed && input.pending_skill_id > 0 && registry.valid(input.target_entity)) {
        // Skill attack
        combat->StartCast(input.pending_skill_id, 0.4f, input.target_entity);
        spdlog::debug("COMBAT_INPUT: player {} skill {} on target {}",
                      static_cast<uint32_t>(player), input.pending_skill_id,
                      static_cast<uint32_t>(input.target_entity));
    }
}

void CombatSystem::HandleAutoAttack(entt::registry& registry, entt::entity player, entt::entity target, float dt) {
    if (!registry.valid(player) || !registry.valid(target)) return;

    auto* combat = registry.try_get<CombatState>(player);
    if (!combat) {
        combat = &registry.emplace<CombatState>(player);
    }

    if (combat->is_casting || combat->is_animation_locked) return;

    // Auto-attack every 1.2s (Old behavior)
    static std::unordered_map<uint32_t, float> auto_attack_timers;
    uint32_t player_id = static_cast<uint32_t>(player);
    auto& timer = auto_attack_timers[player_id];
    timer += dt;
    if (timer >= 1.2f) {
        timer = 0.0f;
        combat->StartCast(0, 0.5f, target);
        spdlog::debug("AUTO_ATTACK: player {} auto-attacks target {}",
                      player_id, static_cast<uint32_t>(target));
    }
}

void CombatSystem::Update(entt::registry& registry, float dt) {
    auto view = registry.view<CombatState>();
    for (auto entity : view) {
        auto& combat = view.get<CombatState>(entity);

        if (combat.is_casting) {
            combat.current_cast_time += dt;
            if (combat.current_cast_time >= combat.cast_time) {
                // Cast finished, trigger effect and start animation lock
                auto* stats = registry.try_get<CharacterStats>(entity);
                auto* target_stats = registry.try_get<CharacterStats>(combat.target);

                if (stats && target_stats) {
                    float combo_mult = 1.0f;
                    if (combo_system_) {
                        combo_mult = combo_system_->GetComboMultiplier(registry, entity);
                    }

                    // Check auto-attack (skill_id == 0) vs skill
                    float add_damage = 0.0f;
                    if (combat.casting_skill_id > 0) {
                        // In full implementation, query skill book for params
                        add_damage = 10.0f;
                    }
                    auto result = CalculateDamage(*stats, *target_stats, add_damage, 1, 0, 0, combo_mult);

                    if (!result.is_miss) {
                        ApplyDamage(registry, combat.target, result.damage);

                        if (combo_system_) {
                            combo_system_->RegisterHit(registry, entity, combat.target);
                        }

                        // Generate threat
                        auto* ai = registry.try_get<AIComponent>(combat.target);
                        if (ai) {
                            HandleThreatOnDamage(registry, combat.target, entity, result.threat_generated);
                        }
                    } else {
                        if (combo_system_) {
                            combo_system_->ResetCombo(registry, entity);
                        }
                    }
                }

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

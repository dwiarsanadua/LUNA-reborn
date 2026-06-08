#include "CombatSystem.h"
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>

CombatSystem::CombatSystem() : rng_(std::random_device{}()) {}

DamageResult CombatSystem::CalculateDamage(const CharacterStats& attacker,
                                            const CharacterStats& defender,
                                            float skill_add_damage,
                                            uint8_t add_type,
                                            float rate_add_value,
                                            float plus_add_value) {
    DamageResult result{};

    float acc = 85.0f + attacker.accuracy - defender.evasion;
    if (acc < static_cast<float>(std::uniform_int_distribution<int>(1, 100)(rng_))) {
        result.is_miss = true;
        return result;
    }

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
    ApplyElementalModifiers(damage, attacker, defender);

    if (attacker.level < defender.level) {
        if (damage < 1.0f) damage = 1.0f;
        damage *= (1.0f + (static_cast<float>(attacker.level) - static_cast<float>(defender.level)) * 0.015f);
    }

    if (defender.shield_defense > 0 &&
        defender.block_rate > std::uniform_int_distribution<int>(0, 99)(rng_)) {
        result.is_block = true;
        damage = (damage * (0.6f - (defender.constitution / 4000.0f))) - defender.shield_defense;
    } else if (attacker.critical_rate >= 100.0f ||
               attacker.critical_rate >= std::uniform_int_distribution<int>(1, 100)(rng_)) {
        result.is_critical = true;
        damage = (damage * 1.5f) * (1.0f + (attacker.critical_damage_rate / 100.0f)) + attacker.critical_damage_plus;
    }

    if (damage < 1.0f) damage = 1.0f;
    result.damage = static_cast<int32_t>(damage);
    return result;
}

void CombatSystem::ApplyElementalModifiers(float& damage, const CharacterStats& attacker,
                                            const CharacterStats& defender) {
    float element_atk[7] = {attacker.attr_none, attacker.attr_earth, attacker.attr_water,
                            attacker.attr_divine, attacker.attr_wind, attacker.attr_fire, attacker.attr_dark};
    float element_def[7] = {defender.attr_none, defender.attr_earth, defender.attr_water,
                            defender.attr_divine, defender.attr_wind, defender.attr_fire, defender.attr_dark};
    float net = 0;
    for (int i = 0; i < 7; i++) net += element_atk[i] - element_def[i];
    if (net != 0) damage *= (1.0f + net / 100.0f);
}

void CombatSystem::HandleAttack(entt::registry& registry, entt::entity attacker,
                                 entt::entity target, uint16_t skill_id) {
    if (!registry.valid(attacker) || !registry.valid(target)) return;
    auto* atk_stats = registry.try_get<CharacterStats>(attacker);
    auto* def_stats = registry.try_get<CharacterStats>(target);
    if (!atk_stats || !def_stats) return;

    float add_damage = 0, rate_add = 0, plus_add = 0;
    uint8_t add_type = 1;

    if (skill_id > 0) {
        auto* skill_book = registry.try_get<SkillBook>(attacker);
        if (skill_book) {
            auto it = skill_book->skills.find(skill_id);
            if (it != skill_book->skills.end()) {
                add_damage = it->second.add_damage;
                add_type = it->second.add_type;
                rate_add = it->second.rate_add_value;
                plus_add = it->second.plus_add_value;
            }
        }
    }

    auto result = CalculateDamage(*atk_stats, *def_stats, add_damage, add_type, rate_add, plus_add);
    if (!result.is_miss) {
        ApplyDamage(registry, target, result.damage);

        // Apply skill effects (stun/poison/burn)
        if (skill_id > 0 && result.damage > 0) {
            auto* skill_book = registry.try_get<SkillBook>(attacker);
            if (skill_book) {
                auto it = skill_book->skills.find(skill_id);
                if (it != skill_book->skills.end()) {
                    auto& sk = it->second;
                    if (sk.stun_duration > 0)
                        ApplyStatusEffect(registry, target, StatusEffectType::STUN, sk.stun_duration);
                    if (sk.poison_damage > 0)
                        ApplyStatusEffect(registry, target, StatusEffectType::POISON, sk.poison_duration, sk.poison_damage);
                    if (sk.slow_amount > 0)
                        ApplyStatusEffect(registry, target, StatusEffectType::SLOW, sk.slow_duration, 0, sk.slow_amount);
                }
            }
        }

        if (result.is_critical)
            spdlog::debug("Critical hit: {} damage", result.damage);
    }
}

void CombatSystem::ApplyDamage(entt::registry& registry, entt::entity target, int32_t damage) {
    auto* stats = registry.try_get<CharacterStats>(target);
    if (!stats) return;

    // Apply reflect damage
    if (stats->reflect_damage > 0) {
        // Would reflect back to attacker
    }

    // Apply absorb
    if (stats->absorb_damage > 0) {
        int32_t absorbed = static_cast<int32_t>(damage * stats->absorb_damage / 100.0f);
        damage -= absorbed;
        // absorbed goes to MP or HP depending on implementation
    }

    // Apply reduction
    if (stats->reduce_damage > 0) {
        damage = static_cast<int32_t>(damage * (1.0f - stats->reduce_damage / 100.0f));
    }

    // Apply add damage modifier
    damage += static_cast<int32_t>(stats->add_damage);

    stats->hp = std::max(0, stats->hp - damage);

    if (stats->hp <= 0) {
        spdlog::debug("Entity {} died", static_cast<uint32_t>(target));
    }
}

void CombatSystem::HandleDeath(entt::registry& registry, entt::entity target, entt::entity killer) {
    auto* target_stats = registry.try_get<CharacterStats>(target);
    if (!target_stats) return;

    bool is_player = registry.all_of<TagPlayer>(target);
    bool is_monster = registry.all_of<TagMonster>(target);
    bool killer_is_player = registry.valid(killer) && registry.all_of<TagPlayer>(killer);

    if (is_player && killer_is_player) {
        // PK death: drop items
        spdlog::info("PK death: player {} killed by player {}", 
                     static_cast<uint32_t>(target), static_cast<uint32_t>(killer));
        // Drop chance: 5% per equipment slot
        // In production, iterate equipment and drop random items
    }

    if (is_player && !killer_is_player) {
        // Normal death: XP loss, respawn at bind point
        uint64_t exp_loss = static_cast<uint64_t>(target_stats->exp * 0.10f);
        target_stats->exp = (target_stats->exp > exp_loss) ? target_stats->exp - exp_loss : 0;
        spdlog::info("Player {} died, lost {} exp", static_cast<uint32_t>(target), exp_loss);
    }

    if (is_monster && killer_is_player) {
        // Monster death: calculate XP for killer
        auto* killer_stats = registry.try_get<CharacterStats>(killer);
        if (killer_stats) {
            uint64_t exp_gain = CalculateExpGain(*killer_stats, *target_stats);

            // Check for party
            auto* party = registry.try_get<PartyInfo>(killer);
            if (party && !party->members.empty()) {
                ShareExp(registry, killer, target, party->members);
            } else {
                killer_stats->exp += exp_gain;
                spdlog::debug("Player gained {} exp from kill", exp_gain);
            }

            // Check level up
            while (killer_stats->exp >= killer_stats->exp_next_level) {
                killer_stats->exp -= killer_stats->exp_next_level;
                killer_stats->level++;
                killer_stats->exp_next_level = static_cast<uint64_t>(killer_stats->level * 100);
                killer_stats->max_hp += 20;
                killer_stats->hp = killer_stats->max_hp;
                killer_stats->physic_attack += 2;
                killer_stats->physic_defense += 1;
                spdlog::info("Player {} leveled up to level {}!",
                             static_cast<uint32_t>(killer), killer_stats->level);
            }
        }

        // Roll loot (delegated to ItemSystem)
    }

    // Reset HP to 1 for respawn
    target_stats->hp = 1;
}

uint64_t CombatSystem::CalculateExpGain(const CharacterStats& killer, const CharacterStats& victim) {
    uint64_t base = static_cast<uint64_t>(victim.level * 50);
    float level_ratio = static_cast<float>(victim.level) / std::max(1, killer.level);

    // Bonus for higher-level mobs (up to 2x)
    float bonus = (level_ratio > 1.0f) ? std::min(level_ratio, 2.0f) : 0.5f;
    // Level penalty: killer > victim gets reduced XP
    if (killer.level > victim.level + 5) bonus *= 0.5f;
    if (killer.level > victim.level + 10) bonus = 0;

    return static_cast<uint64_t>(base * bonus * killer.bonus_exp_rate);
}

void CombatSystem::ShareExp(entt::registry& registry, entt::entity killer, entt::entity victim,
                              std::vector<entt::entity>& party_members) {
    auto* vic_stats = registry.try_get<CharacterStats>(victim);
    if (!vic_stats) return;

    uint64_t total_exp = CalculateExpGain(registry.get<CharacterStats>(killer), *vic_stats);
    // Party bonus: +20%
    total_exp = static_cast<uint64_t>(total_exp * 1.2f);

    int party_size = static_cast<int>(party_members.size()) + 1;
    uint64_t per_member = total_exp / party_size;

    auto* killer_stats = registry.try_get<CharacterStats>(killer);
    if (killer_stats) killer_stats->exp += per_member;

    for (auto member : party_members) {
        auto* mstats = registry.try_get<CharacterStats>(member);
        if (mstats) {
            mstats->exp += per_member;
            spdlog::debug("Party member {} gained {} exp", static_cast<uint32_t>(member), per_member);
        }
    }
}

void CombatSystem::HandleSkill(entt::registry& registry, entt::entity caster,
                                entt::entity target, const SkillEntry& skill) {
    if (!registry.valid(caster)) return;
    auto* caster_stats = registry.try_get<CharacterStats>(caster);

    // Heal skill
    if (skill.heal_amount > 0 && registry.valid(target)) {
        auto* target_stats = registry.try_get<CharacterStats>(target);
        if (target_stats) {
            int32_t heal = skill.heal_amount + (caster_stats ? static_cast<int32_t>(caster_stats->intelligence * 2) : 0);
            target_stats->hp = std::min(target_stats->max_hp, target_stats->hp + heal);
            spdlog::debug("Heal: +{} HP on entity {}", heal, static_cast<uint32_t>(target));
        }
    }

    // Buff/Debuff
    if (skill.buff_duration > 0) {
        // Apply buff effects (stat modifiers)
    }

    if (skill.stun_duration > 0) {
        ApplyStatusEffect(registry, target, StatusEffectType::STUN, skill.stun_duration);
    }

    if (skill.poison_damage > 0) {
        ApplyStatusEffect(registry, target, StatusEffectType::POISON, skill.poison_duration, skill.poison_damage);
    }

    if (skill.slow_amount > 0) {
        ApplyStatusEffect(registry, target, StatusEffectType::SLOW, skill.slow_duration, 0, skill.slow_amount);
    }

    // Damage skill
    if (skill.add_damage > 0 || skill.rate_add_value > 0) {
        HandleAttack(registry, caster, target, skill.skill_id);
    }
}

void CombatSystem::ApplyStatusEffect(entt::registry& registry, entt::entity target,
                                       StatusEffectType type, float duration,
                                       int32_t tick_damage, float slow_amount) {
    if (!registry.valid(target)) return;

    auto& effects = registry.get_or_emplace<StatusEffectComponent>(target);
    // Don't stack same effect
    for (auto& e : effects.effects) {
        if (e.type == type) {
            e.duration = std::max(e.duration, duration);
            e.tick_damage = tick_damage;
            e.slow_amount = slow_amount;
            e.elapsed = 0;
            return;
        }
    }
    effects.effects.push_back({type, duration, 0, tick_damage, 1.0f, 0, slow_amount});
    spdlog::debug("StatusEffect: applied {} on entity {} for {:.1f}s",
                  static_cast<int>(type), static_cast<uint32_t>(target), duration);
}

void CombatSystem::RemoveStatusEffect(entt::registry& registry, entt::entity target, StatusEffectType type) {
    auto* effects = registry.try_get<StatusEffectComponent>(target);
    if (!effects) return;
    effects->effects.erase(
        std::remove_if(effects->effects.begin(), effects->effects.end(),
            [type](const StatusEffect& e) { return e.type == type; }),
        effects->effects.end());
}

void CombatSystem::ProcessStatusEffectTick(entt::registry& registry, entt::entity entity,
                                             StatusEffect& effect, float dt) {
    effect.tick_timer += dt;
    if (effect.tick_timer >= effect.tick_interval) {
        effect.tick_timer = 0;
        if (effect.tick_damage > 0) {
            auto* stats = registry.try_get<CharacterStats>(entity);
            if (stats) {
                stats->hp = std::max(0, stats->hp - effect.tick_damage);
                spdlog::debug("DOT tick: {} damage on entity {}", effect.tick_damage,
                              static_cast<uint32_t>(entity));
            }
        }
    }
}

bool CombatSystem::IsInRange(glm::vec3 a, glm::vec3 b, float range) {
    return glm::distance(a, b) <= range;
}

void CombatSystem::Update(entt::registry& registry, float dt) {
    // Process combat state timers
    auto view = registry.view<CombatState>();
    for (auto entity : view) {
        auto& combat = view.get<CombatState>(entity);
        if (combat.is_casting) {
            combat.current_cast_time += dt;
            if (combat.current_cast_time >= combat.cast_time) {
                auto* skill_book = registry.try_get<SkillBook>(entity);
                if (skill_book) {
                    auto it = skill_book->skills.find(combat.casting_skill_id);
                    if (it != skill_book->skills.end()) {
                        HandleSkill(registry, entity, combat.target, it->second);
                    }
                } else {
                    HandleAttack(registry, entity, combat.target, combat.casting_skill_id);
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

    // Process status effects
    auto status_view = registry.view<StatusEffectComponent, CharacterStats>();
    for (auto entity : status_view) {
        auto& effects = status_view.get<StatusEffectComponent>(entity);
        auto& stats = status_view.get<CharacterStats>(entity);

        for (auto it = effects.effects.begin(); it != effects.effects.end(); ) {
            it->elapsed += dt;

            switch (it->type) {
            case StatusEffectType::STUN:
                // Stun: skip casting and attacking
                if (auto* combat = registry.try_get<CombatState>(entity))
                    combat->CancelAction();
                break;
            case StatusEffectType::SLOW:
                stats.move_speed = 5.0f * (1.0f - it->slow_amount);
                break;
            case StatusEffectType::POISON:
            case StatusEffectType::BLEED:
            case StatusEffectType::BURN:
                ProcessStatusEffectTick(registry, entity, *it, dt);
                break;
            default:
                break;
            }

            if (it->elapsed >= it->duration) {
                if (it->type == StatusEffectType::SLOW)
                    stats.move_speed = 5.0f; // restore
                it = effects.effects.erase(it);
            } else {
                ++it;
            }
        }

        // Death check
        if (stats.hp <= 0) {
            HandleDeath(registry, entity, entt::null);
        }
    }
}

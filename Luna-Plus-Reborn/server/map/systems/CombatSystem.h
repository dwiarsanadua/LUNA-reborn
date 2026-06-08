// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <random>
#include <entt/entt.hpp>
#include <ecs/components/CharacterStats.hpp>
#include <ecs/components/CombatState.hpp>
#include <ecs/components/Transform.hpp>
#include <ecs/components/Tag.hpp>
#include <ecs/components/SkillBook.hpp>

struct DamageResult {
    int32_t damage = 0;
    bool is_miss = false;
    bool is_critical = false;
    bool is_block = false;
    bool is_dead = false;
};

enum class StatusEffectType : uint8_t {
    NONE,
    STUN,
    POISON,
    SLOW,
    KNOCKBACK,
    BURN,
    FREEZE,
    BLIND,
    SILENCE,
    BLEED
};

struct StatusEffect {
    StatusEffectType type = StatusEffectType::NONE;
    float duration = 0.0f;
    float elapsed = 0.0f;
    int32_t tick_damage = 0;   // for DOT effects
    float tick_interval = 1.0f;
    float tick_timer = 0.0f;
    float slow_amount = 0.0f;  // for SLOW
    entt::entity source = entt::null;
};

struct StatusEffectComponent {
    std::vector<StatusEffect> effects;
};

struct ExperienceInfo {
    uint64_t exp_gained = 0;
    bool leveled_up = false;
};

struct PartyInfo {
    entt::entity leader = entt::null;
    std::vector<entt::entity> members;
    bool share_exp = true;
    bool share_loot = false;
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

    void HandleDeath(entt::registry& registry, entt::entity target, entt::entity killer);

    void ApplyStatusEffect(entt::registry& registry, entt::entity target,
                           StatusEffectType type, float duration, int32_t tick_damage = 0,
                           float slow_amount = 0.0f);

    void RemoveStatusEffect(entt::registry& registry, entt::entity target, StatusEffectType type);

    uint64_t CalculateExpGain(const CharacterStats& killer, const CharacterStats& victim);
    void ShareExp(entt::registry& registry, entt::entity killer, entt::entity victim,
                  std::vector<entt::entity>& party_members);

    void HandleSkill(entt::registry& registry, entt::entity caster,
                     entt::entity target, const SkillEntry& skill);

    bool IsInRange(glm::vec3 a, glm::vec3 b, float range);
    void Update(entt::registry& registry, float dt);

private:
    std::mt19937 rng_;
    void ProcessStatusEffectTick(entt::registry& registry, entt::entity entity,
                                  StatusEffect& effect, float dt);
    void ApplyElementalModifiers(float& damage, const CharacterStats& attacker,
                                  const CharacterStats& defender);
};

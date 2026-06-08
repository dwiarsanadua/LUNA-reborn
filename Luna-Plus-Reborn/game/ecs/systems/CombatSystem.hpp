#pragma once
#include <entt/entt.hpp>
#include "../components/CharacterStats.hpp"
#include "../components/Transform.hpp"

enum class Element : uint8_t {
    None = 0, Earth = 1, Water = 2, Divine = 3, Wind = 4, Fire = 5, Dark = 6
};

struct DamageResult {
    int32_t damage;
    bool is_critical;
    bool is_miss;
    bool is_block;
    bool is_dead;
};

struct MonsterBaseStats {
    int32_t hp, mp, attack, defense, magic_attack, magic_defense;
    uint32_t exp;
    uint32_t gold_min, gold_max;
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

    // Element system (Agent E)
    static Element GetAttackElement(const CharacterStats& stats);
    static float GetElementAdvantage(Element atk_elem, Element def_elem);

    // Scale monster base stats by level (Agent E)
    static MonsterBaseStats ScaleMonsterStats(const MonsterBaseStats& base, uint16_t base_level, uint16_t target_level);
    
    void Update(entt::registry& registry, float dt);
};

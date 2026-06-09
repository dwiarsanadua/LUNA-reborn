#pragma once
#include <entt/entt.hpp>
#include "../components/CharacterStats.hpp"
#include "../components/Transform.hpp"
#include "../components/AIComponent.hpp"
#include <vector>
#include <utility>
#include <cstdint>

class ComboSystem;

enum class Element : uint8_t {
    None = 0, Earth = 1, Water = 2, Divine = 3, Wind = 4, Fire = 5, Dark = 6
};

enum class CombatContext {
    Normal,
    PvP,
    GuildTournament,
    Siege,
};

struct DamageResult {
    int32_t damage = 0;
    bool is_miss = false;
    bool is_critical = false;
    bool is_blocked = false;
    int32_t threat_generated = 0;
    int32_t hp_remaining = 0;
};

struct MonsterBaseStats {
    int32_t hp, mp, attack, defense, magic_attack, magic_defense;
    uint32_t exp;
    uint32_t gold_min, gold_max;
};

struct ThreatEntry {
    uint32_t entity_id;
    int32_t hate_amount;
};

struct ThreatTable {
    std::vector<ThreatEntry> entries;

    void AddThreat(uint32_t entity_id, int32_t amount);
    uint32_t GetTopThreat() const;
    void Clear();
};

class CombatSystem {
public:
    static DamageResult CalculateDamage(const CharacterStats& attacker,
                                        const CharacterStats& defender,
                                        float skill_add_damage = 0.0f,
                                        uint8_t add_type = 1,
                                        float rate_add_value = 0.0f,
                                        float plus_add_value = 0.0f,
                                        float combo_multiplier = 1.0f,
                                        CombatContext context = CombatContext::Normal);
    void HandleAttack(entt::registry& registry,
                      entt::entity attacker, entt::entity target,
                      uint16_t skill_id = 0,
                      CombatContext context = CombatContext::Normal);
    void ApplyDamage(entt::registry& registry,
                     entt::entity target, int32_t damage);
    bool IsInRange(const Transform& a, const Transform& b, float range);

    // Element system (Agent E) — DISABLED for Old accuracy
    // [[deprecated("Element system tidak ada di Old. Disabled untuk akurasi.")]]
    // static Element GetAttackElement(const CharacterStats& stats);
    // static float GetElementAdvantage(Element atk_elem, Element def_elem);

    // Scale monster base stats by level (Agent E)
    static MonsterBaseStats ScaleMonsterStats(const MonsterBaseStats& base, uint16_t base_level, uint16_t target_level);

    // Threat system (Old: CHero hate/threat)
    void AddThreat(entt::registry& registry, entt::entity monster, entt::entity attacker, int32_t amount);
    uint32_t GetTopThreat(entt::registry& registry, entt::entity monster);
    void ResetThreat(entt::registry& registry, entt::entity monster);
    void HandleThreatOnDamage(entt::registry& registry, entt::entity monster, entt::entity attacker, int32_t damage);
    void HandleThreatOnHeal(entt::registry& registry, entt::entity monster, entt::entity healer, int32_t heal_amount);

    // Old-accurate stat calculations
    static float CalcPhysicAttack(const CharacterStats& s);
    static float CalcPhysicDefense(const CharacterStats& s);
    static float CalcMagicAttack(const CharacterStats& s);
    static float CalcMagicDefense(const CharacterStats& s);
    static float CalcHealAmount(const CharacterStats& healer, float skill_factor);

    // Accuracy / Evasion (Agent C)
    static float CalcAccuracy(const CharacterStats& s);
    static float CalcEvasion(const CharacterStats& s);
    static float GetClassAccuracyFactor(uint8_t class_id);
    static bool IsHit(const CharacterStats& attacker, const CharacterStats& defender);

    // Combat input handling (Old: GameIn.cpp)
    struct CombatInput {
        bool attack_pressed = false;
        bool skill_pressed = false;
        uint16_t pending_skill_id = 0;
        entt::entity target_entity = entt::null;
        bool target_self = false;
    };
    void HandleCombatInput(entt::registry& registry, entt::entity player, const CombatInput& input);
    void HandleAutoAttack(entt::registry& registry, entt::entity player, entt::entity target, float dt);

    // Crit & Block formulas (Agent B)
    static float CalcDexRate(const CharacterStats& s);
    static float CalcIntRate(const CharacterStats& s);
    static float CalcWisRate(const CharacterStats& s);
    static float CalcCritRate(const CharacterStats& s);
    static float CalcMagicCritRate(const CharacterStats& s);
    static float CalcBlockRate(const CharacterStats& s);
    static float GetClassBlockBonus(uint8_t class_id);

    void SetComboSystem(ComboSystem* cs) { combo_system_ = cs; }
    void Update(entt::registry& registry, float dt);

private:
    ComboSystem* combo_system_ = nullptr;
};

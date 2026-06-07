#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

struct CharacterStats {
    int str = 10, dex = 10, vit = 10, intel = 10;
    int hp = 500, max_hp = 500;
    int mp = 100, max_mp = 100;
    int attack = 50, defense = 20;
    int magic_attack = 20, magic_defense = 15;
    float move_speed = 1.0f;
    float attack_speed = 1.0f;
    float crit_rate = 0.05f;
    float crit_damage = 1.5f;
    float dodge_rate = 0.03f;
    float block_rate = 0.02f;
    int hp_regen = 0;
    int mp_regen = 0;
};

enum class BuffType {
    StatModifier,     // Modify stats (flat or %)
    DamageOverTime,   // DOT: deal damage per tick
    HealOverTime,     // HOT: heal per tick
    Stun,             // Cannot act
    Silence,          // Cannot cast skills
    Root,             // Cannot move
    Invulnerable,     // Immune to damage
    Reflect,          // Reflect damage back
    Taunt,            // Force target to attack caster
    Custom            // Custom callback
};

enum class BuffStackRule {
    None,       // Cannot stack, refresh duration
    Full,       // Stack fully (multiple instances)
    Intensity,  // Stack by increasing/decreasing intensity
    Exclusive   // Only one instance at a time
};

struct BuffInstance {
    int id = 0;
    std::string name;
    BuffType type = BuffType::StatModifier;
    float duration = 0, elapsed = 0;
    bool permanent = false;
    bool expired = false;
    
    // Stat modifications
    struct StatMod {
        std::string stat_name; // "str", "attack", "move_speed", etc.
        float value = 0;
        bool percent = false;  // true = +X%, false = +X flat
    };
    std::vector<StatMod> mods;
    
    // DOT/HOT
    int tick_damage = 0;
    int tick_heal = 0;
    float tick_interval = 1.0f;
    float tick_timer = 0;
    
    // Stacking
    BuffStackRule stack_rule = BuffStackRule::None;
    int max_stacks = 1;
    int current_stacks = 1;
    float intensity_per_stack = 1.0f;
    
    // Custom callback
    std::function<void(CharacterStats& stats, float dt)> on_tick;
    std::function<void(CharacterStats& stats)> on_apply;
    std::function<void(CharacterStats& stats)> on_expire;
    
    // Visual
    uint32_t icon_color = 0xffffffff;
    std::string icon_name;
};

class BuffSystem {
public:
    void Update(float dt, CharacterStats& stats);
    
    int Apply(const BuffInstance& buff, CharacterStats& stats);
    void Remove(int buff_id);
    void RemoveByType(BuffType type);
    void RemoveByName(const std::string& name);
    void RemoveAll();
    void ClearExpired();
    
    bool HasBuff(int buff_id) const;
    bool HasBuffType(BuffType type) const;
    bool HasBuffName(const std::string& name) const;
    int GetStackCount(int buff_id) const;
    int GetBuffCount() const { return (int)active_buffs_.size(); }
    
    const std::vector<BuffInstance>& GetActiveBuffs() const { return active_buffs_; }
    
    // Helper factories
    static BuffInstance MakeStatBuff(const std::string& name, float duration,
                                      const std::string& stat, float value, bool percent = false);
    static BuffInstance MakeDOT(const std::string& name, float duration, int tick_dmg, float interval = 1.0f);
    static BuffInstance MakeHOT(const std::string& name, float duration, int tick_heal, float interval = 1.0f);
    static BuffInstance MakeStun(float duration);
    static BuffInstance MakeSilence(float duration);
    
private:
    int next_id_ = 1;
    std::vector<BuffInstance> active_buffs_;
    void ApplyMods(const BuffInstance& buff, CharacterStats& stats, bool apply);
};

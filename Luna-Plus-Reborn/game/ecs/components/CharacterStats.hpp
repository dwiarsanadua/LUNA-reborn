#pragma once
#include <cstdint>
#include <glm/glm.hpp>

struct CharacterStats {
    uint16_t level = 1;
    uint64_t exp = 0;
    uint64_t exp_next_level = 100;

    // Core Stats (Base)
    float strength = 10.0f;
    float dexterity = 10.0f;
    float constitution = 10.0f; // mapped from Vitality
    float intelligence = 10.0f;
    float wisdom = 10.0f;

    // Combat Stats (Calculated)
    float physic_attack = 10.0f;
    float physic_defense = 5.0f;
    float magic_attack = 5.0f;
    float magic_defense = 3.0f;
    float accuracy = 80.0f;
    float evasion = 10.0f;
    float critical_rate = 5.0f;
    float critical_damage_rate = 0.0f; // percentage boost (Legacy: CriticalDamageRate)
    float critical_damage_plus = 0.0f; // flat boost (Legacy: CriticalDamagePlus)
    float crit_rate_buff = 0.0f;       // Percent buffs untuk crit
    float attack_range = 3.0f;
    float move_speed = 5.0f;
    float block_rate = 0.0f;
    float block_rate_buff = 0.0f;
    float shield_defense = 0.0f;
    float cool_time_reduction = 0.0f;
    float casting_protect = 0.0f;

    // HP / MP and Recovery
    int32_t max_hp = 100;
    int32_t max_mp = 50;
    int32_t hp = 100;
    int32_t mp = 50;
    float hp_regen = 0.5f;
    float mp_regen = 0.3f;
    float hp_recover_rate = 1.0f;
    float mp_recover_rate = 1.0f;

    // Modifiers (Add/Reduce)
    float add_damage = 0.0f;
    float reduce_damage = 0.0f;
    float reflect_damage = 0.0f;
    float absorb_damage = 0.0f;
    float damage_to_hp_pct = 0.0f;
    float damage_to_mp_pct = 0.0f;
    float bonus_exp_rate = 1.0f;
    float bonus_gold_rate = 1.0f;
    float bonus_drop_rate = 1.0f;

    // Attributes (Resistances/Power)
    float attr_none = 0;
    float attr_earth = 0;
    float attr_water = 0;
    float attr_divine = 0;
    float attr_wind = 0;
    float attr_fire = 0;
    float attr_dark = 0;

    // Masteries
    float master_sword = 0;
    float master_mace = 0;
    float master_axe = 0;
    float master_staff = 0;
    float master_bow = 0;
    float master_gun = 0;
    float master_dagger = 0;
    float master_spear = 0;
    float master_onehand = 0;
    float master_twohand = 0;
    float master_twoblade = 0;
    float master_robe = 0;
    float master_light_armor = 0;
    float master_heavy_armor = 0;

    // Speed Multipliers
    float normal_speed_rate = 1.0f;
    float physic_skill_speed_rate = 1.0f;
    float magic_skill_speed_rate = 1.0f;

    // Old formula fields
    int32_t weapon_attack = 0;       // Senjata equip attack
    int32_t armor_defense = 0;       // Armor equip defense
    int32_t base_strength = 0;       // Base STR sebelum buff/item
    int32_t base_dexterity = 0;
    uint8_t class_id = 0;          // 1=Fighter, 2=Rogue, 3=Ranger, 4=Mage
    float accuracy_pct = 0.0f;     // Percent accuracy buffs
    float accuracy_plus = 0.0f;    // Flat accuracy buffs
    float evasion_pct = 0.0f;      // Percent evasion buffs
    float evasion_plus = 0.0f;     // Flat evasion buffs
    int32_t base_vitality = 0;
    int32_t base_intelligence = 0;
    int32_t base_wisdom = 0;
    uint16_t enchant_level = 0;      // Enchant level senjata/baju
    float physic_attack_pct = 0.0f;  // Percent buffs (+%)
    float physic_attack_plus = 0.0f; // Flat buffs (+val)
    float physic_defense_pct = 0.0f;
    float physic_defense_plus = 0.0f;
    float magic_attack_pct = 0.0f;
    float magic_attack_plus = 0.0f;
    float magic_defense_pct = 0.0f;
    float magic_defense_plus = 0.0f;
};

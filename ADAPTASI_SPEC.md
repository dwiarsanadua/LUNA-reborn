# ADAPTASI SPEC v3 — Spesifikasi Teknis Final

> Update: 2026-06-09 (setelah scan langsung Old source code)
> Temuan kritis: Formula Combat Reborn **SALAH** — 9 dari 12 formula Old tidak cocok

---

## ⚠️ PRIORITAS #1: Koreksi Formula Combat

Dari hasil scan `[Server]Map/AttackManager.cpp` dan `[Client]LUNA/Hero.cpp`, ditemukan bahwa implementasi CombatSystem.cpp menggunakan formula yang berbeda secara fundamental dengan Old.

### Formula yang harus diperbaiki:

#### 1. Base Physical Attack — 🔴 SALAH

```cpp
// OLD (AttackManager.cpp + Hero.cpp):
// PhysicAttack = (WeaponAttack + Level) * (1 + STR * 0.001) + (RealSTR - 30)
//              × (1 + EnchantLevel² / 400)
//              × (1 + PercentBuffs / 100) + PlusBuffs
float CalcOldPhysicAttack(const CharacterStats& s) {
    float weapon_atk = s.weapon_attack;
    float base = (weapon_atk + s.level) * (1.0f + s.strength * 0.001f) + (s.strength - 30);
    float enchant_bonus = 1.0f + (s.enchant_level * s.enchant_level) / 400.0f;
    return base * enchant_bonus * (1.0f + s.physic_attack_pct / 100.0f) + s.physic_attack_plus;
}

// REBORN (SALAH):
// float damage = attacker.physic_attack - defender.physic_defense * 0.5f;
// HARUSNYA: hitung dari weapon + level + STR scaling
```

#### 2. Physical Defense — 🔴 SALAH

```cpp
// OLD (Hero.cpp):
// PhysicDefense = (ArmorDefense + Level) * (1 + VIT * 0.000333) + (RealVIT - 40) / 5
//               × (1 + EnchantLevel / 200)
//               × (1 + PercentBuffs / 100) + PlusBuffs
float CalcOldPhysicDefense(const CharacterStats& s) {
    float armor_def = s.armor_defense;
    float base = (armor_def + s.level) * (1.0f + s.vitality / 3000.0f) + (s.vitality - 40) / 5.0f;
    float enchant_bonus = 1.0f + s.enchant_level / 200.0f;
    return base * enchant_bonus;
}
```

#### 3. Critical Rate — 🔴 SALAH

```cpp
// OLD (Hero.cpp:1314):
// DexRate = max(DEX - BaseDEX, 0) / ((Level - 1) * 5)
// CritRate = 45 * DexRate + (BaseDEX - 25) / 5 + buffs
// ClassVals: Fighter=0.14, Rogue=0.18, Ranger=0.11, Mage=0.15
float CalcOldCritRate(const CharacterStats& s) {
    float dex_rate = std::max(0.0f, (float)(s.dexterity - s.base_dexterity)) / ((s.level - 1) * 5.0f);
    float base_contrib = (s.base_dexterity - 25.0f) / 5.0f;
    float class_bonus = s.class_crit_factor; // 45 for phys, 10 for magic
    return class_bonus * dex_rate + base_contrib + s.crit_rate_buff;
}
// NOT: DEX / 1000
```

#### 4. Block Rate — 🔴 SALAH

```cpp
// OLD (Player.cpp:3610):
// Block = DEX / 27 + ClassBonus + buffs
// ClassBonus: Fighter=15, Rogue=10, Ranger=5, Mage=9
float CalcOldBlockRate(const CharacterStats& s) {
    static const float CLASS_BLOCK_BONUS[] = {0, 15, 10, 5, 9}; // 1=Fighter, 2=Rogue...
    float class_bonus = (s.class_id >= 1 && s.class_id <= 4) ? CLASS_BLOCK_BONUS[s.class_id] : 0;
    return s.dexterity / 27.0f + class_bonus + s.block_rate_buff;
}
// NOT: CON / 2000
```

#### 5. Accuracy / Evasion System — 🔴 SALAH (MISSING)

```cpp
// OLD (Hero.cpp:1224-1290):
// Accuracy = (DexRate * 0.6 + (BaseDEX - 30)/1000 + Level * ClassVal/15000) * 100
// Evasion = formula identik (pakai Avoid buffs)
// HitChance = 85 + Accuracy - Evasion
// if HitChance < rand(1,100) → MISS
// BUKAN: 1% fixed miss rate!
```

#### 6. Level Penalty — 🟡 SALAH

```cpp
// OLD (AttackManager.cpp):
// HANYA jika attacker level < defender level:
// damage *= 1 + (atkLevel - defLevel) * 0.015
// (HANYA penalty, bukan bonus untuk level tinggi)
// REBORN: ±5% per level, cap 50% (dua arah) → SALAH
```

#### 7. PvP Damage Reduction — 🔴 MISSING

```cpp
// OLD (AttackManager.cpp):
// PvP: damage *= 0.35  (35% damage ke player lain)
// GT:  damage *= 0.10  (10% damage di Guild Tournament)
```

#### 8. Heal Formula — 🔴 MISSING

```cpp
// OLD (AttackManager.cpp:RecoverLife):
// heal = ((Wisdom * 11 + Int * 4 + Level * 20) * SkillFactor) / 800 + 100
// GT: heal *= 0.5
// Aggro to monster: heal / 3
```

#### 9. Element System — 🟡 DIHAPUS

```cpp
// Old TIDAK punya elemental advantage dalam damage formula.
// Element attributes exist on items but are NOT used in CalcDamage.
// Reborn menambahkan GetElementAdvantage() — harus dihapus atau 
// dibuat sebagai optional system (bukan core combat).
```

### Pseudo-code Implementasi yang Benar:

```cpp
// CombatSystem.cpp — CalculateDamage yang benar sesuai Old

struct OldDamageResult {
    int32_t damage = 0;
    bool is_miss = false;
    bool is_critical = false;
    bool is_blocked = false;
    int32_t threat_generated = 0;
};

OldDamageResult CalculateDamageOld(
    const CharacterStats& attacker,
    const CharacterStats& defender,
    uint16_t skill_id,
    float skill_add_damage,
    uint8_t add_type,       // 1=STR, 2=Weapon
    float rate_add_value,
    float plus_add_value,
    bool is_pvp,
    bool is_gt,
    int32_t aggro_count     // number of monsters aggroing this target
) {
    OldDamageResult result;

    // 1. Damage calculation
    float atk, def;
    if (add_type == 1) { // STR-based
        atk = (attacker.weapon_attack + attacker.level) 
            * (1.0f + attacker.strength * 0.001f) 
            + (attacker.strength - 30);
    } else { // Weapon-based
        atk = (attacker.weapon_attack + attacker.level) 
            * (1.0f + attacker.weapon_attack * 0.001f) 
            + (attacker.strength - 30);
    }
    
    // Enchant bonus
    atk *= 1.0f + (attacker.enchant_level * attacker.enchant_level) / 400.0f;
    // Buffs
    atk *= 1.0f + attacker.physic_attack_pct / 100.0f;
    atk += attacker.physic_attack_plus;

    // Defense
    def = (attacker.armor_defense + attacker.level) 
        * (1.0f + attacker.vitality / 3000.0f) 
        + (attacker.vitality - 40) / 5.0f;
    def *= 1.0f + attacker.enchant_level / 200.0f;
    def *= 1.0f + attacker.physic_defense_pct / 100.0f;

    float damage = atk - def;

    // 2. Skill modifier
    damage = damage * (1000.0f + skill_add_damage) / 1000.0f;
    damage = damage * (1.0f + rate_add_value / 100.0f) + plus_add_value;

    // 3. Level penalty (ONLY if attacker < defender)
    if (attacker.level < defender.level) {
        damage *= 1.0f + (attacker.level - defender.level) * 0.015f;
    }

    // 4. Accuracy check
    float dex_rate = std::max(0.0f, (float)(attacker.dexterity - attacker.base_dexterity)) 
                     / ((attacker.level - 1) * 5.0f);
    float accuracy = (dex_rate * 0.6f + (attacker.base_dexterity - 30.0f) / 1000.0f 
                     + attacker.level * attacker.class_accuracy_factor / 15000.0f) * 100.0f;
    // Same for evasion
    float evasion = /* similar formula with avoid stats */;
    float hit_chance = 85.0f + accuracy - evasion;
    if (hit_chance < (float)(rand() % 100 + 1)) {
        result.is_miss = true;
        return result;
    }

    // 5. Block check
    float block_rate = attacker.dexterity / 27.0f + attacker.class_block_bonus 
                      + attacker.block_rate_buff;
    if (attacker.shield_defense > 0 && block_rate > (float)(rand() % 100)) {
        result.is_blocked = true;
        damage = damage * (0.6f - attacker.vitality / 4000.0f) - attacker.shield_defense;
    }

    // 6. Critical check
    float crit_rate = 45.0f * dex_rate + (attacker.base_dexterity - 25.0f) / 5.0f 
                     + attacker.crit_rate_buff;
    if (!result.is_blocked && crit_rate > (float)(rand() % 100)) {
        result.is_critical = true;
        damage = damage * 1.5f * (1.0f + attacker.crit_damage_rate / 100.0f) 
                + attacker.crit_damage_plus;
    }

    // 7. Aggro penalty
    if (aggro_count >= 3) {
        float avoid_penalty = 1.0f - (aggro_count - 2) * (aggro_count - 2) * 0.01f;
        // Apply to subsequent attacks (handled externally)
    }
    if (aggro_count >= 6) {
        float defense_penalty = 1.0f - std::pow((float)(aggro_count - 5), 1.5f) * 0.01f;
        def *= defense_penalty;
    }

    // 8. PvP / GT modifier
    if (is_gt) damage *= 0.10f;
    else if (is_pvp) damage *= 0.35f;

    // 9. Variance
    float variance = 1.0f + ((float)(rand() % 21 - 10) / 100.0f); // ±10%
    damage *= variance;

    // 10. Clamp
    result.damage = std::max(1, (int)damage);
    result.threat_generated = result.damage;

    return result;
}
```

---

## Prioritas Sisa Pekerjaan (Non-Combat)

```
PRIORITAS SISA GAP (Setelah Koreksi Formula)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
P0  🔴 Koreksi 9 formula combat (lihat atas)        3 weeks
P1  🔴 Wire NPC_SPEECH_SYN handler (server+client)   1 week
P1  🔴 Wire VEHICLE server handlers                  2 weeks
P2  🟡 Implement accuracy/evasion system              2 weeks
P2  🟡 Implement PvP/GT damage reduction              1 week
P2  🟡 Implement heal formula                         1 week
P2  🟡 Wire MOVE_STOP/MOVE_TELEPORT handler           1 week
P3  🟡 Missing critical dialogs (NPCShop, dll.)       2 weeks
P3  🟡 Server subsystems (Looting, Exchange, dll.)    3 weeks
P4  🟡 Security mitigations (validation wiring)       2 weeks
P4  🟡 Localization (CJK font, strings)               2 weeks
P5  🔵 Minor dialogs (~20)                            2 weeks
P5  🔵 Build tools (PackingTool, dll.)                1 week
P5  🔵 Collision → movement integration               1 week
─────────────────────────────────────────────────────────────────────────────
TOTAL:                      ~24 weeks (1 FTE ~6 months)
─────────────────────────────────────────────────────────────────────────────
```

---

*End of ADAPTASI_SPEC.md v3 — Prioritas: Koreksi Formula Combat*


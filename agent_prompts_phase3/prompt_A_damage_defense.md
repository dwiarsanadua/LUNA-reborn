# Agent A — Fix Base Damage & Defense Formulas 🔴 KRITIKAL

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Masalah

CombatSystem.cpp menggunakan formula yang SALAH. Base damage dan defense tidak sesuai Old.

```
OLD (Hero.cpp + AttackManager.cpp):
  PhysicAttack = (WeaponAttack + Level) × (1 + STR × 0.001) + (RealSTR − 30)
                 × (1 + EnchantLevel² / 400)
                 × (1 + PercentBuffs / 100) + PlusBuffs

  PhysicDefense = (ArmorDefense + Level) × (1 + VIT / 3000) + (RealVIT − 40) / 5
                  × (1 + EnchantLevel / 200)
                  × (1 + PercentBuffs / 100) + PlusBuffs

REBORN (SALAH):
  damage = attacker.physic_attack - defender.physic_defense * 0.5f
```

## File yang harus diubah

### game/ecs/components/CharacterStats.hpp [UPDATE]

Tambah field yang diperlukan untuk Old formula:
```cpp
// Fields to ADD:
int32_t weapon_attack = 0;       // Senjata equip attack
int32_t armor_defense = 0;       // Armor equip defense  
int32_t base_strength = 0;       // Base STR sebelum buff/item
int32_t base_dexterity = 0;
int32_t base_vitality = 0;
int32_t base_intelligence = 0;
int32_t base_wisdom = 0;
uint16_t enchant_level = 0;      // Enchant level senjata/baju
float physic_attack_pct = 0.0f;  // Percent buffs (+%)
float physic_attack_plus = 0.0f; // Flat buffs (+val)
float physic_defense_pct = 0.0f;
float magic_attack_pct = 0.0f;
float magic_defense_pct = 0.0f;
```

### game/ecs/systems/CombatSystem.hpp [UPDATE]

Tambah method baru:
```cpp
// Old-accurate stat calculations
static float CalcPhysicAttack(const CharacterStats& s);
static float CalcPhysicDefense(const CharacterStats& s);
static float CalcMagicAttack(const CharacterStats& s);
static float CalcMagicDefense(const CharacterStats& s);
static float CalcHealAmount(const CharacterStats& healer, float skill_factor);
```

### game/ecs/systems/CombatSystem.cpp [UPDATE — MAIN]

**Replace CalculateDamage() dengan Old-accurate version:**

```cpp
float CombatSystem::CalcPhysicAttack(const CharacterStats& s) {
    // OLD: (WeaponAttack + Level) × (1 + STR × 0.001) + (RealSTR − 30)
    float base = (s.weapon_attack + s.level) * (1.0f + s.strength * 0.001f)
                 + (s.strength - s.base_strength);
    // Enchant: × (1 + EnchantLevel² / 400)
    float enchant = 1.0f + (s.enchant_level * s.enchant_level) / 400.0f;
    // Buffs: × (1 + Percent/100) + Plus
    return base * enchant * (1.0f + s.physic_attack_pct / 100.0f) + s.physic_attack_plus;
}

float CombatSystem::CalcPhysicDefense(const CharacterStats& s) {
    // OLD: (ArmorDefense + Level) × (1 + VIT / 3000) + (RealVIT − 40) / 5
    float base = (s.armor_defense + s.level) * (1.0f + s.vitality / 3000.0f)
                 + (s.vitality - 40.0f) / 5.0f;
    // Enchant: × (1 + EnchantLevel / 200)
    float enchant = 1.0f + s.enchant_level / 200.0f;
    // Buffs
    return base * enchant * (1.0f + s.physic_defense_pct / 100.0f) + s.physic_defense_plus;
}
```

**Update CalculateDamage() — gunakan CalcPhysicAttack/Defense:**

```cpp
DamageResult CombatSystem::CalculateDamage(const CharacterStats& attacker,
                                           const CharacterStats& defender,
                                           float skill_add_damage,
                                           uint8_t add_type,     // 1=STR, 2=Weapon
                                           float rate_add_value,
                                           float plus_add_value) {
    DamageResult result{};

    // 1. Hit chance (Accuracy vs Evasion) — panggil dari Agent C nanti
    // Sementara pakai 85% base hit rate
    if (std::uniform_real_distribution<float>(0, 1)(rng) > 0.85f) {
        result.is_miss = true;
        return result;
    }

    // 2. Base damage — Old formula
    float atk, def;
    if (add_type == 1) { // STR-based
        atk = (attacker.weapon_attack + attacker.level)
            * (1.0f + attacker.strength * 0.001f)
            + (attacker.strength - attacker.base_strength);
        // Enchant
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
        * (1.0f + defender.vitality / 3000.0f)
        + (defender.vitality - 40.0f) / 5.0f;
    def *= 1.0f + defender.enchant_level / 200.0f;

    float damage = atk - def;
    damage = std::max(1.0f, damage);

    // 3. Skill modifier — sama seperti sebelumnya
    if (add_type == 1) {
        damage = damage * ((1000.0f + skill_add_damage + attacker.strength) / 1000.0f);
    } else if (add_type == 2) {
        damage = damage * ((1000.0f + skill_add_damage + attacker.weapon_attack) / 1000.0f);
    }
    damage = damage * (1.0f + rate_add_value / 100.0f) + plus_add_value;

    // 4. Level penalty — HANYA jika attacker < defender (Agent D nanti)
    // Sementara dipasang dulu yang benar:
    if (attacker.level < defender.level) {
        float diff = static_cast<float>(defender.level - attacker.level);
        damage *= 1.0f + diff * 0.015f; // -1.5% per level
    }

    // 5. Variance ±10%
    float variance = 1.0f + std::uniform_real_distribution<float>(-0.10f, 0.10f)(rng);
    damage *= variance;

    result.damage = std::max(1, static_cast<int32_t>(damage));
    return result;
}
```

## Aturan

1. LOAD file dulu sebelum edit
2. JANGAN hapus fungsi yang sudah ada — tambah fungsi baru
3. JAGA kompatibilitas: HandleAttack() tetap dipanggil dengan cara yang sama
4. JANGAN build atau compile
5. ✅ Kembalikan "Agent A done: base damage & defense formulas fixed"

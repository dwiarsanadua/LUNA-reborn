# Agent B — Fix Critical & Block Formulas 🔴 KRITIKAL

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Masalah

```
OLD CRIT (Hero.cpp:1314):
  DexRate = max(DEX − BaseDEX, 0) / ((Level − 1) × 5)
  CritRate = 45 × DexRate + (BaseDEX − 25) / 5
  + PercentBuffs + PlusBuffs / 45
  
  ClassVals: Fighter=0.14, Rogue=0.18, Ranger=0.11, Mage=0.15
  Untuk magic crit: 10 × IntRate + 20 × WisRate + (BaseINT−25)/5

REBORN (SALAH):
  crit_rate = attacker.dexterity / 1000.0f

OLD BLOCK (Player.cpp:3610):
  Block = DEX / 27 + ClassBonus + BonusBuffs
  ClassBonus: Fighter=15, Rogue=10, Ranger=5, Mage=9

REBORN (SALAH):
  block_chance = defender.constitution / 2000.0f
```

## File yang harus diubah

### game/ecs/components/CharacterStats.hpp [UPDATE]

Tambah field:
```cpp
int32_t base_dexterity = 0;
int32_t base_intelligence = 0;
int32_t base_wisdom = 0;
uint8_t class_id = 0;              // 1=Fighter, 2=Rogue, 3=Ranger, 4=Mage
float crit_rate_buff = 0.0f;       // Percent buffs untuk crit
float crit_damage_rate = 0.0f;     // Crit damage % bonus
float crit_damage_plus = 0.0f;     // Crit damage flat bonus
float block_rate_buff = 0.0f;
int32_t shield_defense = 0;        // Shield item defense
```

### game/ecs/systems/CombatSystem.hpp [UPDATE]

Tambah:
```cpp
static float CalcDexRate(const CharacterStats& s);     // DexRate helper
static float CalcIntRate(const CharacterStats& s);     // IntRate helper  
static float CalcWisRate(const CharacterStats& s);     // WisRate helper
static float CalcCritRate(const CharacterStats& s);    // Old crit formula
static float CalcMagicCritRate(const CharacterStats& s);
static float CalcBlockRate(const CharacterStats& s);   // Old block formula
static float GetClassBlockBonus(uint8_t class_id);     // Class block table
```

### game/ecs/systems/CombatSystem.cpp [UPDATE]

Implementasi:

```cpp
float CombatSystem::CalcDexRate(const CharacterStats& s) {
    // DexRate = max(DEX − BaseDEX, 0) / ((Level − 1) × 5)
    float dex_gain = std::max(0.0f, static_cast<float>(s.dexterity - s.base_dexterity));
    float level_factor = std::max(1.0f, static_cast<float>(s.level - 1) * 5.0f);
    return dex_gain / level_factor;
}

float CombatSystem::CalcIntRate(const CharacterStats& s) {
    float int_gain = std::max(0.0f, static_cast<float>(s.intelligence - s.base_intelligence));
    float level_factor = std::max(1.0f, static_cast<float>(s.level - 1) * 5.0f);
    return int_gain / level_factor;
}

float CombatSystem::CalcCritRate(const CharacterStats& s) {
    // CritRate = 45 × DexRate + (BaseDEX − 25) / 5 + buffs
    float dex_rate = CalcDexRate(s);
    float base_contrib = (s.base_dexterity - 25.0f) / 5.0f;
    float buff_contrib = s.crit_rate_buff + s.crit_rate_buff_flat / 45.0f;
    return 45.0f * dex_rate + base_contrib + buff_contrib;
}

float CombatSystem::CalcMagicCritRate(const CharacterStats& s) {
    // MagicCritRate = 10 × IntRate + 20 × WisRate + (BaseINT−25)/5 + buffs
    float int_rate = CalcIntRate(s);
    float wis_rate = CalcWisRate(s);
    return 10.0f * int_rate + 20.0f * wis_rate + (s.base_intelligence - 25.0f) / 5.0f
           + s.crit_rate_buff + s.crit_rate_buff_flat / 45.0f;
}

float CombatSystem::GetClassBlockBonus(uint8_t class_id) {
    // OLD: Fighter=15, Rogue=10, Ranger=5, Mage=9
    static const float BONUS[] = {0, 15, 10, 5, 9};
    if (class_id >= 1 && class_id <= 4) return BONUS[class_id];
    return 0;
}

float CombatSystem::CalcBlockRate(const CharacterStats& s) {
    // Block = DEX / 27 + ClassBonus + buffs
    return s.dexterity / 27.0f + GetClassBlockBonus(s.class_id) + s.block_rate_buff;
}
```

**Update bagian critical hit di CalculateDamage():**

```cpp
// Ganti ini:
// float crit_rate = attacker.dexterity / 1000.0f;

// Menjadi:
float crit_rate = CalcCritRate(attacker);
// Pastikan crit_rate dalam persen (0-100 scale)
crit_rate = std::clamp(crit_rate, 0.0f, 100.0f);

// Ganti crit damage:
// float crit_dmg = 1.50f + attacker.strength / 200.0f;
// Menjadi:
float crit_dmg = 1.50f * (1.0f + attacker.crit_damage_rate / 100.0f) + attacker.crit_damage_plus;
```

**Update bagian block di CalculateDamage():**

```cpp
// Ganti ini:
// float block_chance = defender.constitution / 2000.0f;

// Menjadi:
float block_chance = CalcBlockRate(defender);
block_chance = std::clamp(block_chance, 0.0f, 100.0f);
```

## Aturan

1. LOAD file dulu sebelum edit — baca CombatSystem.hpp dan CombatSystem.cpp
2. TAMBAH fungsi baru, jangan hapus yang lama (kecuali formula yang salah)
3. PASTIKAN inline helper functions (CalcDexRate dll) accessible dari CalculateDamage
4. JANGAN build atau compile
5. ✅ Kembalikan "Agent B done: crit & block formulas fixed"

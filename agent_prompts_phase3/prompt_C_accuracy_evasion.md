# Agent C — Implement Accuracy/Evasion System 🔴 KRITIKAL

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Masalah

Old punya accuracy/evasion system penuh. Reborn pakai 1% fixed miss rate — SALAH.

```
OLD (Hero.cpp:1224-1290):
  DexRate = max(DEX−BaseDEX, 0) / ((Level−1) × 5)
  
  Accuracy = (DexRate × 0.6 + (BaseDEX−30)/1000 + Level × ClassVal/15000) × 100
             × (1 + Percent/100) + Plus
  ClassVal: Fighter=14, Rogue=18, Ranger=11, Mage=15
  
  Evasion = (DexRate × 0.6 + (BaseDEX−30)/1000 + Level × ClassVal/15000) × 100
            × (1 + Percent/100) + Plus
  (Evasion menggunakan Avoid buffs, formula identik)
  
  HitChance = 85 + Accuracy − Evasion
  if HitChance < rand(1,100) → MISS

REBORN (SALAH):
  miss_chance = 0.01f; // 1% fixed
```

## File yang harus diubah

### game/ecs/components/CharacterStats.hpp [UPDATE]

```cpp
float accuracy_pct = 0.0f;     // Percent accuracy buffs
float accuracy_plus = 0.0f;    // Flat accuracy buffs
float evasion_pct = 0.0f;      // Percent evasion buffs  
float evasion_plus = 0.0f;     // Flat evasion buffs
uint8_t class_id = 0;          // 1=Fighter, 2=Rogue, 3=Ranger, 4=Mage
int32_t base_dexterity = 0;
```

### game/ecs/systems/CombatSystem.hpp [UPDATE]

```cpp
static float CalcAccuracy(const CharacterStats& s);
static float CalcEvasion(const CharacterStats& s);
static float GetClassAccuracyFactor(uint8_t class_id);
static float GetClassEvasionFactor(uint8_t class_id);
static bool IsHit(const CharacterStats& attacker, const CharacterStats& defender);
```

### game/ecs/systems/CombatSystem.cpp [UPDATE]

Implementasi:

```cpp
float CombatSystem::GetClassAccuracyFactor(uint8_t class_id) {
    // OLD: Fighter=14, Rogue=18, Ranger=11, Mage=15
    static const float FACTOR[] = {0, 14, 18, 11, 15};
    if (class_id >= 1 && class_id <= 4) return FACTOR[class_id];
    return 14;
}

float CombatSystem::CalcAccuracy(const CharacterStats& s) {
    // DexRate sama seperti di Agent B
    float dex_rate = CalcDexRate(s);
    
    // Accuracy = (DexRate × 0.6 + (BaseDEX−30)/1000 + Level × ClassVal/15000) × 100
    float acc = (dex_rate * 0.6f 
                + (s.base_dexterity - 30.0f) / 1000.0f
                + s.level * GetClassAccuracyFactor(s.class_id) / 15000.0f) * 100.0f;
    
    // Buffs: × (1 + Percent/100) + Plus
    acc = acc * (1.0f + s.accuracy_pct / 100.0f) + s.accuracy_plus;
    
    return acc;
}

float CombatSystem::CalcEvasion(const CharacterStats& s) {
    // Formula identik dengan Accuracy, tapi pakai Avoid buffs
    float dex_rate = CalcDexRate(s);
    
    float eva = (dex_rate * 0.6f
                + (s.base_dexterity - 30.0f) / 1000.0f
                + s.level * GetClassAccuracyFactor(s.class_id) / 15000.0f) * 100.0f;
    
    eva = eva * (1.0f + s.evasion_pct / 100.0f) + s.evasion_plus;
    
    return eva;
}

bool CombatSystem::IsHit(const CharacterStats& attacker, const CharacterStats& defender) {
    // HitChance = 85 + Accuracy − Evasion
    float accuracy = CalcAccuracy(attacker);
    float evasion = CalcEvasion(defender);
    float hit_chance = 85.0f + accuracy - evasion;
    
    // Clamp: minimal 10%, maksimal 99%
    hit_chance = std::clamp(hit_chance, 10.0f, 99.0f);
    
    // Roll
    float roll = std::uniform_real_distribution<float>(0, 100)(rng);
    return roll < hit_chance;
}
```

**Update CalculateDamage() — ganti miss check:**

```cpp
// HAPUS ini:
// float miss_chance = 0.01f;
// if (std::uniform_real_distribution<float>(0, 1)(rng) < miss_chance) {
//     result.is_miss = true;
//     return result;
// }

// GANTI dengan:
if (!IsHit(attacker, defender)) {
    result.is_miss = true;
    spdlog::debug("DAMAGE: MISS (acc={:.1f} eva={:.1f} hit_chance={:.1f}%)",
                  CalcAccuracy(attacker), CalcEvasion(defender),
                  85.0f + CalcAccuracy(attacker) - CalcEvasion(defender));
    return result;
}
```

## Aturan

1. LOAD file dulu sebelum edit
2. Fungsi CalcDexRate() perlu ada — pastikan sudah dideklarasikan (dari Agent B)
3. JAGA: system RNG yang sudah ada (std::mt19937 rng) — reuse
4. JANGAN build atau compile
5. ✅ Kembalikan "Agent C done: accuracy/evasion system implemented"

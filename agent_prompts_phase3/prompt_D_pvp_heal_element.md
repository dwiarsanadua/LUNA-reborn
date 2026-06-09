# Agent D — PvP Reduction, Level Penalty, Heal, Element Removal 🔴 KRITIKAL

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Masalah

4 isu di CombatSystem:

1. **Level penalty**: Old hanya -1.5% per level jika atk < def. Reborn ±5% dua arah. SALAH.
2. **PvP reduction**: Old punya 35% damage reduction untuk PvP, 10% untuk GT. REBORN MISSING.
3. **Heal formula**: Old punya formula spesifik. REBORN MISSING.
4. **Element system**: Old TIDAK punya elemental advantage. REBORN punya. HARUS DIREMOVE.

```
OLD LEVEL PENALTY (AttackManager.cpp):
  HANYA jika attacker level < defender level:
    damage *= 1 + (atkLevel − defLevel) × 0.015
  Jika attacker >= defender: tidak ada modifikasi

OLD PvP DAMAGE (AttackManager.cpp):
  PvP: damage *= 0.35  (hanya 35% damage ke player lain)
  GT:  damage *= 0.10  (hanya 10% damage di Guild Tournament)

OLD HEAL (AttackManager.cpp:RecoverLife):
  heal = ((Wisdom × 11 + Int × 4 + Level × 20) × SkillFactor) / 800 + 100
  GT: heal *= 0.5
  Aggro: heal / 3

OLD ELEMENT:
  Tidak ada elemental advantage dalam damage formula.
  Element attributes exist on items but NOT used in CalcDamage.
```

## File yang harus diubah

### game/ecs/systems/CombatSystem.hpp [UPDATE]

```cpp
// Tambah:
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

// Update signature:
static DamageResult CalculateDamage(const CharacterStats& attacker,
                                    const CharacterStats& defender,
                                    float skill_add_damage,
                                    uint8_t add_type,
                                    float rate_add_value,
                                    float plus_add_value,
                                    CombatContext context = CombatContext::Normal);

// New methods:
static float CalcHealAmount(const CharacterStats& healer, float skill_factor);
```

### game/ecs/systems/CombatSystem.cpp [UPDATE]

**1. Fix Level Penalty:**

```cpp
// HAPUS ini:
// float level_mod = 1.0f + std::clamp(static_cast<float>(level_diff) * 0.05f, -0.50f, 0.50f);
// damage *= level_mod;

// GANTI dengan:
if (attacker.level < defender.level) {
    int32_t level_diff = defender.level - attacker.level;
    float penalty = 1.0f - level_diff * 0.015f; // -1.5% per level
    penalty = std::max(penalty, 0.5f); // cap 50%
    damage *= penalty;
}
// Jika attacker.level >= defender.level: NO MODIFICATION
```

**2. Add PvP/GT Reduction:**

```cpp
// Tambah di CalculateDamage(), setelah block/crit check, sebelum variance:
// PvP / GT damage reduction
if (context == CombatContext::PvP) {
    damage *= 0.35f;  // 35% damage ke player
} else if (context == CombatContext::GuildTournament) {
    damage *= 0.10f;  // 10% damage di GT
} else if (context == CombatContext::Siege) {
    damage *= 0.50f;  // 50% damage di siege (estimasi)
}
```

**3. Add Heal Formula:**

```cpp
float CombatSystem::CalcHealAmount(const CharacterStats& healer, float skill_factor) {
    // OLD: heal = ((Wisdom × 11 + Int × 4 + Level × 20) × SkillFactor) / 800 + 100
    float heal = (healer.wisdom * 11.0f + healer.intelligence * 4.0f + healer.level * 20.0f)
                 * skill_factor / 800.0f + 100.0f;
    return std::max(1.0f, heal);
}
```

**4. Remove Element System:**

```cpp
// HAPUS fungsi GetAttackElement() dan GetElementAdvantage()
// HAPUS bagian ini dari CalculateDamage():
// Element atk_elem = GetAttackElement(attacker);
// Element def_elem = GetAttackElement(defender);
// float elem_mult = GetElementAdvantage(atk_elem, def_elem);
// damage *= elem_mult;
```

Atau alternatif: Comment out dengan `[[deprecated]]` daripada hapus, untuk jaga-jaga jika mau dipakai sebagai optional system nanti.

### client/gameobjects/Hero.cpp [UPDATE]

Jika ada referensi ke element system di client-side Hero, update juga.

### game/ecs/systems/CombatSystem.hpp [UPDATE]

```cpp
// Comment out atau [[deprecated]]:
// [[deprecated("Element system tidak ada di Old. Disabled untuk akurasi.")]]
// static Element GetAttackElement(const CharacterStats& stats);
// static float GetElementAdvantage(Element atk_elem, Element def_elem);
```

## Update HandleAttack signature

Di CombatSystem.hpp, update HandleAttack untuk menerima CombatContext:

```cpp
void HandleAttack(entt::registry& registry,
                  entt::entity attacker, entt::entity target,
                  uint16_t skill_id,
                  CombatContext context = CombatContext::Normal);
```

## Aturan

1. LOAD file dulu sebelum edit
2. JANGAN hapus kode secara fisik — comment out dengan penjelasan
3. Pastikan DamageResult struct konsisten antara .hpp dan .cpp
4. JANGAN build atau compile
5. ✅ Kembalikan "Agent D done: PvP/level/heal/element fixed"

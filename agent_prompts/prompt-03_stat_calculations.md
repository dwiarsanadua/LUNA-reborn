# Agent-03 — Server Methods: Character Stat Calculations (Old: CharacterCalcManager)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Implementasi method kalkulasi stat karakter yang masih missing. Old punya CharacterCalcManager.cpp (3.412 lines) dengan formula stat detail. Reborn sudah punya CalcMaxLife/CalcMaxMana dari agent F. Perlu method tambahan.

## Aturan Ketat

1. BACA game/ecs/systems/CombatSystem.hpp — catat method yang SUDAH ADA
2. CEK satu per satu method berikut apakah sudah ada:
   - `CalcMaxLife` → cek dengan rg "CalcMaxLife" 
   - `CalcMaxMana` → cek
   - `CalcStatPoint` → cek (distribusi STR/DEX/VIT/INT/WIS)
   - `GetAttackSpeed` → cek (Old: DoGetAttackSpeedRate)
   - `GetCastingSpeed` → cek (Old: DoGetCastingSpeedRate)
   - `GetMoveSpeed` → cek (Old: DoGetMoveSpeedRate)
3. ✅ Jika SUDAH ADA → skip. Jangan duplikasi.
4. 🔧 Jika BELUM ADA → tambah dengan style yang sama.

## Method Prioritas (dari Old CharacterCalcManager)

```cpp
// Tambah di CombatSystem.hpp:
static int32_t CalcMaxLife(const CharacterStats& s);      // ✅ sudah ada dari agent F
static int32_t CalcMaxMana(const CharacterStats& s);      // ✅ sudah ada dari agent F
static int32_t CalcRequiredExp(uint16_t level);            // 🔧 cek
static float CalcAttackSpeed(const CharacterStats& s);     // 🔧 cek (Old: DoGetAttackSpeedRate)
static float CalcCastingSpeed(const CharacterStats& s);    // 🔧 cek
static float CalcMoveSpeed(const CharacterStats& s);       // 🔧 cek
static void ApplyStatPoint(CharacterStats& s, uint8_t stat_type, int32_t points); // 🔧 cek
```

Jika method belum ada, implementasi:
```cpp
int32_t CombatSystem::CalcRequiredExp(uint16_t level) {
    // Old: level * level * 100 + level * 50 (sederhana, bisa disesuaikan)
    return static_cast<int32_t>(level) * level * 100 + level * 50;
}

float CombatSystem::CalcAttackSpeed(const CharacterStats& s) {
    // Old: DoGetAttackSpeedRate — dari passives + buffs
    return s.physic_skill_speed_rate;
}

float CombatSystem::CalcMoveSpeed(const CharacterStats& s) {
    // Old: DoGetMoveSpeed() / 4.5 - 100
    return s.move_speed;
}
```

## Output

✅ Kembalikan: "Agent-03 done: added [list method yang ditambah], skipped [list method yang sudah ada]"

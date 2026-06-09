# Agent FINAL-F — Implementasi Method Server yang Missing

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Implementasi method-method server yang masih missing. Dari verifikasi agents A-D:
- Old Map server: 1.938 methods (116 files)
- Reborn Map server: ~723 methods (18 files) — corrected dari klaim 902
Gap: ~1.215 methods. Target: tambah method yang paling kritis (P1-P5).

## Aturan Ketat (WAJIB)

1. **📖 BACA dulu Reborn file** — pahami struktur dan style yang sudah ada.
2. **📖 BACA Old reference** — pahami logic asli sebelum implementasi.
3. **✅ Jika method SUDAH ADA di Reborn** — skip. JANGAN buat ulang.
4. **🔧 Jika method BELUM ADA** — implementasi dengan style yang SAMA dengan kode existing.
5. **🚫 JANGAN copy-paste Old code**. Adaptasi ke ECS pattern Reborn.
6. **🚫 JANGAN hapus kode existing** — hanya tambah.
7. **✅ Build setelah selesai** — 0 error, 0 warning.

## Prioritas Method (dari yang paling kritis)

### P1: CharacterCalcManager (Old: 3.412 lines)
Old punya kalkulasi stat lengkap. Reborn punya CharacterStats.hpp dengan field-field Old.

**Baca Old**: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/[Server]Map/CharacterCalcManager.cpp
- Cari method CalcMaxLife, CalcMaxMana, CalcCharPhyAttack, CalcCharPhyDefense
- Cari AddStat (STR/DEX/VIT/INT/WIS point distribution)

**Cek Reborn**: 
- game/ecs/systems/CombatSystem.cpp — apakah CalcMaxLife/CalcMaxMana sudah ada?
- Jika BELUM: tambah method static ke CombatSystem:

```cpp
static int32_t CalcMaxLife(const CharacterStats& s);
static int32_t CalcMaxMana(const CharacterStats& s);
static void ApplyStatPoint(CharacterStats& s, uint8_t stat_type, int32_t points);
```

### P2: Distributer (Old: 1.975 lines) — Party XP/Item Distribution
**Baca Old**: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/[Server]Map/Distributer.cpp
- Cari method DistributeXP, DistributeItem, DistributeGold

**Cek Reborn**:
- server/map/systems/PartySystem.cpp — cari DistributeXP, ShareExp, DistributeLoot
- Jika BELUM: tambah ke PartySystem.cpp:

```cpp
void DistributeXP(entt::registry& reg, entt::entity party_entity, int32_t total_exp);
void DistributeLoot(entt::registry& reg, entt::entity party_entity, entt::entity monster);
void DistributeGold(entt::registry& reg, entt::entity party_entity, int32_t total_gold);
```

### P3: Monster (Old: 1.421 lines)
**Baca Old**: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/[Server]Map/Monster.cpp
- Cari AI states, aggro logic, death handling

**Cek Reborn**:
- game/ecs/systems/AISystem.cpp — cari method untuk monster states
- Jika method AI state seperti Patrol, Chase, Flee belum ada:
  Implementasi FSM sederhana di AISystem.cpp

### P4: MoveManager Recall (Old: 1.258 lines)
**Baca Old**: MoveManager.cpp — cari RequestToRecall, Recall

**Cek Reborn**:
- server/map/systems/RecallSystem.cpp — cari method Recall, RecallParty, BindLocation
- Jika BELUM: implementasi sesuai Old pattern

### P5: Housing (Old: 3.554 lines)
**Baca Old**: HousingMgr.cpp — cari house ownership, decoration

**Cek Reborn**:
- game/ecs/systems/HousingSystem.cpp — cari method yang sudah ada
- Tambah method yang missing: Decorate, AddFurniture, RemoveFurniture, GetHouseInfo

## Verifikasi

```
SEBELUM: 902 methods di Reborn Map server
SESUDAH: Hitung total method yang ditambah
LAPORKAN: "Added X methods across Y files, total now Z methods"
```

## ✅ Kembalikan "Agent FINAL-F done: implemented X missing methods, 0 build errors"

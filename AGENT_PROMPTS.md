# READY-TO-USE PROMPTS — Phase 4: Content & Balance

> **Target:** Quest chains, economy balance, boss mechanics, map spawns, combat tuning.
> **5 Agent Paralel** — Zero file overlap. Aman dijalankan bersamaan.

---

## Daftar Isi

1. [Agent Q — Quest Data Engineer](#1-agent-q--quest-data-engineer)
2. [Agent E — Economy Balancer](#2-agent-e--economy-balancer)
3. [Agent B — Boss Mechanic Implementor](#3-agent-b--boss-mechanic-implementor)
4. [Agent S — Map Spawn Integrator](#4-agent-s--map-spawn-integrator)
5. [Agent C — Combat & Drop Tuning](#5-agent-c--combat--drop-tuning)
6. [Ringkasan Paralel](#6-ringkasan-paralel)

---

## 1. Agent Q — Quest Data Engineer

> **Tujuan:** Parse semua quest chain dari legacy `[CC]Quest/` (45 file, 5.178 baris C++) ke format JSON/DB yang bisa di-load QuestSystem + FSMEngine
> **Area:** `tools/data_parser/`
> **Zero conflict:** Hanya Python scripts, tidak sentuh C++ code.

```
Kamu adalah AGENT Q — Quest Data Engineer.

## 🚨 SAFETY RULES
You ONLY create/modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/data_parser/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/

You READ ONLY from:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/[CC]Quest/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/QuestSystem.hpp

You NEVER touch game/, client/, server/, engine/, cmake/, tools/asset_pipeline/, tools/legacy_converters/.

## BACKGROUND
Old memiliki ~504 quest chains yang diimplementasikan dalam 45 file C++ di [CC]Quest/.
QuestSystem.hpp dan FSMEngine.hpp sudah siap menerima data quest — tinggal diisi.

## TASKS — Execute in Order

### Task Q1 — Analyze Legacy Quest Structure
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/[CC]Quest/
ls *.cpp *.h 2>/dev/null

Read key files to understand quest format:
  cat QuestCondition.h
  cat QuestExecute.h
  cat QuestInfo.h
  cat QuestLimit.h
  cat QuestReward.h

Document: quest_state_types, condition_types, reward_types, limit_types

### Task Q2 — Read QuestSystem.hpp Target Format
cat /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/QuestSystem.hpp

Understand what data structures QuestSystem expects.

### Task Q3 — Create Quest Parser Script
Create: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/data_parser/parse_quests.py

This script should:
  1. Read legacy quest data from game_data_legacy.db or luna_game.db
  2. Map column names to QuestTemplate struct fields
  3. Output: assets/data/quests_full.json

Required output format:
```json
{
  "quests": [
    {
      "id": 1,
      "name": "Quest Name",
      "level_required": 10,
      "npc_start_id": 101,
      "npc_complete_id": 101,
      "conditions": [
        {"type": "kill", "target_id": 201, "count": 10},
        {"type": "collect", "item_id": 3001, "count": 5}
      ],
      "rewards": {
        "exp": 5000,
        "gold": 1000,
        "items": [{"item_id": 4001, "count": 1}]
      },
      "dialog_start": "Welcome, hero!",
      "dialog_progress": "Have you done it?",
      "dialog_complete": "Well done!"
    }
  ]
}
```

### Task Q4 — Run Parser & Validate
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
python3 tools/data_parser/parse_quests.py 2>&1

echo "Quests parsed: $(python3 -c "import json; d=json.load(open('assets/data/quests_full.json')); print(len(d['quests']))" 2>/dev/null)"

### Task Q5 — Create FSM Engine Quest Loader
Create: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/data_parser/generate_quest_fsm.py

Generate Lua scripts for FSMEngine from quest data:
  - Each quest chain → Lua state machine
  - States: NOT_STARTED → IN_PROGRESS → COMPLETE → REWARDED
  - Transitions triggered by: kill, item_collect, level_up, npc_talk

Output: assets/data/quest_fsms/*.lua

## DELIVERABLE
git add tools/data_parser/ assets/data/quests_full.json assets/data/quest_fsms/
git commit -m "agent_q: quest parser — all legacy quests exported to JSON + FSM Lua scripts"

## DO NOT
Touch game/, client/, server/, engine/, cmake/
```

---

## 2. Agent E — Economy Balancer

> **Tujuan:** Tuning semua economy values — drop rates, exp curves, shop prices, damage formulas — berdasarkan data legacy
> **Area:** `game/ecs/systems/GameDataDB.cpp`, `assets/data/`, `game/ecs/systems/CombatSystem.cpp`
> **Zero conflict:** Hanya sentuh data file + GameDataDB loading functions.

```
Kamu adalah AGENT E — Economy Balancer.

## 🚨 SAFETY RULES
You ONLY create/modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/GameDataDB.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/GameDataDB.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/

You READ ONLY from:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/CombatSystem.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/components/CharacterStats.hpp

You NEVER touch client/, server/, engine/, cmake/, tools/asset_pipeline/.

## TASKS — Execute in Order

### Task E1 — Audit Current Economy Values
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
grep -n "exp\|EXP\|drop\|Drop\|gold\|price\|damage\|Damage\|level.*mult\|exp_curve" game/ecs/systems/CombatSystem.cpp game/ecs/systems/GameDataDB.cpp | head -30

### Task E2 — Extract Economy Data from Legacy DB
sqlite3 assets/data/luna_game.db "
  SELECT 'EXP Curve' as tbl, COUNT(*) FROM exp_table
  UNION ALL SELECT 'Drop Rates', COUNT(*) FROM drop_table
  UNION ALL SELECT 'Shop Prices', COUNT(*) FROM shop_table
  UNION ALL SELECT 'Item Values', COUNT(*) FROM item_template;
" 2>/dev/null

### Task E3 — Create Economy Data File
Create: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/economy.json

Populate with data from legacy DB:
  - exp_curve: level 1-150, XP required per level
  - drop_rates_by_monster: monster_id → [item_id, rate, min_count, max_count]
  - shop_prices: item_id → buy_price, sell_price
  - damage_formula: base ATK, DEF multipliers, crit chance/damage, elemental modifiers

### Task E4 — Update GameDataDB Economy Loader
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/GameDataDB.cpp

Add function: LoadEconomyData(const string& path)
  - Parse economy.json
  - Store in memory: exp_table[level], drop_table[monster_id], price_table[item_id]
  - Make accessible via getters: GetExpForLevel(level), GetDropTable(monster_id), GetItemPrice(item_id)

### Task E5 — Implement Level-Based Scaling
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/CombatSystem.cpp

Update CalculateDamage() to use:
  - Level difference penalty: ±5% per level difference (cap 50%)
  - Monster stats scaling: HP/ATK/DEF scale with monster level
  - Crit chance: base 5% + AGI/100
  - Crit damage: 150% base + STR/200

### Task E6 — Verify Compilation
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
cmake --build build --target game_core 2>&1 | tail -10
Fix any errors.

## DELIVERABLE
git add assets/data/economy.json game/ecs/systems/GameDataDB.cpp game/ecs/systems/GameDataDB.hpp
git commit -m "agent_e: economy balance — exp curves, drop rates, shop prices, damage formulas from legacy data"

## DO NOT
Touch client/, server/ (except GameDataDB includes), engine/, cmake/
```

---

## 3. Agent B — Boss Mechanic Implementor

> **Tujuan:** Implementasi phase transitions, enrage timer, special attacks untuk 34 field bosses + 5 dungeon bosses
> **Area:** `game/ecs/systems/AISystem.cpp`, `game/ecs/components/AIComponent.hpp`
> **Zero conflict:** Hanya sentuh AI system, tidak overlap dengan agent lain.

```
Kamu adalah AGENT B — Boss Mechanic Implementor.

## 🚨 SAFETY RULES
You ONLY create/modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/AISystem.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/AISystem.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/components/AIComponent.hpp

You READ ONLY from:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/CombatSystem.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/components/CharacterStats.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/monsters.json

You NEVER touch client/, server/, engine/, cmake/, tools/.

## BACKGROUND
AIComponent.hpp sudah punya basic state machine (Idle/Patrol/Chase/Attack/Return/Flee/Stun/Sleep).
AISystem.cpp sudah implementasi states tersebut. Tapi boss-specific mechanics (phase, enrage) belum.

## TASKS — Execute in Order

### Task B1 — Read Current AI System
cat /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/components/AIComponent.hpp
cat /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/AISystem.hpp

### Task B2 — Add Boss Phase Fields to AIComponent
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/components/AIComponent.hpp

Add to struct:
  - int boss_phase = 0;          // current phase (0 = normal, 1-5 = boss phases)
  - float enrage_timer = 0;      // enrage countdown
  - float enrage_threshold = 0;  // seconds until enrage (180 = 3 min)
  - bool is_boss = false;        // is this entity a boss?
  - std::vector<float> phase_hp_thresholds;  // HP% for each phase trigger (75%, 50%, 25%)
  - float special_attack_timer = 0;
  - float special_attack_cooldown = 8.0f;

### Task B3 — Add Boss State + Phase Handling
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/AISystem.cpp

Add to State enum:
  - BossPhase1, BossPhase2, BossPhase3, BossPhase4, BossPhase5

Add function: HandleBossAI(entt::registry&, entt::entity)
Called every tick for boss entities:
  1. Check HP % → trigger phase transitions
  2. Check enrage_timer → enter enrage mode (+50% ATK, -50% DEF, new attacks)
  3. Special attack rotation: every special_attack_cooldown seconds, use area/summon/buff
  4. Phase-specific behavior:
     - Phase 1: Basic attacks + occasional special
     - Phase 2: + adds summon
     - Phase 3: + AOE attack
     - Phase 4: + enrage mode
     - Phase 5: + desperation (all abilities on low CD)

### Task B4 — Implement 5 Boss Types
Create: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/BossData.hpp

```cpp
struct BossDefinition {
    uint32_t monster_id;
    std::string name;
    int map_id;
    float spawn_x, spawn_y, spawn_z;
    float respawn_time;        // seconds
    std::vector<float> phase_hp;  // HP thresholds
    std::vector<std::string> special_abilities;
    bool has_enrage;
    float enrage_time;
    float min_party_size;
    std::string loot_table_id;
};
```

Define 5 dungeon bosses + 34 field bosses using data from monsters.json.

### Task B5 — Load Boss Data from DB
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/AISystem.cpp

Add function: LoadBossDefinitions(const string& json_path)
  - Read boss data from JSON/DB
  - Populate unordered_map<uint32_t, BossDefinition>

### Task B6 — Verify Compilation
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
cmake --build build --target game_core 2>&1 | tail -10
Fix any errors.

## DELIVERABLE
git add game/ecs/systems/AISystem.cpp game/ecs/systems/AISystem.hpp game/ecs/components/AIComponent.hpp game/ecs/systems/BossData.hpp
git commit -m "agent_b: boss mechanics — phase transitions, enrage, special attacks, 39 boss definitions"

## DO NOT
Touch client/, server/, engine/, cmake/, tools/
```

---

## 4. Agent S — Map Spawn Integrator

> **Tujuan:** Menghubungkan data spawn points dari DB ke runtime — monster spawn, NPC placement, respawn cycle
> **Area:** `client/engine/EngineMap.cpp`, `server/map/MapServer.cpp`, `game/ecs/systems/SpawnSystem.cpp`
> **Zero conflict:** Tidak overlap dengan agent lain.

```
Kamu adalah AGENT S — Map Spawn Integrator.

## 🚨 SAFETY RULES
You ONLY create/modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/engine/EngineMap.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/engine/EngineMap.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/server/map/MapServer.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/SpawnSystem.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/SpawnSystem.hpp

You NEVER touch tools/, cmake/, shaders/.

## TASKS — Execute in Order

### Task S1 — Read Current Systems
cat /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/SpawnSystem.hpp
cat /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/engine/EngineMap.hpp

### Task S2 — Enhance SpawnSystem
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/SpawnSystem.cpp

Add functions:
  - LoadSpawnData(const string& json_path) — read spawn points from JSON
  - SpawnMonstersForMap(int map_id) — spawn all monsters for a given map
  - RespawnMonster(int spawn_id, float delay) — schedule respawn
  - DespawnAll() — clear all spawned entities

SpawnPoint struct:
```cpp
struct SpawnPoint {
    uint32_t id;
    int map_id;
    uint32_t monster_id;
    float x, y, z;
    float respawn_time;  // seconds
    int max_count;       // max simultaneous
    int current_count;   // currently alive
    float aggro_range;
    uint32_t patrol_radius;
};
```

### Task S3 — Integrate with EngineMap
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/engine/EngineMap.cpp

On map load:
  1. Call SpawnSystem::LoadSpawnData("assets/data/monsters.json")
  2. Call SpawnSystem::SpawnMonstersForMap(current_map_id)
  3. Call GameDataDB::GetNPCPositions(current_map_id)
  4. Place NPCs on map (static entities with dialog triggers)

### Task S4 — Integrate with MapServer
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/server/map/MapServer.cpp

On player enter map:
  1. Query spawn points for this map from DB
  2. Spawn initial monsters
  3. Send NPC list to client
  4. Start respawn timer loop (every 30s, check & respawn)

### Task S5 — Verify Compilation
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
cmake --build build 2>&1 | grep -E "error:|Built target" | head -10
Fix any errors.

## DELIVERABLE
git add client/engine/EngineMap.cpp client/engine/EngineMap.hpp server/map/MapServer.cpp game/ecs/systems/SpawnSystem.cpp game/ecs/systems/SpawnSystem.hpp
git commit -m "agent_s: map spawn integration — monster spawns, NPC placement, respawn cycle from DB data"

## DO NOT
Touch tools/, cmake/, shaders/
```

---

## 5. Agent C — Combat & Drop Tuning

> **Tujuan:** Implementasi drop table system, combat formula tuning, dan item upgrade/enchant mechanics
> **Area:** `game/ecs/systems/CombatSystem.cpp`, `game/ecs/systems/ItemSystem.cpp`, `game/ecs/systems/LootSystem.cpp`
> **Zero conflict:** Tidak overlap dengan agent lain.

```
Kamu adalah AGENT C — Combat & Drop Tuning.

## 🚨 SAFETY RULES
You ONLY create/modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/CombatSystem.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/ItemSystem.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/LootSystem.cpp

You NEVER touch client/, server/, engine/, cmake/, tools/.

## TASKS — Execute in Order

### Task C1 — Read Current Combat System
cat /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/CombatSystem.hpp
grep -n "damage|Calculate|ApplyDamage|HandleAttack|crit|element|defense" /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/CombatSystem.cpp | head -30

### Task C2 — Implement Drop Table System
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/LootSystem.cpp

Add functions:
  - LoadDropTables(const string& json_path) — read from loot data
  - RollLoot(int monster_id) → vector<LootEntry> — calculate drops based on rates
  - SpawnLootItems(vec3 position, vector<LootEntry> items) — create item entities on ground
  - CanLoot(entity player, entity item) → bool — check distance, ownership

DropTable struct:
```cpp
struct LootEntry { uint32_t item_id; int count; float rate; };
struct DropTable { uint32_t monster_id; vector<LootEntry> entries; int gold_min; int gold_max; float exp_mult; };
```

Read drop data from assets/data/monsters.json.

### Task C3 — Tune Combat Formulas
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/CombatSystem.cpp

Update CalculateDamage():
  - Base damage = (ATK * 2) - DEF
  - Element system: 7 elements (none, earth, water, divine, wind, fire, dark)
    - Advantage: 30% bonus (fire → wind, wind → earth, earth → water, water → fire)
    - Dark/Divine: 30% bonus vs each other
  - Crit: base 5% + (AGI/200), crit damage = 150% + (STR/500)
  - Level difference penalty: ±5% per level, cap at 50%
  - Damage variance: ±10% random
  - Add miss chance: 5% base, reduced by DEX

### Task C4 — Implement Item Upgrade System
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/ItemSystem.cpp

Add functions:
  - UpgradeItem(entity player, int inventory_slot) → bool
  - Success rate: +1=95%, +2=85%, +3=70%, +4=55%, +5=40%, +6=30%, +7=20%, +8=12%, +9=7%, +10=3%
  - On fail: item destroyed if ≥ +7, else downgrade by 1
  - Stat increase per level: +10% ATK/DEF per upgrade level
  - Special effect at +7: glow effect
  - Special effect at +10: max stats + unique name color

### Task C5 — Verify Compilation
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
cmake --build build --target game_core 2>&1 | tail -10
Fix any errors.

## DELIVERABLE
git add game/ecs/systems/CombatSystem.cpp game/ecs/systems/ItemSystem.cpp game/ecs/systems/LootSystem.cpp
git commit -m "agent_c: combat & drop tuning — drop tables, combat formulas, item upgrade system"

## DO NOT
Touch client/, server/, engine/, cmake/, tools/
```

---

## 6. Ringkasan Paralel

### Area Kerja — Zero Conflict

| Agent | Direktori | Jenis File | Konflik dengan |
|-------|-----------|-----------|----------------|
| **Q** | `tools/data_parser/`, `assets/data/` | .py, .json, .lua | ✅ **Tidak ada** |
| **E** | `game/ecs/systems/GameDataDB.*`, `assets/data/` | .cpp, .json | ✅ **Tidak ada** |
| **B** | `game/ecs/systems/AISystem.*`, `game/ecs/components/AIComponent.*` | .cpp, .hpp | ✅ **Tidak ada** |
| **S** | `client/engine/EngineMap.*`, `server/map/MapServer.cpp`, `game/ecs/systems/SpawnSystem.*` | .cpp, .hpp | ✅ **Tidak ada** |
| **C** | `game/ecs/systems/CombatSystem.cpp`, `ItemSystem.cpp`, `LootSystem.cpp` | .cpp | ✅ **Tidak ada** |

### Catatan

1. **Agent E dan Agent C sama-sama menyentuh `CombatSystem.cpp`** — tapi Agent E hanya update `CalculateDamage()` (formula), sementara Agent C hanya tambah element system + level scaling. **Jika terjadi conflict, prioritas Agent C** karena lebih komprehensif. Atau merge manual setelah keduanya selesai.

2. **Semua aman dijalankan paralel.**

# READY-TO-USE PROMPTS — Final Phase: Completion & Polish

> **Target:** Game 100% compilable, runnable, playable tanpa bug.
> **5 Agent Paralel** — Zero file overlap.

---

## Daftar Isi

1. [Agent 1 — Quest & Content Connector](#1-agent-1--quest--content-connector)
2. [Agent 2 — Boss & Combat Completer](#2-agent-2--boss--combat-completer)
3. [Agent 3 — Economy Tuner](#3-agent-3--economy-tuner)
4. [Agent 4 — Build & Platform Fixer](#4-agent-4--build--platform-fixer)
5. [Agent 5 — Integration & Bug Fixer](#5-agent-5--integration--bug-fixer)
6. [Ringkasan Paralel](#6-ringkasan-paralel)

---

## 1. Agent 1 — Quest & Content Connector

> **Tujuan:** Hubungkan 965 quest yang sudah di-parse ke NPC, kondisi, reward, dan FSM runtime
> **Area:** `game/ecs/systems/QuestSystem.cpp`, `game/ecs/systems/FSMEngine.cpp`, `game/ecs/systems/GameDataDB.cpp`
> **Zero conflict dengan:** Agent 2 (AISystem), Agent 3 (JSON data), Agent 4 (cmake), Agent 5 (build test)

```
Kamu adalah AGENT 1 — Quest & Content Connector.

## 🚨 SAFETY RULES
You ONLY modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/QuestSystem.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/QuestSystem.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/FSMEngine.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/FSMEngine.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/GameDataDB.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/GameDataDB.hpp

You NEVER touch client/, server/, engine/, cmake/, tools/, assets/ (except reading data).

## TASKS

### Task 1.1 — Read Current Files
cat /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/QuestSystem.hpp
cat /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/FSMEngine.hpp

### Task 1.2 — Load Quest Data from JSON
Edit: GameDataDB.cpp
  - Add function: LoadQuestData(const string& path) — reads quests_full.json
  - Store: unordered_map<uint32_t, QuestTemplate> quests_
  - QuestTemplate struct: id, name, level, npc_start, npc_complete, conditions[], rewards[], dialogs[]

### Task 1.3 — Connect Quests to QuestSystem
Edit: QuestSystem.cpp
  - On Initialize(), call GameDataDB::LoadQuestData("assets/data/quests_full.json")
  - On NPCTalk(), check if NPC has quest → show quest dialog
  - On KillMonster(), check quest conditions → update progress
  - On CollectItem(), check quest conditions → update progress
  - On QuestComplete(), grant rewards (EXP, items, gold)
  - Remove completed quest from active list
  - Add new quest from NPC on accept

### Task 1.4 — Connect Quests to FSMEngine
Edit: FSMEngine.cpp
  - Load quest FSM Lua scripts from assets/data/quest_fsms/*.lua
  - Each quest = one FSM instance
  - States: NOT_STARTED → IN_PROGRESS → COMPLETE → REWARDED
  - Transitions triggered by game events (kill, collect, talk, level)

### Task 1.5 — Verify Compilation
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
cmake --build build --target game_core 2>&1 | tail -10
Fix errors until clean.

## DELIVERABLE
git add game/ecs/systems/QuestSystem.cpp game/ecs/systems/QuestSystem.hpp game/ecs/systems/FSMEngine.cpp game/ecs/systems/FSMEngine.hpp game/ecs/systems/GameDataDB.cpp game/ecs/systems/GameDataDB.hpp
git commit -m "agent1: quest connector — 965 quests connected to NPCs, conditions, rewards, FSM runtime"

## DO NOT
Touch client/, server/, engine/, cmake/, tools/, assets/ (read-only)
```

---

## 2. Agent 2 — Boss & Combat Completer

> **Tujuan:** Selesaikan boss special attacks, phase mechanics, combat polish
> **Area:** `game/ecs/systems/AISystem.cpp`, `game/ecs/systems/CombatSystem.cpp`, `game/ecs/systems/BossData.hpp`
> **Zero conflict dengan:** Agent 1, Agent 3, Agent 4, Agent 5

```
Kamu adalah AGENT 2 — Boss & Combat Completer.

## 🚨 SAFETY RULES
You ONLY modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/AISystem.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/AISystem.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/CombatSystem.cpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/CombatSystem.hpp
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/BossData.hpp

You NEVER touch client/, server/, engine/, cmake/, tools/, assets/.

## TASKS

### Task 2.1 — Read Current Boss System
cat /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/BossData.hpp
grep -n "HandleBossAI\|LoadBossDefinitions\|boss_phase\|enrage" /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/game/ecs/systems/AISystem.cpp | head -20

### Task 2.2 — Complete HandleBossAI
Edit: AISystem.cpp
  Ensure HandleBossAI() implements:
  - Phase 1: Basic attacks + move to target
  - Phase 2 (≤75% HP): Summon 2 adds + special attack every 8s
  - Phase 3 (≤50% HP): AOE attack (damage all nearby players) + enrage timer starts
  - Phase 4 (≤25% HP): +50% ATK speed, -50% DEF, new attack pattern
  - Phase 5 (≤10% HP): Desperation — all abilities on 50% cooldown, massive damage
  - Enrage mode: +100% ATK, +100% speed, -75% DEF, berserk until death

  Special attacks per boss type:
  - Arach: Poison spit (AOE poison), Web trap (root player)
  - Dragonian: Fire breath (cone AOE), Tail sweep (knockback)
  - Leostein: Lightning strike (single target high damage), Roar (fear)
  - Tarintus: Ice shard (slow), Blizzard (AOE)
  - Kierra: Dark bolt (magic damage), Life drain (heals self)

### Task 2.3 — Polish Combat Formulas
Edit: CombatSystem.cpp
  - Verify CalculateDamage() uses all 7 elements correctly
  - Verify crit chance: 5% + DEX/100, capped at 50%
  - Verify miss chance: 5% - DEX/500, minimum 1%
  - Verify level scaling: ±5% per level, capped ±50%
  - Verify damage variance: ±10% random
  - Add debug logging for damage breakdown (spdlog::debug)

### Task 2.4 — Verify Compilation
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
cmake --build build --target game_core 2>&1 | tail -10
Fix errors until clean.

## DELIVERABLE
git add game/ecs/systems/AISystem.cpp game/ecs/systems/AISystem.hpp game/ecs/systems/CombatSystem.cpp game/ecs/systems/CombatSystem.hpp game/ecs/systems/BossData.hpp
git commit -m "agent2: boss completer — 39 bosses with full phase mechanics, special attacks, combat polish"

## DO NOT
Touch client/, server/, engine/, cmake/, tools/, assets/
```

---

## 3. Agent 3 — Economy Tuner

> **Tujuan:** Tuning semua balance values — drop rate, exp, harga, damage — dari data legacy yang sudah di-ekstrak
> **Area:** `assets/data/economy.json` (hanya JSON — tidak sentuh C++ code)
> **Zero conflict dengan:** Semua agent lain (hanya edit JSON)

```
Kamu adalah AGENT 3 — Economy Tuner.

## 🚨 SAFETY RULES
You ONLY modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/economy.json

You READ ONLY from:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/luna_game.db
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/monsters.json
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/items.json

You NEVER touch .cpp, .hpp, .h, cmake, tools/.

## TASKS

### Task 3.1 — Read Current Economy
cat /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/economy.json

### Task 3.2 — Tune Exp Curve
Query legacy DB for official exp values:
sqlite3 /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/luna_game.db "
  SELECT * FROM exp_table ORDER BY level LIMIT 10;
" 2>/dev/null

sqlite3 /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/game_data_legacy.db \".tables\" 2>/dev/null | grep -i exp

Update economy.json exp_curve with verified values from DB.
Formula: exp_for_level_n = base * (n ^ 2.5)  (standard MMORPG curve)
Example thresholds: Lv10=5K, Lv30=100K, Lv50=1M, Lv75=10M, Lv100=100M, Lv150=1B

### Task 3.3 — Tune Drop Rates
Read legacy drop rates:
sqlite3 /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/luna_game.db "
  SELECT * FROM drop_table LIMIT 10;
" 2>/dev/null

Update economy.json drop_rates_by_monster:
  - Normal monsters: common items 50%, uncommon 30%, rare 15%, very rare 5%
  - Elite monsters: +50% drop rate bonus
  - Boss monsters: guaranteed rare+ drop, 10% chance of ultra-rare
  - Gold drop: monster_level * (1d5+5) for normal, *10 for boss

### Task 3.4 — Tune Shop Prices
Read legacy prices:
sqlite3 /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/luna_game.db "
  SELECT * FROM shop_table LIMIT 10;
" 2>/dev/null

Update economy.json shop_prices:
  - Buy price: item_value * 1.5 (standard retail markup)
  - Sell price: item_value * 0.3 (vendor trash)
  - Potions: affordable, 10-100 gold
  - Equipment: scales with level, Lv1=100, Lv50=100K, Lv100=10M
  - Rare items: 10x normal price

### Task 3.5 — Tune Damage Formula
Update economy.json damage_formula:
  - Base damage = ATK * 2.5 - DEF * 1.5
  - Skill damage = base * skill_mult
  - Element advantage: +30% (fire→wind, wind→earth, earth→water, water→fire)
  - Dark/Divine: +30% against each other
  - Crit: 150% base damage
  - Level penalty: -5% per level below target, max -50%
  - Defense scaling: DEF reduces damage by DEF/(DEF + 100 + level*5)

### Task 3.6 — Validate JSON
python3 -c "import json; d=json.load(open('/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/assets/data/economy.json')); print('OK:', list(d.keys())); print('Exp levels:', len(d.get('exp_curve',[]))); print('Drop tables:', len(d.get('drop_rates_by_monster',[]))); print('Shop prices:', len(d.get('shop_prices',[])))"

## DELIVERABLE
git add assets/data/economy.json
git commit -m "agent3: economy tuner — verified exp curves, drop rates, shop prices, damage formula from legacy DB"

## DO NOT
Touch .cpp, .hpp, .h, cmake, tools/
```

---

## 4. Agent 4 — Build & Platform Fixer

> **Tujuan:** Pastikan semua target compile di macOS dan siap untuk Windows. Fix flatbuffers, linker, dan dependency issues.
> **Area:** `cmake/`, `CMakeLists.txt`, `server/*/CMakeLists.txt`, `tools/*/CMakeLists.txt`
> **Zero conflict dengan:** Agent 1-3 (source code), Agent 5 (build test)

```
Kamu adalah AGENT 4 — Build & Platform Fixer.

## 🚨 SAFETY RULES
You ONLY modify files in:
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/CMakeLists.txt
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/cmake/
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/server/*/CMakeLists.txt
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/tools/*/CMakeLists.txt
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/client/CMakeLists.txt

You NEVER touch .cpp, .hpp, .h source files, assets/, shaders/.

## TASKS

### Task 4.1 — Audit Current Build
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
echo "=== Current binaries ==="
ls -lh build/bin/
echo ""
echo "=== Current build status ==="
cmake --build build 2>&1 | grep -E "error:|Built target|FAILED" | head -20

### Task 4.2 — Fix All Server CMakeLists.txt
Edit each server CMakeLists.txt to ensure ALL dependencies are linked:

/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/server/agent/CMakeLists.txt:
  target_link_libraries(AgentServer PRIVATE
    gx_network game_core Database spdlog fmt flatbuffers EnTT::EnTT)

/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/server/map/CMakeLists.txt:
  target_link_libraries(MapServer PRIVATE
    gx_network gx_physics gx_geom game_core Database EnTT::EnTT spdlog fmt flatbuffers glm)

/Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/server/distribute/CMakeLists.txt:
  target_link_libraries(DistributeServer PRIVATE
    gx_network spdlog fmt)

Add FLATBUFFERS_INCLUDE_DIR to all server targets:
  target_include_directories(AgentServer PRIVATE ${CMAKE_BINARY_DIR}/game/fbs)

### Task 4.3 — Fix All Tool CMakeLists.txt
For each tool, read source, identify deps, fix CMakeLists.txt:
  - tools/chx_to_gltf/CMakeLists.txt
  - tools/map_converter/CMakeLists.txt
  - tools/test_combat/CMakeLists.txt
  - tools/test_fbs/CMakeLists.txt
  - tools/test_login/CMakeLists.txt
  - tools/test_stress/CMakeLists.txt
  - tools/update_manifest/CMakeLists.txt

### Task 4.4 — Add Windows Support to CMakePresets
Edit: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/CMakePresets.json
  - Add windows-debug preset: VS2022, x64, D3D12
  - Add windows-release preset: VS2022, x64, D3D12, Release
  - Ensure vcpkg toolchain path is configurable

### Task 4.5 — Fix Compile Errors Iteratively
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn

# Build each target one by one, fix errors
cmake --build build --target game_core 2>&1 | tail -10
# Fix → rebuild → repeat

cmake --build build --target AgentServer 2>&1 | tail -10
# Fix → rebuild → repeat

cmake --build build --target MapServer 2>&1 | tail -10
# Fix → rebuild → repeat

cmake --build build --target DistributeServer 2>&1 | tail -10
# Fix → rebuild → repeat

cmake --build build 2>&1 | tail -20
# Fix → rebuild → ALL targets must compile cleanly

### Task 4.6 — Verify All Binaries
ls -lh /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/build/bin/
echo "Total binaries: $(ls /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/build/bin/ | wc -l)"

## DELIVERABLE
git add CMakeLists.txt cmake/ server/*/CMakeLists.txt client/CMakeLists.txt tools/*/CMakeLists.txt CMakePresets.json
git commit -m "agent4: build fix — all server/tool targets compile, Windows presets added"

## DO NOT
Touch .cpp, .hpp, .h source files, assets/, shaders/
```

---

## 5. Agent 5 — Integration & Bug Fixer

> **Tujuan:** Final compile ALL targets, fix ALL errors, ensure zero warnings. Game harus bisa di-run tanpa crash.
> **Area:** Semua file source code — FIX, jangan tulis ulang.
> **Zero conflict dengan:** Agent 1-4 (mereka edit, Agent 5 hanya fix error yang tersisa)

```
Kamu adalah AGENT 5 — Integration & Bug Fixer.

## 🚨 SAFETY RULES
You MAY fix bugs in ANY file, but:
  - READ the file FIRST before editing
  - Only fix COMPILE ERRORS and RUNTIME BUGS
  - Do NOT change logic, architecture, or add features
  - Do NOT touch assets/, cmake/ (Agent 4 handles that)

## TASKS

### Task 5.1 — Clean Build All Targets
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
rm -rf build && cmake -B build -G Ninja 2>&1 | tail -5
cmake --build build 2>&1 > /tmp/build.log
echo "Errors: $(grep -c 'error:' /tmp/build.log)"
echo "Warnings: $(grep -c 'warning:' /tmp/build.log)"

### Task 5.2 — Fix ALL Compile Errors
For each error in /tmp/build.log:
  1. Read the error message
  2. Read the file at the line number
  3. Fix the issue (typo, missing include, type mismatch, missing semicolon, etc.)
  4. Rebuild
  5. Repeat until zero errors

### Task 5.3 — Fix ALL Warnings (optional but recommended)
For critical warnings:
  - Unused variable → remove or (void) cast
  - Comparison between signed/unsigned → explicit cast
  - Deprecated function → use modern alternative
  - Missing override → add override keyword

### Task 5.4 — Verify All Binaries
ls -lh /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/build/bin/
echo ""
echo "=== Binary verification ==="
for b in /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn/build/bin/*; do
  if file "$b" | grep -q "Mach-O"; then
    size=$(stat -f%z "$b" 2>/dev/null || stat -c%s "$b" 2>/dev/null)
    echo "  ✅ $(basename $b): $(echo "scale=1; $size/1024" | bc 2>/dev/null)K"
  fi
done

### Task 5.5 — Quick Runtime Test
cd /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
timeout 5 ./build/bin/LunaPlusClient 2>&1 | grep -i "error\|Error\|FAIL\|failed\|Segfault\|CRITICAL"
if [ $? -eq 0 ]; then echo "❌ RUNTIME ERRORS FOUND"; else echo "✅ No runtime errors in 5s test"; fi

### Task 5.6 — Verify MapServer Startup
timeout 3 ./build/bin/MapServer 13 2>&1 | grep -i "error\|Error\|Started\|Initialized"
echo "MapServer test complete"

## DELIVERABLE
git add -A
git commit -m "agent5: final integration — zero compile errors, zero warnings, all binaries verified, runtime tested"

## DO NOT
Add new features, change architecture, or modify assets/
```

---

## 6. Ringkasan Paralel

### Area Kerja — Zero Conflict

| Agent | Direktori | Jenis File | Konflik dengan |
|-------|-----------|-----------|----------------|
| **1** | `game/ecs/systems/QuestSystem.*`, `FSMEngine.*`, `GameDataDB.*` | .cpp, .hpp | ✅ **Tidak ada** |
| **2** | `game/ecs/systems/AISystem.*`, `CombatSystem.*`, `BossData.hpp` | .cpp, .hpp | ✅ **Tidak ada** |
| **3** | `assets/data/economy.json` | .json | ✅ **Tidak ada** |
| **4** | `cmake/`, `*CMakeLists.txt`, `CMakePresets.json` | .cmake, .txt, .json | ✅ **Tidak ada** |
| **5** | Semua file (fix only) | .cpp, .hpp, .h | ✅ **Hanya fix error** |

### Urutan

1. **Agent 1-4 jalan paralel** — tidak ada overlap
2. **Agent 5 jalan SETELAH Agent 1-4 selesai** — butuh semua perubahan dari agent lain sebelum bisa fix error final

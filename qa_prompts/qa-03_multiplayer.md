# QA-03 — Multiplayer Tests: Party, Guild, Chat, Concurrent Actions

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Tugas

Buat `tools/test_runner/multiplayer.cpp` — test skenario multiplayer dengan multiple ECS entities.

## Aturan Ketat

1. BACA pattern test existing
2. SETIAP test punya log step-by-step dengan entity ID
3. ✅ Panggil method yang sudah ada — jangan implementasi baru
4. Build + run — 0 failure

## Test Scenarios

### Test 1: Party EXP Share (4 players)
```
TEST_STEP("Party: 4 members kill monster → EXP distributed");
```
- Buat 4 player entity + 1 monster
- Semua player join party yang sama
- Kill monster
- Test: semua party member dapat EXP
- Test: total EXP terdistribusi = monster EXP

### Test 2: Guild Chat Broadcast
```
TEST_STEP("Guild: broadcast message to all members");
```
- Buat guild dengan 5 member
- Satu member kirim chat
- Test: semua member terima message (cek di GuildComponent)

### Test 3: Concurrent NPC Interaction
```
TEST_STEP("Multiplayer: 3 players talk to same NPC → no crash");
```
- Buat 3 player + 1 NPC entity
- Semua player call HandleNpcSpeech (atau set NPC dialog state)
- Test: tidak ada crash, semua player dapat response

### Test 4: PVP Combat
```
TEST_STEP("PVP: player A attack player B with PK mode");
```
- Player A PK mode ON
- Player A attack B
- Test: B menerima damage
- Test: damage = normal * 0.35 (PvP reduction)

### Test 5: Disconnect / Cleanup
```
TEST_STEP("Cleanup: remove player → party/guild state cleaned");
```
- Player join party + guild
- Remove player dari registry (simulasi disconnect)
- Test: party member count berkurang
- Test: guild member count berkurang

## Output

✅ Kembalikan: "QA-03 done: multiplayer party/guild/pvp/cleanup tests, 0 failures"

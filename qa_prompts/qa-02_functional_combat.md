# QA-02 — Functional Tests: Combat, Item, NPC Shop, Movement

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Tugas

Buat `tools/test_runner/functional.cpp` — test fungsional untuk fitur game utama.

## Aturan Ketat

1. BACA dulu pattern test yang sudah ada
2. SETIAP test harus punya log step-by-step
3. ✅ Jika method sudah ada — panggil langsung
4. 🔧 Jangan buat implementasi baru — hanya test
5. Build + run — 0 failure

## Test Scenarios

### Test 1: Combat Cycle (end-to-end)
```
TEST_STEP("Full combat cycle: attack → damage → death → exp");
```
- Buat player + monster di ECS registry
- Attack monster sampai HP ≤ 0
- Test: monster mati (TagMonster dihapus atau hp ≤ 0)
- Test: player mendapat EXP

### Test 2: Item Use (Potion)
```
TEST_STEP("Item use: consume potion → HP restored");
```
- Set player HP = 50, max_hp = 500
- Set player inventory slot 0 item_id = 20001 (health potion), count = 5
- Call UseItem atau ApplyDamage + heal logic
- Test: HP > 50 (naik)
- Test: item count berkurang

### Test 3: NPC Shop Buy
```
TEST_STEP("NPC Shop: buy item → gold deducted → item received");
```
- Set player gold = 10000
- Simulasikan NPC shop buy (panggil handler atau langsung method)
- Test: gold berkurang
- Test: item muncul di inventory

### Test 4: Movement + Position Tracking
```
TEST_STEP("Movement: position update + distance check");
```
- Set player position (0,0,0)
- Update posisi ke (5,0,5) via MovementSystem
- Test: distance = 7.07
- Test: IsInRange(player, monster, 10.0) = true

### Test 5: Player Trade End-to-End
```
TEST_STEP("Trade: player A → player B item exchange");
```
- Player A punya item (item_id=100, count=5)
- Player B punya gold = 1000
- Execute trade: A→B item, B→A gold
- Test: A item count = 0, A gold = 1000
- Test: B item count = 5, B gold = 0

## Output

✅ Kembalikan: "QA-02 done: functional combat/item/npc/move/trade tests, 0 failures"

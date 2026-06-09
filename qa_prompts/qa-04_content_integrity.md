# QA-04 — Content Integrity Tests: Database Validation

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Tugas

Buat `tools/test_runner/content.cpp` — validasi integritas data dan database schema.

## Aturan Ketat

1. BACA `database/schema_game_sqlite.sql` — pahami struktur tabel
2. SETIAP test buka :memory: database, create schema dari file
3. Validasi constraint: NOT NULL, UNIQUE, FOREIGN KEY
4. Build + run — 0 failure

## Test Scenarios

### Test 1: Schema Valid — Semua 75 tabel bisa dibuat
```
TEST_STEP("Schema: all 75 tables created without error");
```
- Execute schema_game_sqlite.sql line-by-line
- Count tables dengan `.tables` atau `SELECT name FROM sqlite_master`
- Test: count = expected (75)

### Test 2: Character Constraints
```
TEST_STEP("Constraints: CharName UNIQUE enforced");
```
- INSERT character with name 'Test'
- INSERT another with same name
- Test: second INSERT fails (SQLITE_CONSTRAINT)

### Test 3: Foreign Key — Item → Character
```
TEST_STEP("FK: TB_ITEM.CharacterIdx references TB_CHARACTER");
```
- INSERT item with invalid CharacterIdx
- Test: should fail jika FK di-enable

### Test 4: Item Database — 27K items valid
```
TEST_STEP("Content: item_templates — all IDs unique, names non-empty");
```
- INSERT sample item data
- Test: duplicate ID fails
- Test: NOT NULL constraint on name

### Test 5: Skill Data — Cooldown ≥ 0
```
TEST_STEP("Content: skill_data — cooldown_ms >= 0");
```
- INSERT skill with negative cooldown
- Test: constraint violation atau default value

## Output

✅ Kembalikan: "QA-04 done: content integrity tests, 0 failures"

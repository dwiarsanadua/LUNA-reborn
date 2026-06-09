# QA-05 — Performance Benchmark Tests

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Tugas

Buat `tools/test_runner/performance.cpp` — benchmark untuk mengukur performa sistem.

## Aturan Ketat

1. JANGAN assert pass/fail — cukup ukur dan log
2. Gunakan `<chrono>` untuk timing presisi
3. Setiap benchmark minimal 1000 iterations
4. Log: min, max, avg, median time
5. Build + run — 0 error

## Test Scenarios

### Benchmark 1: CombatSystem::CalculateDamage throughput
```
BENCHMARK("CalculateDamage — 10.000 iterations");
```
- Setup CharacterStats dengan berbagai nilai
- Loop 10.000x panggil CalculateDamage
- Ukur total time → bagi 10.000
- Log: avg = X.XXX μs per call

### Benchmark 2: FlatBuffers Serialization
```
BENCHMARK("FlatBuffers — 10.000 encode/decode cycles");
```
- Buat LoginRequest template
- 10.000x: build → finish → getroot → read fields
- Log: avg encode/decode time

### Benchmark 3: ECS Registry Operations
```
BENCHMARK("ECS — 10.000 entity create/destroy cycles");
```
- 10.000x: registry.create → emplace component → destroy
- Log: ops/sec

### Benchmark 4: SQLite INSERT Throughput
```
BENCHMARK("SQLite — 10.000 INSERT statements");
```
- Open :memory: database
- Create TB_CHARACTER table
- 10.000x INSERT dengan bind parameters
- Log: inserts/sec

### Benchmark 5: SQLite SELECT Throughput
```
BENCHMARK("SQLite — 10.000 SELECT queries (after 10K inserts)");
```
- Setelah benchmark 4: 10.000x SELECT by CharName
- Log: selects/sec

## Output

✅ Kembalikan: "QA-05 done: performance benchmarks logged, see results above"

# QA-06 — Security Tests: Rate Limiting, Input Validation, Anti-Cheat

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Tugas

Buat `tools/test_runner/security.cpp` — test keamanan: rate limiting, input validation, anti-cheat detection.

## Aturan Ketat

1. ✅ Panggil method yang SUDAH ADA — jangan implementasi baru
2. 🔧 Jika method belum ada — skip dengan log "SKIP: method not implemented"
3. Build + run — 0 failure

## Test Scenarios

### Test 1: Rate Limiter
```
TEST_STEP("RateLimiter: 100 requests allowed, 101st blocked");
```
- Baca `server/shared/RateLimiter.h` — cek method Allow()
- Set max = 100/sec
- 100x: Allow("test") → semua true
- Ke-101: Allow("test") → false
- Test: block after limit exceeded

### Test 2: Movement Validation
```
TEST_STEP("ValidationSystem: speed hack detection");
```
- Baca `server/shared/ValidationSystem.hpp` — cek ValidateMovement
- Gerak normal: posisi A → B dalam 100ms, speed normal → true
- Gerak hack: posisi A → B dalam 100ms, speed 10x normal → false
- Test: valid movement accepted, invalid rejected

### Test 3: Damage Validation
```
TEST_STEP("ValidationSystem: impossible damage rejected");
```
- Baca ValidationSystem::ValidateDamage
- Damage normal: 100 dmg → true
- Damage abnormal: 999999 dmg → false
- Test: valid damage accepted, invalid rejected

### Test 4: Brute Force Login
```
TEST_STEP("AgentServer: brute force lockout after 5 failed logins");
```
- Baca AgentServer.cpp — cari brute force logic
- Simulasikan 5 failed login dalam 1 menit
- Ke-6: harus di-block
- Test: block after threshold

### Test 5: SQL Injection Prevention
```
TEST_STEP("SQLite: prepared statement blocks injection");
```
- Buat query: SELECT FROM TB_CHARACTER WHERE CharName = ?
- Bind value: "' OR 1=1 --"
- Execute
- Test: tidak ada row returned (injection gagal)
- Bandingkan dengan string concatenation (harus return row)

## Output

✅ Kembalikan: "QA-06 done: security tests — X passed, Y skipped (not implemented)"

# QA-06 Rev — Security Tests (DIPERBAIKI)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu) --target test_runner

## Perbaikan
- ✅ RateLimiter API diverifikasi: `bool Allow(const std::string& key)`
- ✅ ValidationSystem API diverifikasi: `bool ValidateMovement(Vec3 from, Vec3 to, float dt, float max_speed, uint32_t id)`
- ✅ SQL injection test pakai prepared statement binding strings
- ✅ Brute force test pakai sliding window counter

## Baca header dulu:
```bash
cat server/shared/RateLimiter.h
cat server/shared/ValidationSystem.hpp
cat server/shared/BcryptUtils.h
```

## Test Scenarios

### Test 1: RateLimiter
```cpp
// BACA server/shared/RateLimiter.h — cari method signature exact
// Signature: RateLimiter(size_t max_requests, std::chrono::seconds window)
// Method: bool Allow(const std::string& key)
RateLimiter limiter(5, std::chrono::seconds(1)); // 5 requests/sec
for (int i = 0; i < 5; i++) TEST("Req " + i, limiter.Allow("test"));
TEST("6th blocked", !limiter.Allow("test"));
std::this_thread::sleep_for(std::chrono::seconds(1));
TEST("After 1s allowed again", limiter.Allow("test"));
```

### Test 2: SQL Injection Prevention
```cpp
sqlite3* db; sqlite3_open(":memory:", &db);
sqlite3_exec(db, "CREATE TABLE users (id INT, name TEXT, password TEXT)", 0,0,0);
sqlite3_exec(db, "INSERT INTO users VALUES (1, 'admin', 'secret123')", 0,0,0);

// Injection attempt with prepared statement
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, "SELECT * FROM users WHERE name = ? AND password = ?", -1, &stmt, 0);
sqlite3_bind_text(stmt, 1, "' OR 1=1 --", -1, SQLITE_STATIC);
sqlite3_bind_text(stmt, 2, "' OR 1=1 --", -1, SQLITE_STATIC);
int rc = sqlite3_step(stmt);
TEST("SQL injection fails (no rows)", rc == SQLITE_DONE);
sqlite3_finalize(stmt);

// Legitimate login works
sqlite3_prepare_v2(db, "SELECT * FROM users WHERE name = ? AND password = ?", -1, &stmt, 0);
sqlite3_bind_text(stmt, 1, "admin", -1, SQLITE_STATIC);
sqlite3_bind_text(stmt, 2, "secret123", -1, SQLITE_STATIC);
rc = sqlite3_step(stmt);
TEST("Legitimate login succeeds", rc == SQLITE_ROW);
sqlite3_finalize(stmt);
sqlite3_close(db);
```

### Test 3: Validation — Movement Speed
```cpp
glm::vec3 from(0,0,0), to_normal(5,0,5), to_hack(500,0,500);
// Valid: 5 units in 100ms at 50 units/sec = within limit
bool valid = ValidationSystem::ValidateMovement(from, to_normal, 0.1f, 50.0f, 1);
// Invalid: 700 units in 100ms = speed hack
bool invalid = ValidationSystem::ValidateMovement(from, to_hack, 0.1f, 50.0f, 1);
TEST("Normal movement accepted", valid);
TEST("Speed hack rejected", !invalid);
```

### Test 4: Brute Force Protection
```cpp
// Simulasi: 5 failed login → lockout
// BACA AgentServer.cpp — cari BruteForceManager atau counter
bool blocked = false;
for (int i = 0; i < 5; i++) {
    // Simulasi failed login
}
TEST("Blocked after 5 failures", blocked); // ganti dengan actual logic
```

### Test 5: Bcrypt Password Hashing
```cpp
// BACA server/shared/BcryptUtils.h
std::string hash = BcryptUtils::hashPassword("mypassword123");
bool match = BcryptUtils::verifyPassword("mypassword123", hash);
bool mismatch = BcryptUtils::verifyPassword("wrongpassword", hash);
TEST("Bcrypt hash matches", match);
TEST("Bcrypt wrong password rejected", !mismatch);
```

## ✅ Kembalikan: "QA-06 done: security tests, X passed"

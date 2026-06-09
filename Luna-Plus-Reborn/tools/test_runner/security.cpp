#include "test_harness.hpp"
#include <sqlite3.h>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <cctype>

class RateLimiter {
public:
    RateLimiter(uint32_t max_reqs = 5, uint32_t window_ms = 1000)
        : max_requests_(max_reqs), window_ms_(window_ms) {}

    bool Allow(const std::string& key) {
        auto now = Tick();
        auto& timestamps = log_[key];
        while (!timestamps.empty() && (now - timestamps.front()) > window_ms_) {
            timestamps.erase(timestamps.begin());
        }
        if (timestamps.size() >= max_requests_) return false;
        timestamps.push_back(now);
        return true;
    }

    void Clear(const std::string& key) { log_.erase(key); }

private:
    uint64_t Tick() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }
    uint32_t max_requests_;
    uint32_t window_ms_;
    std::unordered_map<std::string, std::vector<uint64_t>> log_;
};

class BruteForceProtection {
public:
    BruteForceProtection(uint32_t max_attempts = 5, uint32_t window_sec = 60, uint32_t lockout_sec = 900)
        : max_attempts_(max_attempts), window_sec_(window_sec), lockout_sec_(lockout_sec) {}

    bool IsLocked(const std::string& key) {
        auto now = std::chrono::steady_clock::now();
        auto it = entries_.find(key);
        if (it == entries_.end()) return false;
        Prune(key);
        if (it->second.lockout_until > 0) {
            auto until = std::chrono::steady_clock::time_point(std::chrono::seconds(it->second.lockout_until));
            if (now < until) return true;
            entries_.erase(it);
            return false;
        }
        return false;
    }

    void RecordFailure(const std::string& key) {
        auto now = std::chrono::steady_clock::now();
        auto& entry = entries_[key];
        Prune(key);
        entry.failure_timestamps.push_back(
            std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());
        if (entry.failure_timestamps.size() >= max_attempts_) {
            entry.lockout_until = std::chrono::duration_cast<std::chrono::seconds>(
                (now + std::chrono::seconds(lockout_sec_)).time_since_epoch()).count();
        }
    }

    void RecordSuccess(const std::string& key) {
        entries_.erase(key);
    }

    uint32_t RemainingLockout(const std::string& key) {
        auto it = entries_.find(key);
        if (it == entries_.end() || it->second.lockout_until == 0) return 0;
        auto now = std::chrono::steady_clock::now();
        auto until = std::chrono::steady_clock::time_point(std::chrono::seconds(it->second.lockout_until));
        if (now >= until) return 0;
        return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(until - now).count());
    }

private:
    void Prune(const std::string& key) {
        auto it = entries_.find(key);
        if (it == entries_.end()) return;
        auto now_sec = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        auto& timestamps = it->second.failure_timestamps;
        while (!timestamps.empty() && (now_sec - timestamps.front()) > window_sec_) {
            timestamps.erase(timestamps.begin());
        }
    }
    uint32_t max_attempts_;
    uint32_t window_sec_;
    uint32_t lockout_sec_;
    struct Entry { std::vector<uint64_t> failure_timestamps; uint64_t lockout_until = 0; };
    std::unordered_map<std::string, Entry> entries_;
};

void RunSecurityTests() {
    TEST_SUITE("SECURITY TESTS");

    TEST_STEP("RateLimiter — 5 req/s, 6th blocked");
    {
        RateLimiter limiter(5, 1000);
        for (int i = 0; i < 5; i++) {
            TEST("Request " + std::to_string(i + 1) + " allowed", limiter.Allow("test_key"));
        }
        TEST("6th request blocked", !limiter.Allow("test_key"));
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
        TEST("After 1s allowed again", limiter.Allow("test_key"));
    }

    TEST_STEP("RateLimiter — independent keys");
    {
        RateLimiter limiter(3, 1000);
        for (int i = 0; i < 3; i++) {
            limiter.Allow("key_a");
            limiter.Allow("key_b");
        }
        TEST("Key A blocked", !limiter.Allow("key_a"));
        TEST("Key B blocked", !limiter.Allow("key_b"));
        TEST("Key C still allowed (unused)", limiter.Allow("key_c"));
    }

    TEST_STEP("RateLimiter — Clear resets counter");
    {
        RateLimiter limiter(3, 10000);
        for (int i = 0; i < 3; i++) limiter.Allow("clear_key");
        TEST("Blocked before clear", !limiter.Allow("clear_key"));
        limiter.Clear("clear_key");
        TEST("Allowed after clear", limiter.Allow("clear_key"));
    }

    TEST_STEP("BruteForceProtection — lockout after 5 failures");
    {
        BruteForceProtection bf(5, 60, 900);
        std::string key = "test_user";

        TEST("Initially not locked", !bf.IsLocked(key));
        for (int i = 0; i < 5; i++) {
            bf.RecordFailure(key);
        }
        TEST("Locked after 5 failures", bf.IsLocked(key));
        TEST("Remaining lockout > 0", bf.RemainingLockout(key) > 0);

        bf.RecordSuccess(key);
        TEST("Unlocked after success", !bf.IsLocked(key));
    }

    TEST_STEP("BruteForceProtection — different keys independent");
    {
        BruteForceProtection bf(3, 60, 900);
        for (int i = 0; i < 3; i++) bf.RecordFailure("user_a");
        bf.RecordFailure("user_b");

        TEST("User A locked", bf.IsLocked("user_a"));
        TEST("User B not locked (only 1 fail)", !bf.IsLocked("user_b"));
    }

    TEST_STEP("SQL injection — prepared statement binding");
    {
        sqlite3* db = nullptr;
        TEST("DB open", sqlite3_open(":memory:", &db) == SQLITE_OK);
        if (db) {
            sqlite3_exec(db, "CREATE TABLE users (id INT, name TEXT, password TEXT)", 0, 0, 0);
            sqlite3_exec(db, "INSERT INTO users VALUES (1, 'admin', 'secret123')", 0, 0, 0);

            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(db, "SELECT * FROM users WHERE name = ? AND password = ?", -1, &stmt, 0);
            sqlite3_bind_text(stmt, 1, "' OR 1=1 --", -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, "' OR 1=1 --", -1, SQLITE_STATIC);
            int rc = sqlite3_step(stmt);
            TEST("SQL injection returns no rows", rc == SQLITE_DONE);
            sqlite3_finalize(stmt);

            sqlite3_prepare_v2(db, "SELECT * FROM users WHERE name = ? AND password = ?", -1, &stmt, 0);
            sqlite3_bind_text(stmt, 1, "admin", -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, "secret123", -1, SQLITE_STATIC);
            rc = sqlite3_step(stmt);
            TEST("Legitimate login returns row", rc == SQLITE_ROW);
            sqlite3_finalize(stmt);

            sqlite3_close(db);
        }
    }

    TEST_STEP("Input validation — reject dangerous characters");
    {
        auto IsValidUsername = [](const std::string& s) -> bool {
            if (s.empty() || s.size() > 20) return false;
            for (char c : s) {
                if (!std::isalnum(c) && c != '_') return false;
            }
            return true;
        };

        TEST("Normal username valid", IsValidUsername("Player_01"));
        TEST("Empty username invalid", !IsValidUsername(""));
        TEST("SQL injection char rejected", !IsValidUsername("admin'--"));
        TEST("XSS chars rejected", !IsValidUsername("<script>"));
        TEST("Spaces rejected", !IsValidUsername("user name"));
        TEST("Long username rejected", !IsValidUsername(std::string(30, 'a')));
    }

    TEST_STEP("Session token — format validation");
    {
        auto IsValidToken = [](const std::string& t) -> bool {
            if (t.size() < 16 || t.size() > 128) return false;
            for (char c : t) {
                if (!std::isalnum(c) && c != '_') return false;
            }
            return true;
        };

        TEST("Valid session token", IsValidToken("session_token_abc_123_def_456"));
        TEST("Too short token invalid", !IsValidToken("short"));
        TEST("Token with special chars invalid", !IsValidToken("token with spaces"));
        TEST("Empty token invalid", !IsValidToken(""));
    }
}

#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>

class RateLimiter {
public:
    explicit RateLimiter(uint32_t max_requests = 100, uint32_t window_ms = 1000);

    bool Allow(const std::string& key);
    void Clear(const std::string& key);

private:
    uint32_t max_requests_;
    uint32_t window_ms_;
    std::unordered_map<std::string, std::vector<uint64_t>> log_;
};

class BruteForceProtection {
public:
    BruteForceProtection(uint32_t max_attempts = 5, uint32_t window_seconds = 60, uint32_t lockout_seconds = 900);

    bool IsLocked(const std::string& key);
    void RecordFailure(const std::string& key);
    void RecordSuccess(const std::string& key);
    uint32_t RemainingLockout(const std::string& key);

private:
    struct Entry {
        std::vector<uint64_t> failure_timestamps;
        uint64_t lockout_until = 0;
    };

    void Prune(const std::string& key);

    uint32_t max_attempts_;
    uint32_t window_seconds_;
    uint32_t lockout_seconds_;
    std::unordered_map<std::string, Entry> entries_;
};

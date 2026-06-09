#include "RateLimiter.h"
#include <algorithm>

static uint64_t NowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

RateLimiter::RateLimiter(uint32_t max_requests, uint32_t window_ms)
    : max_requests_(max_requests), window_ms_(window_ms) {}

bool RateLimiter::Allow(const std::string& key) {
    auto now = NowMs();
    auto& timestamps = log_[key];
    auto cutoff = now - window_ms_;

    timestamps.erase(
        std::remove_if(timestamps.begin(), timestamps.end(),
            [cutoff](uint64_t t) { return t < cutoff; }),
        timestamps.end());

    if (timestamps.size() >= max_requests_) return false;

    timestamps.push_back(now);
    return true;
}

void RateLimiter::Clear(const std::string& key) {
    log_.erase(key);
}

BruteForceProtection::BruteForceProtection(uint32_t max_attempts, uint32_t lockout_seconds)
    : max_attempts_(max_attempts), lockout_seconds_(lockout_seconds) {}

bool BruteForceProtection::IsLocked(const std::string& key) {
    auto it = entries_.find(key);
    if (it == entries_.end()) return false;
    auto now = NowMs();
    if (now >= it->second.lockout_until) {
        entries_.erase(it);
        return false;
    }
    return true;
}

void BruteForceProtection::RecordFailure(const std::string& key) {
    auto& entry = entries_[key];
    entry.failures++;
    if (entry.failures >= max_attempts_) {
        entry.lockout_until = NowMs() + lockout_seconds_ * 1000ULL;
    }
}

void BruteForceProtection::RecordSuccess(const std::string& key) {
    entries_.erase(key);
}

uint32_t BruteForceProtection::RemainingLockout(const std::string& key) {
    auto it = entries_.find(key);
    if (it == entries_.end()) return 0;
    auto now = NowMs();
    if (now >= it->second.lockout_until) {
        entries_.erase(it);
        return 0;
    }
    return static_cast<uint32_t>((it->second.lockout_until - now) / 1000);
}

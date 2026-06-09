#include "PunishManager.hpp"
#include <spdlog/spdlog.h>
#include <ctime>
#include <algorithm>
#include <limits>

PunishManager::PunishManager() {}

bool PunishManager::Initialize(const std::string& db_path) {
    LoadFromDB(db_path);
    spdlog::info("PunishManager: initialized with {} active punishments", punishments_.size());
    return true;
}

void PunishManager::LoadFromDB(const std::string& db_path) {
    // In production, load from database.
    // For now, load from a lightweight SQLite or file if available.
    (void)db_path;
    punishments_.clear();
    next_id_ = 1;
}

bool PunishManager::Ban(int account_id, const std::string& reason, const std::string& issued_by,
                         std::chrono::hours duration) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Check if already banned
    for (auto& p : punishments_) {
        if (p.type == PunishType::Ban && p.scope == PunishScope::Account &&
            p.target_id == account_id && IsActive(p)) {
            spdlog::warn("PunishManager: account {} is already banned", account_id);
            return false;
        }
    }

    auto now = std::chrono::system_clock::now();
    InternalPunish p;
    p.id = next_id_++;
    p.type = PunishType::Ban;
    p.scope = PunishScope::Account;
    p.target_id = account_id;
    p.reason = reason;
    p.issued_by = issued_by;
    p.issued_at_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    p.permanent = (duration.count() == 0);

    if (!p.permanent) {
        auto expires = now + duration;
        p.expires_at_epoch = std::chrono::duration_cast<std::chrono::seconds>(expires.time_since_epoch()).count();
    } else {
        p.expires_at_epoch = std::numeric_limits<int64_t>::max();
    }

    punishments_.push_back(p);
    spdlog::info("PunishManager: banned account {} ({}) - reason: {} - permanent: {}",
                 account_id, reason, issued_by, p.permanent);
    return true;
}

bool PunishManager::BanIP(const std::string& ip, const std::string& reason,
                           const std::string& issued_by, std::chrono::hours duration) {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& p : punishments_) {
        if (p.type == PunishType::Ban && p.scope == PunishScope::IP &&
            p.target_str == ip && IsActive(p)) {
            spdlog::warn("PunishManager: IP {} is already banned", ip);
            return false;
        }
    }

    auto now = std::chrono::system_clock::now();
    InternalPunish p;
    p.id = next_id_++;
    p.type = PunishType::Ban;
    p.scope = PunishScope::IP;
    p.target_str = ip;
    p.reason = reason;
    p.issued_by = issued_by;
    p.issued_at_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    p.permanent = (duration.count() == 0);

    if (!p.permanent) {
        auto expires = now + duration;
        p.expires_at_epoch = std::chrono::duration_cast<std::chrono::seconds>(expires.time_since_epoch()).count();
    } else {
        p.expires_at_epoch = std::numeric_limits<int64_t>::max();
    }

    punishments_.push_back(p);
    spdlog::info("PunishManager: banned IP {} - reason: {}", ip, reason);
    return true;
}

bool PunishManager::Mute(int account_id, const std::string& reason, const std::string& issued_by,
                          std::chrono::hours duration) {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& p : punishments_) {
        if (p.type == PunishType::Mute && p.scope == PunishScope::Account &&
            p.target_id == account_id && IsActive(p)) {
            spdlog::warn("PunishManager: account {} is already muted", account_id);
            return false;
        }
    }

    auto now = std::chrono::system_clock::now();
    InternalPunish p;
    p.id = next_id_++;
    p.type = PunishType::Mute;
    p.scope = PunishScope::Account;
    p.target_id = account_id;
    p.reason = reason;
    p.issued_by = issued_by;
    p.issued_at_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    auto expires = now + duration;
    p.expires_at_epoch = std::chrono::duration_cast<std::chrono::seconds>(expires.time_since_epoch()).count();

    punishments_.push_back(p);
    spdlog::info("PunishManager: muted account {} for {}h - reason: {}",
                 account_id, duration.count(), reason);
    return true;
}

bool PunishManager::Warn(int account_id, const std::string& reason, const std::string& issued_by) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto now = std::chrono::system_clock::now();
    InternalPunish p;
    p.id = next_id_++;
    p.type = PunishType::Warning;
    p.scope = PunishScope::Account;
    p.target_id = account_id;
    p.reason = reason;
    p.issued_by = issued_by;
    p.issued_at_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    p.expires_at_epoch = std::chrono::duration_cast<std::chrono::seconds>(
        (now + std::chrono::hours(24 * 30)).time_since_epoch()).count(); // Warnings last 30 days
    punishments_.push_back(p);

    // Count total strikes and auto-ban if threshold reached
    int strikes = GetStrikeCount(account_id);
    spdlog::warn("PunishManager: warned account {} - strike {}/{} - reason: {}",
                 account_id, strikes, GetStrikeThreshold(), reason);

    if (strikes >= GetStrikeThreshold()) {
        spdlog::warn("PunishManager: auto-banning account {} ({} strikes)", account_id, strikes);
        Ban(account_id, "Auto-ban: " + std::to_string(strikes) + " strikes", "System",
            std::chrono::hours(24 * 7)); // 7 days
    }
    return true;
}

bool PunishManager::IsBanned(int account_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& p : punishments_) {
        if (p.type == PunishType::Ban && p.scope == PunishScope::Account &&
            p.target_id == account_id && IsActive(p)) {
            return true;
        }
    }
    return false;
}

bool PunishManager::IsBannedIP(const std::string& ip) const {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& p : punishments_) {
        if (p.type == PunishType::Ban && p.scope == PunishScope::IP &&
            p.target_str == ip && IsActive(p)) {
            return true;
        }
    }
    return false;
}

bool PunishManager::IsMuted(int account_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& p : punishments_) {
        if (p.type == PunishType::Mute && p.scope == PunishScope::Account &&
            p.target_id == account_id && IsActive(p)) {
            return true;
        }
    }
    return false;
}

bool PunishManager::CanTrade(int account_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& p : punishments_) {
        if (p.type == PunishType::TradeRestrict && p.scope == PunishScope::Account &&
            p.target_id == account_id && IsActive(p)) {
            return false;
        }
    }
    return true;
}

bool PunishManager::CanParty(int account_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& p : punishments_) {
        if (p.type == PunishType::PartyRestrict && p.scope == PunishScope::Account &&
            p.target_id == account_id && IsActive(p)) {
            return false;
        }
    }
    return true;
}

bool PunishManager::CanChat(int account_id) const {
    return !IsMuted(account_id);
}

PunishRecord PunishManager::GetRecord(int account_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    PunishRecord record;
    for (auto& p : punishments_) {
        if (p.scope == PunishScope::Account && p.target_id == account_id) {
            PunishEntry entry;
            entry.type = p.type;
            entry.scope = p.scope;
            entry.target_id = p.target_id;
            entry.target_str = p.target_str;
            entry.reason = p.reason;
            entry.issued_by = p.issued_by;
            entry.issued_at = std::chrono::system_clock::time_point(
                std::chrono::seconds(p.issued_at_epoch));
            if (p.permanent) {
                entry.expires_at = std::chrono::system_clock::time_point::max();
            } else {
                entry.expires_at = std::chrono::system_clock::time_point(
                    std::chrono::seconds(p.expires_at_epoch));
            }
            entry.permanent = p.permanent;
            entry.strike_count = p.strike_count;
            record.history.push_back(std::move(entry));

            if (IsActive(p)) {
                if (p.type == PunishType::Ban) record.is_banned = true;
                if (p.type == PunishType::Mute) record.is_muted = true;
                if (p.type == PunishType::Warning) record.total_strikes += p.strike_count + 1;
            }
        }
    }
    return record;
}

std::vector<PunishEntry> PunishManager::GetActivePunishments(int account_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<PunishEntry> result;
    for (auto& p : punishments_) {
        if (p.scope == PunishScope::Account && p.target_id == account_id && IsActive(p)) {
            PunishEntry entry;
            entry.type = p.type;
            entry.scope = p.scope;
            entry.target_id = p.target_id;
            entry.target_str = p.target_str;
            entry.reason = p.reason;
            entry.issued_by = p.issued_by;
            entry.issued_at = std::chrono::system_clock::time_point(
                std::chrono::seconds(p.issued_at_epoch));
            if (p.permanent) {
                entry.expires_at = std::chrono::system_clock::time_point::max();
            } else {
                entry.expires_at = std::chrono::system_clock::time_point(
                    std::chrono::seconds(p.expires_at_epoch));
            }
            entry.permanent = p.permanent;
            entry.strike_count = p.strike_count;
            result.push_back(std::move(entry));
        }
    }
    return result;
}

std::vector<PunishEntry> PunishManager::GetPunishmentHistory(int account_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<PunishEntry> result;
    for (auto& p : punishments_) {
        if (p.scope == PunishScope::Account && p.target_id == account_id) {
            PunishEntry entry;
            entry.type = p.type;
            entry.scope = p.scope;
            entry.target_id = p.target_id;
            entry.target_str = p.target_str;
            entry.reason = p.reason;
            entry.issued_by = p.issued_by;
            entry.issued_at = std::chrono::system_clock::time_point(
                std::chrono::seconds(p.issued_at_epoch));
            if (p.permanent) {
                entry.expires_at = std::chrono::system_clock::time_point::max();
            } else {
                entry.expires_at = std::chrono::system_clock::time_point(
                    std::chrono::seconds(p.expires_at_epoch));
            }
            entry.permanent = p.permanent;
            entry.strike_count = p.strike_count;
            result.push_back(std::move(entry));
        }
    }
    return result;
}

bool PunishManager::LiftPunishment(int punish_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto it = punishments_.begin(); it != punishments_.end(); ++it) {
        if (it->id == punish_id) {
            spdlog::info("PunishManager: lifted punishment {} (type={})",
                         punish_id, static_cast<int>(it->type));
            punishments_.erase(it);
            return true;
        }
    }
    return false;
}

bool PunishManager::LiftAllPunishments(int account_id, PunishType type) {
    std::lock_guard<std::mutex> lock(mutex_);

    bool found = false;
    for (auto it = punishments_.begin(); it != punishments_.end(); ) {
        if (it->scope == PunishScope::Account && it->target_id == account_id &&
            (type == PunishType::None || it->type == type)) {
            spdlog::info("PunishManager: lifted punishment {} for account {}",
                         static_cast<int>(it->type), account_id);
            it = punishments_.erase(it);
            found = true;
        } else {
            ++it;
        }
    }
    return found;
}

void PunishManager::Update() {
    std::lock_guard<std::mutex> lock(mutex_);

    auto now = std::chrono::system_clock::now();
    int64_t now_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    for (auto it = punishments_.begin(); it != punishments_.end(); ) {
        if (!it->permanent && it->expires_at_epoch <= now_epoch) {
            spdlog::info("PunishManager: expired punishment {} (type={}) for target {}",
                         it->id, static_cast<int>(it->type), it->target_id);
            it = punishments_.erase(it);
        } else {
            ++it;
        }
    }
}

int PunishManager::AddStrike(int account_id, const std::string& reason,
                               const std::string& issued_by) {
    Warn(account_id, reason, issued_by);
    return GetStrikeCount(account_id);
}

int PunishManager::GetStrikeCount(int account_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    int count = 0;
    auto now = std::chrono::system_clock::now();
    int64_t now_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    for (auto& p : punishments_) {
        if (p.type == PunishType::Warning && p.scope == PunishScope::Account &&
            p.target_id == account_id && p.expires_at_epoch > now_epoch) {
            count += p.strike_count + 1;
        }
    }
    return count;
}

bool PunishManager::IsActive(const InternalPunish& p) const {
    if (p.permanent) return true;
    auto now = std::chrono::system_clock::now();
    int64_t now_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    return p.expires_at_epoch > now_epoch;
}

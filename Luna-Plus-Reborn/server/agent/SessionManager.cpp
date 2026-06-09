#include "SessionManager.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

SessionManager::SessionManager() {}

std::string SessionManager::GenerateToken() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 15);
    static const char* hex = "0123456789abcdef";
    std::string uuid(36, '-');
    for (int i = 0; i < 36; i++) {
        if (i == 8 || i == 13 || i == 18 || i == 23) continue;
        uuid[i] = hex[dist(rng)];
    }
    return uuid + "-" + std::to_string(std::time(nullptr));
}

std::string SessionManager::CreateSession(int account_id, const std::string& username,
                                           const std::string& client_ip) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Check max sessions per account
    if (max_sessions_per_account_ > 0) {
        int count = 0;
        for (auto& [token, session] : sessions_) {
            if (session.account_id == account_id) count++;
        }
        if (count >= max_sessions_per_account_) {
            spdlog::warn("SessionManager: account {} exceeded max sessions ({})",
                         account_id, max_sessions_per_account_);
            return "";
        }
    }

    std::string token = GenerateToken();
    auto now = std::chrono::steady_clock::now();
    sessions_[token] = {account_id, username, client_ip, 0, "", now, now};
    spdlog::info("SessionManager: created session {} for account {} ({})",
                 token.substr(0, 8), account_id, username);
    return token;
}

int SessionManager::ValidateSession(const std::string& token) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = sessions_.find(token);
    if (it == sessions_.end()) {
        return -1;
    }

    auto now = std::chrono::steady_clock::now();
    auto age = std::chrono::duration_cast<std::chrono::hours>(now - it->second.created_at);
    if (age >= session_timeout_) {
        spdlog::info("SessionManager: session {} expired (age={}h)",
                     token.substr(0, 8), age.count());
        sessions_.erase(it);
        return -1;
    }

    it->second.last_activity = now;
    return it->second.account_id;
}

void SessionManager::DestroySession(const std::string& token) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = sessions_.find(token);
    if (it != sessions_.end()) {
        spdlog::info("SessionManager: destroyed session {} for account {}",
                     token.substr(0, 8), it->second.account_id);
        sessions_.erase(it);
    }
}

void SessionManager::DestroySessionsByAccount(int account_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto it = sessions_.begin(); it != sessions_.end(); ) {
        if (it->second.account_id == account_id) {
            spdlog::info("SessionManager: destroying session {} for account {}",
                         it->first.substr(0, 8), account_id);
            it = sessions_.erase(it);
        } else {
            ++it;
        }
    }
}

SessionInfo* SessionManager::GetSession(const std::string& token) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = sessions_.find(token);
    if (it == sessions_.end()) return nullptr;

    thread_local static SessionInfo info;
    info.token = it->first;
    info.account_id = it->second.account_id;
    info.username = it->second.username;
    info.client_ip = it->second.client_ip;
    info.selected_char_id = it->second.selected_char_id;
    info.selected_char_name = it->second.selected_char_name;
    info.created_at = it->second.created_at;
    info.last_activity = it->second.last_activity;
    return &info;
}

int SessionManager::GetActiveSessionCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<int>(sessions_.size());
}

std::vector<SessionInfo> SessionManager::GetAllSessions() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<SessionInfo> result;
    for (auto& [token, session] : sessions_) {
        SessionInfo info;
        info.token = token;
        info.account_id = session.account_id;
        info.username = session.username;
        info.client_ip = session.client_ip;
        info.selected_char_id = session.selected_char_id;
        info.selected_char_name = session.selected_char_name;
        info.created_at = session.created_at;
        info.last_activity = session.last_activity;
        result.push_back(std::move(info));
    }
    return result;
}

std::vector<SessionInfo> SessionManager::GetSessionsByAccount(int account_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<SessionInfo> result;
    for (auto& [token, session] : sessions_) {
        if (session.account_id == account_id) {
            SessionInfo info;
            info.token = token;
            info.account_id = session.account_id;
            info.username = session.username;
            info.client_ip = session.client_ip;
            info.selected_char_id = session.selected_char_id;
            info.selected_char_name = session.selected_char_name;
            info.created_at = session.created_at;
            info.last_activity = session.last_activity;
            result.push_back(std::move(info));
        }
    }
    return result;
}

bool SessionManager::IsAccountOnline(int account_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& [token, session] : sessions_) {
        if (session.account_id == account_id) return true;
    }
    return false;
}

void SessionManager::Update() {
    CleanupExpired();
}

void SessionManager::SetSessionTimeout(std::chrono::hours timeout) {
    session_timeout_ = timeout;
}

void SessionManager::SetMaxSessionsPerAccount(int max) {
    max_sessions_per_account_ = max;
}

void SessionManager::BindCharacter(const std::string& token, int char_id,
                                    const std::string& char_name) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = sessions_.find(token);
    if (it != sessions_.end()) {
        it->second.selected_char_id = char_id;
        it->second.selected_char_name = char_name;
        spdlog::debug("SessionManager: bound char {} ({}) to session {}",
                      char_id, char_name, token.substr(0, 8));
    }
}

int SessionManager::GetBoundCharacter(const std::string& token) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = sessions_.find(token);
    if (it == sessions_.end()) return 0;
    return it->second.selected_char_id;
}

std::string SessionManager::GetBoundCharacterName(const std::string& token) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = sessions_.find(token);
    if (it == sessions_.end()) return "";
    return it->second.selected_char_name;
}

void SessionManager::CleanupExpired() {
    std::lock_guard<std::mutex> lock(mutex_);

    auto now = std::chrono::steady_clock::now();
    for (auto it = sessions_.begin(); it != sessions_.end(); ) {
        auto age = std::chrono::duration_cast<std::chrono::hours>(now - it->second.created_at);
        if (age >= session_timeout_) {
            spdlog::info("SessionManager: expired session {} for account {}",
                         it->first.substr(0, 8), it->second.account_id);
            it = sessions_.erase(it);
        } else {
            ++it;
        }
    }
}

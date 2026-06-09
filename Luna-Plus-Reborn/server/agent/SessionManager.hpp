#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <mutex>

struct SessionInfo {
    std::string token;
    int account_id = 0;
    std::string username;
    std::string client_ip;
    int selected_char_id = 0;
    std::string selected_char_name;
    std::chrono::steady_clock::time_point created_at;
    std::chrono::steady_clock::time_point last_activity;
};

class SessionManager {
public:
    SessionManager();

    // Session lifecycle
    std::string CreateSession(int account_id, const std::string& username, const std::string& client_ip);
    int ValidateSession(const std::string& token);
    void DestroySession(const std::string& token);
    void DestroySessionsByAccount(int account_id);

    // Query
    SessionInfo* GetSession(const std::string& token);
    int GetActiveSessionCount() const;
    std::vector<SessionInfo> GetAllSessions() const;
    std::vector<SessionInfo> GetSessionsByAccount(int account_id) const;
    bool IsAccountOnline(int account_id) const;

    // Maintenance
    void Update(); // Call every frame to purge expired sessions
    void SetSessionTimeout(std::chrono::hours timeout);
    void SetMaxSessionsPerAccount(int max);

    // Character binding
    void BindCharacter(const std::string& token, int char_id, const std::string& char_name);
    int GetBoundCharacter(const std::string& token) const;
    std::string GetBoundCharacterName(const std::string& token) const;

private:
    struct Session {
        int account_id;
        std::string username;
        std::string client_ip;
        int selected_char_id;
        std::string selected_char_name;
        std::chrono::steady_clock::time_point created_at;
        std::chrono::steady_clock::time_point last_activity;
    };

    mutable std::mutex mutex_;
    std::unordered_map<std::string, Session> sessions_;
    std::chrono::hours session_timeout_{24};
    int max_sessions_per_account_ = 1;

    static std::string GenerateToken();
    void CleanupExpired();
};

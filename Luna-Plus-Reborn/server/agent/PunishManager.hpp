#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <mutex>

enum class PunishType : uint8_t {
    None = 0,
    Ban = 1,
    Mute = 2,
    CharacterRestrict = 3,
    TradeRestrict = 4,
    PartyRestrict = 5,
    ChatRestrict = 6,
    Warning = 7,
};

enum class PunishScope : uint8_t {
    Account = 0,
    Character = 1,
    IP = 2,
};

struct PunishEntry {
    PunishType type = PunishType::None;
    PunishScope scope = PunishScope::Account;
    int target_id = 0;          // account_id, char_id, or IP hash
    std::string target_str;     // username, character name, or IP string
    std::string reason;
    std::string issued_by;
    std::chrono::system_clock::time_point issued_at;
    std::chrono::system_clock::time_point expires_at;  // time_point::max() = permanent
    bool permanent = false;
    int strike_count = 0;
};

struct PunishRecord {
    std::vector<PunishEntry> history;
    int total_strikes = 0;
    bool is_banned = false;
    bool is_muted = false;
};

class PunishManager {
public:
    PunishManager();

    // Initialize from database
    bool Initialize(const std::string& db_path);

    // Issue punishments
    bool Ban(int account_id, const std::string& reason, const std::string& issued_by,
             std::chrono::hours duration = std::chrono::hours(0));
    bool BanIP(const std::string& ip, const std::string& reason, const std::string& issued_by,
               std::chrono::hours duration = std::chrono::hours(0));
    bool Mute(int account_id, const std::string& reason, const std::string& issued_by,
              std::chrono::hours duration = std::chrono::hours(24));
    bool Warn(int account_id, const std::string& reason, const std::string& issued_by);

    // Check
    bool IsBanned(int account_id) const;
    bool IsBannedIP(const std::string& ip) const;
    bool IsMuted(int account_id) const;
    bool CanTrade(int account_id) const;
    bool CanParty(int account_id) const;
    bool CanChat(int account_id) const;

    // Queries
    PunishRecord GetRecord(int account_id) const;
    std::vector<PunishEntry> GetActivePunishments(int account_id) const;
    std::vector<PunishEntry> GetPunishmentHistory(int account_id) const;

    // Management
    bool LiftPunishment(int punish_id);
    bool LiftAllPunishments(int account_id, PunishType type);
    void Update(); // Check expirations

    // Auto-punish (strike-based escalation)
    int AddStrike(int account_id, const std::string& reason, const std::string& issued_by);
    int GetStrikeCount(int account_id) const;

private:
    struct InternalPunish {
        int id = 0;
        PunishType type;
        PunishScope scope;
        int target_id = 0;
        std::string target_str;
        std::string reason;
        std::string issued_by;
        int64_t issued_at_epoch = 0;
        int64_t expires_at_epoch = 0;
        bool permanent = false;
        int strike_count = 0;
    };

    mutable std::mutex mutex_;
    std::vector<InternalPunish> punishments_;
    int next_id_ = 1;

    void LoadFromDB(const std::string& db_path);
    bool IsActive(const InternalPunish& p) const;
    int GetStrikeThreshold() const { return 3; }
};

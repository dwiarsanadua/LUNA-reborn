// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>

#include "server/shared/RateLimiter.h"

struct CharData {
    int id;
    std::string name;
    int level;
    int class_id;
    int gender;
    int map_id;
    float pos_x, pos_y, pos_z;
    int hp, max_hp;
    int body_type;
    int face_id;
    int hair_id;
    int weapon_id;
    int dress_id;
    int shoes_id;
    int glove_id;
    int job_grade;
    int job[6];
    std::string guild_name;
};

struct LoginResult {
    bool success = false;
    int account_id = 0;
    std::string session_token;
    std::string message;
};

enum class NackCode : uint8_t {
    SUCCESS = 0,
    INVALID_CREDENTIALS = 1,
    ALREADY_LOGGED_IN = 2,
    BANNED = 3,
    MAINTENANCE = 4,
    VERSION_MISMATCH = 5,
    ACCOUNT_LOCKED = 6,
    SERVER_FULL = 7,
    SESSION_EXPIRED = 8,
    CHARACTER_LIST_FAILED = 9,
    CREATE_CHARACTER_FAILED = 10,
    DELETE_CHARACTER_FAILED = 11,
    ENTER_WORLD_FAILED = 12,
    NAME_ALREADY_EXISTS = 13,
    MAX_CHARACTERS = 14,
    INVALID_SESSION = 15,
    RATE_LIMITED = 16,
};

class NetworkLayer;
class Database;

class AgentServer {
public:
    AgentServer();
    ~AgentServer();

    bool Initialize(uint16_t port);
    void Shutdown();
    void Update();

    LoginResult HandleLogin(const std::string& username, const std::string& password, const std::string& client_ip);
    std::vector<CharData> HandleCharacterList(const std::string& session_token);
    bool HandleCreateCharacter(const std::string& session_token, const std::string& name,
                               int class_id, int gender, int body_type, int face_id, int hair_id);
    bool HandleDeleteCharacter(const std::string& session_token, int char_id);
    bool HandleEnterWorld(const std::string& session_token, int char_id, CharData& out);
    NackCode HandleLoginNack(const std::string& username, const std::string& password,
                             const std::string& client_ip);
    bool HandleCharacterNameCheck(const std::string& session_token, const std::string& name);

    std::string CreateSession(int account_id);
    int ValidateSession(const std::string& token);
    void DestroySession(const std::string& token);

private:
    struct Session {
        int account_id;
        std::string token;
        std::string username;
        std::string client_ip;
        std::chrono::steady_clock::time_point created;
    };

    std::unique_ptr<NetworkLayer> network_;
    std::unique_ptr<Database> db_;
    std::unordered_map<std::string, Session> sessions_;
    RateLimiter packet_limiter_;
    RateLimiter login_limiter_;
    BruteForceProtection brute_force_;
};

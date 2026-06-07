// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>

struct CharacterInfo {
    int id;
    std::string name;
    int level;
    int class_id;
    int gender;
    int map_id;
    float pos_x, pos_y, pos_z;
    int hp, max_hp;
};

struct LoginResult {
    bool success = false;
    int account_id = 0;
    std::string session_token;
    std::string message;
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

    LoginResult HandleLogin(const std::string& username, const std::string& password);
    std::vector<CharacterInfo> HandleCharacterList(const std::string& session_token);
    bool HandleCreateCharacter(const std::string& session_token, const std::string& name,
                               int class_id, const std::string& appearance);
    bool HandleDeleteCharacter(const std::string& session_token, int char_id);

    std::string CreateSession(int account_id);
    int ValidateSession(const std::string& token);
    void DestroySession(const std::string& token);

private:
    struct Session {
        int account_id;
        std::string token;
        std::chrono::steady_clock::time_point created;
    };

    std::unique_ptr<NetworkLayer> network_;
    std::unique_ptr<Database> db_;
    std::unordered_map<std::string, Session> sessions_;
};

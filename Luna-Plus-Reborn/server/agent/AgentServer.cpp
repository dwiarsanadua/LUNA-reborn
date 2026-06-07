#include "AgentServer.h"
#include "engine/network/NetworkLayer.h"
#include "server/shared/Database.h"
#include <spdlog/spdlog.h>
#include <sstream>
#include <iomanip>
#include <random>
#include <ctime>

static std::string GenerateUUID() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 15);
    static const char* hex = "0123456789abcdef";
    std::string uuid(36, '-');
    for (int i = 0; i < 36; i++) {
        if (i == 8 || i == 13 || i == 18 || i == 23) continue;
        uuid[i] = hex[dist(rng)];
    }
    return uuid;
}

AgentServer::AgentServer()
    : network_(std::make_unique<NetworkLayer>())
    , db_(std::make_unique<Database>())
{}

AgentServer::~AgentServer() { Shutdown(); }

bool AgentServer::Initialize(uint16_t port) {
    if (!db_->Initialize("data/luna_agent.db")) {
        spdlog::error("AgentServer: database init failed");
        return false;
    }

    // Create tables if not exist
    db_->Execute(
        "CREATE TABLE IF NOT EXISTS accounts ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  username TEXT UNIQUE NOT NULL,"
        "  password_hash TEXT NOT NULL,"
        "  created_at TEXT DEFAULT CURRENT_TIMESTAMP"
        ");"
        "CREATE TABLE IF NOT EXISTS characters ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  account_id INTEGER NOT NULL,"
        "  name TEXT UNIQUE NOT NULL,"
        "  class_id INTEGER DEFAULT 0,"
        "  gender INTEGER DEFAULT 0,"
        "  level INTEGER DEFAULT 1,"
        "  map_id INTEGER DEFAULT 51,"
        "  pos_x REAL DEFAULT 0,"
        "  pos_y REAL DEFAULT 0,"
        "  pos_z REAL DEFAULT 0,"
        "  hp INTEGER DEFAULT 500,"
        "  max_hp INTEGER DEFAULT 500,"
        "  appearance TEXT DEFAULT '',"
        "  FOREIGN KEY(account_id) REFERENCES accounts(id)"
        ");"
        "INSERT OR IGNORE INTO accounts (id, username, password_hash) VALUES (1, 'admin', 'admin');"
    );

    if (!network_->Initialize(port)) {
        spdlog::error("AgentServer: network init failed on port {}", port);
        return false;
    }

    network_->SetReceiveCallback([this](const uint8_t* data, size_t size) {
        // Parse and dispatch packet
        if (size < 4) return;
        uint16_t type = (data[0] << 8) | data[1];
        // Packet dispatch would use flatbuffers in production
        spdlog::info("AgentServer: received packet type=0x{:04X} size={}", type, size);
    });

    spdlog::info("AgentServer: initialized on port {}", port);
    return true;
}

void AgentServer::Shutdown() {
    network_->Shutdown();
    db_->Shutdown();
}

void AgentServer::Update() {
    network_->Update();

    // Cleanup expired sessions (24 hours)
    auto now = std::chrono::steady_clock::now();
    for (auto it = sessions_.begin(); it != sessions_.end(); ) {
        auto age = std::chrono::duration_cast<std::chrono::hours>(now - it->second.created);
        if (age.count() >= 24) {
            spdlog::info("AgentServer: session {} expired", it->second.token.substr(0, 8));
            it = sessions_.erase(it);
        } else {
            ++it;
        }
    }
}

LoginResult AgentServer::HandleLogin(const std::string& username, const std::string& password) {
    LoginResult result;

    auto rows = db_->Query(
        "SELECT id, password_hash FROM accounts WHERE username='" + username + "'");
    if (rows.empty()) {
        result.message = "Invalid username or password";
        return result;
    }

    int account_id = std::stoi(rows[0][0]);
    std::string pw_hash = rows[0][1];

    // Simple password check (would use bcrypt in production)
    if (pw_hash != password) {
        result.message = "Invalid username or password";
        return result;
    }

    result.success = true;
    result.account_id = account_id;
    result.session_token = CreateSession(account_id);
    result.message = "Login successful";
    return result;
}

std::vector<CharacterInfo> AgentServer::HandleCharacterList(const std::string& session_token) {
    int account_id = ValidateSession(session_token);
    if (account_id < 0) return {};

    auto rows = db_->Query(
        "SELECT id, name, level, class_id, gender, map_id, pos_x, pos_y, pos_z, hp, max_hp "
        "FROM characters WHERE account_id=" + std::to_string(account_id));

    std::vector<CharacterInfo> chars;
    for (auto& row : rows) {
        CharacterInfo ci;
        ci.id = std::stoi(row[0]);
        ci.name = row[1];
        ci.level = std::stoi(row[2]);
        ci.class_id = std::stoi(row[3]);
        ci.gender = std::stoi(row[4]);
        ci.map_id = std::stoi(row[5]);
        ci.pos_x = std::stof(row[6]);
        ci.pos_y = std::stof(row[7]);
        ci.pos_z = std::stof(row[8]);
        ci.hp = std::stoi(row[9]);
        ci.max_hp = std::stoi(row[10]);
        chars.push_back(std::move(ci));
    }
    return chars;
}

bool AgentServer::HandleCreateCharacter(const std::string& session_token, const std::string& name,
                                         int class_id, const std::string& appearance) {
    int account_id = ValidateSession(session_token);
    if (account_id < 0) return false;

    std::string sql = "INSERT INTO characters (account_id, name, class_id, appearance) VALUES (" +
        std::to_string(account_id) + ",'" + name + "'," + std::to_string(class_id) + ",'" + appearance + "')";
    return db_->Execute(sql);
}

bool AgentServer::HandleDeleteCharacter(const std::string& session_token, int char_id) {
    int account_id = ValidateSession(session_token);
    if (account_id < 0) return false;

    return db_->Execute(
        "DELETE FROM characters WHERE id=" + std::to_string(char_id) +
        " AND account_id=" + std::to_string(account_id));
}

std::string AgentServer::CreateSession(int account_id) {
    std::string token = GenerateUUID() + "-" + std::to_string(std::time(nullptr));
    sessions_[token] = {account_id, token, std::chrono::steady_clock::now()};
    return token;
}

int AgentServer::ValidateSession(const std::string& token) {
    auto it = sessions_.find(token);
    if (it == sessions_.end()) return -1;

    auto age = std::chrono::duration_cast<std::chrono::hours>(
        std::chrono::steady_clock::now() - it->second.created);
    if (age.count() >= 24) {
        sessions_.erase(it);
        return -1;
    }
    return it->second.account_id;
}

void AgentServer::DestroySession(const std::string& token) {
    sessions_.erase(token);
}

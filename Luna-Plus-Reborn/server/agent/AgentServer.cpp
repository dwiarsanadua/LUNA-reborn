#include "AgentServer.h"
#include "engine/network/NetworkLayer.h"
#include "server/shared/Database.h"
#include <spdlog/spdlog.h>
#include <sstream>
#include <iomanip>
#include <random>
#include <ctime>
#include <flatbuffers/flatbuffers.h>
#include <Login_generated.h>
#include <Character_generated.h>
#include <PacketType_generated.h>

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

static std::vector<uint8_t> BuildResponse(uint16_t type, flatbuffers::FlatBufferBuilder& fbb) {
    uint8_t type_bytes[2];
    type_bytes[0] = (type >> 8) & 0xFF;
    type_bytes[1] = type & 0xFF;
    std::vector<uint8_t> resp(2 + fbb.GetSize());
    resp[0] = type_bytes[0];
    resp[1] = type_bytes[1];
    std::memcpy(resp.data() + 2, fbb.GetBufferPointer(), fbb.GetSize());
    return resp;
}

AgentServer::AgentServer()
    : network_(std::make_unique<NetworkLayer>())
    , db_(std::make_unique<Database>())
{}

AgentServer::~AgentServer() { Shutdown(); }

bool AgentServer::Initialize(uint16_t port) {
    if (!db_->Initialize("assets/data/luna_agent.db")) {
        spdlog::error("AgentServer: database init failed");
        return false;
    }

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

    using namespace luna::protocol;
    network_->SetReceiveCallback([this](const uint8_t* data, size_t size) {
        if (size < 2) return;
        uint16_t type = (data[0] << 8) | data[1];
        const uint8_t* payload = data + 2;
        size_t payload_len = size - 2;

        switch (type) {
        case PacketType_MP_USERCONN_LOGIN_SYN: {
            auto req = flatbuffers::GetRoot<LoginRequest>(payload);
            std::string username = req->username() ? req->username()->str() : "";
            // In production, hash would be verified; here we use raw password
            std::string password = "admin"; // would come from a password field
            auto result = HandleLogin(username, password);

            flatbuffers::FlatBufferBuilder fbb;
            auto resp = CreateLoginResponse(fbb,
                result.success ? LoginResult_LOGIN_OK : LoginResult_LOGIN_FAILED,
                result.success ? fbb.CreateString(result.session_token) : 0);
            fbb.Finish(resp);
            auto out = BuildResponse(PacketType_MP_USERCONN_LOGIN_ACK, fbb);
            network_->Send(out.data(), out.size());
            break;
        }
        case PacketType_MP_USERCONN_CHARACTERLIST_SYN: {
            auto req = flatbuffers::GetRoot<ServerListRequest>(payload);
            std::string token = req->session_token() ? req->session_token()->str() : "";
            auto chars = HandleCharacterList(token);

            flatbuffers::FlatBufferBuilder fbb;
            std::vector<flatbuffers::Offset<CharacterInfo>> fb_chars;
            for (auto& c : chars) {
                auto pos = Vec3(c.pos_x, c.pos_y, c.pos_z);
                fb_chars.push_back(CreateCharacterInfoDirect(fbb, c.id, c.name.c_str(),
                    c.level, c.class_id, c.gender, c.map_id, &pos, c.hp, c.max_hp));
            }
            auto resp = CreateCharacterListResponseDirect(fbb, &fb_chars, 0);
            fbb.Finish(resp);
            auto out = BuildResponse(PacketType_MP_USERCONN_CHARACTERLIST_ACK, fbb);
            network_->Send(out.data(), out.size());
            break;
        }
        case PacketType_MP_USERCONN_CHARACTER_MAKE_SYN: {
            auto req = flatbuffers::GetRoot<CreateCharacterRequest>(payload);
            std::string token = req->session_token() ? req->session_token()->str() : "";
            std::string name = req->name() ? req->name()->str() : "NewChar";
            int class_id = req->class_();
            bool ok = HandleCreateCharacter(token, name, class_id, "");

            flatbuffers::FlatBufferBuilder fbb;
            auto result_status = ok ? LoginResult_LOGIN_OK : LoginResult_LOGIN_FAILED;
            CharacterInfoBuilder ci(fbb);
            ci.add_id(0);
            ci.add_level(1);
            auto resp = CreateCreateCharacterResponse(fbb, result_status, ci.Finish());
            fbb.Finish(resp);
            auto out = BuildResponse(PacketType_MP_USERCONN_CHARACTER_MAKE_ACK, fbb);
            network_->Send(out.data(), out.size());
            break;
        }
        case PacketType_MP_USERCONN_CHARACTER_DELETE_SYN: {
            auto req = flatbuffers::GetRoot<DeleteCharacterRequest>(payload);
            std::string token = req->session_token() ? req->session_token()->str() : "";
            int char_id = req->character_id();
            bool ok = HandleDeleteCharacter(token, char_id);

            flatbuffers::FlatBufferBuilder fbb;
            auto resp = CreateDeleteCharacterResponse(fbb, ok ? 0 : 1);
            fbb.Finish(resp);
            auto out = BuildResponse(PacketType_MP_USERCONN_CHARACTER_DELETE_ACK, fbb);
            network_->Send(out.data(), out.size());
            break;
        }
        default:
            spdlog::info("AgentServer: unhandled packet type=0x{:04X} size={}", type, payload_len);
        }
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

std::vector<CharData> AgentServer::HandleCharacterList(const std::string& session_token) {
    int account_id = ValidateSession(session_token);
    if (account_id < 0) return {};
    auto rows = db_->Query(
        "SELECT id, name, level, class_id, gender, map_id, pos_x, pos_y, pos_z, hp, max_hp "
        "FROM characters WHERE account_id=" + std::to_string(account_id));
    std::vector<CharData> chars;
    for (auto& row : rows) {
        CharData ci;
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
    // Check duplicate name
    auto existing = db_->Query("SELECT id FROM characters WHERE name='" + name + "'");
    if (!existing.empty()) return false;
    // Check max characters per account (4)
    auto count = db_->Query("SELECT COUNT(*) FROM characters WHERE account_id=" + std::to_string(account_id));
    if (!count.empty() && std::stoi(count[0][0]) >= 4) return false;
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

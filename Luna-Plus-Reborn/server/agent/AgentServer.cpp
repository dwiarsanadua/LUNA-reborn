#include "AgentServer.h"
#include "engine/network/NetworkLayer.h"
#include "server/shared/Database.h"
#include "server/shared/BcryptUtils.h"
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

static std::string GetClientIp() {
    return "0.0.0.0";
}

AgentServer::AgentServer()
    : network_(std::make_unique<NetworkLayer>())
    , db_(std::make_unique<Database>())
    , packet_limiter_(500, 1000)
    , login_limiter_(10, 1000)
    , brute_force_(5, 900)
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
        "  banned INTEGER DEFAULT 0,"
        "  is_locked INTEGER DEFAULT 0,"
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
        "  body_type INTEGER DEFAULT 0,"
        "  face_id INTEGER DEFAULT 0,"
        "  hair_id INTEGER DEFAULT 0,"
        "  weapon_id INTEGER DEFAULT 0,"
        "  dress_id INTEGER DEFAULT 0,"
        "  shoes_id INTEGER DEFAULT 0,"
        "  glove_id INTEGER DEFAULT 0,"
        "  job_grade INTEGER DEFAULT 0,"
        "  job1 INTEGER DEFAULT 0,"
        "  job2 INTEGER DEFAULT 0,"
        "  job3 INTEGER DEFAULT 0,"
        "  job4 INTEGER DEFAULT 0,"
        "  job5 INTEGER DEFAULT 0,"
        "  job6 INTEGER DEFAULT 0,"
        "  guild_name TEXT DEFAULT '',"
        "  appearance TEXT DEFAULT '',"
        "  FOREIGN KEY(account_id) REFERENCES accounts(id)"
        ");"
    );

    auto admin_hash = BcryptUtils::Hash("admin", 10);
    db_->Execute(
        "INSERT OR IGNORE INTO accounts (id, username, password_hash) VALUES (1, 'admin', '" +
        admin_hash + "');"
    );

    if (!network_->Initialize(port)) {
        spdlog::error("AgentServer: network init failed on port {}", port);
        return false;
    }

    using namespace luna::protocol;
    network_->SetReceiveCallback([this](uint16_t type, const uint8_t* payload, size_t payload_len) {
        switch (type) {
        case PacketType_MP_USERCONN_LOGIN_SYN: {
            if (!packet_limiter_.Allow("login_global")) {
                spdlog::warn("AgentServer: global login rate limit exceeded");
                flatbuffers::FlatBufferBuilder fbb;
                auto resp = CreateLoginResponseDirect(fbb, LoginResult_InvalidCredentials, nullptr);
                fbb.Finish(resp);
                network_->SendPacket(PacketType_MP_USERCONN_LOGIN_NACK,
                    fbb.GetBufferPointer(), fbb.GetSize());
                break;
            }

            auto req = flatbuffers::GetRoot<LoginRequest>(payload);
            std::string username = req->username() ? req->username()->str() : "";
            auto pw_vec = req->password_hash();
            std::string password;
            if (pw_vec && pw_vec->size() > 0) {
                password.assign(reinterpret_cast<const char*>(pw_vec->data()), pw_vec->size());
            }
            std::string client_ip = GetClientIp();

            std::string ip_key = "bf_ip:" + client_ip;
            std::string user_key = "bf_user:" + username;

            if (brute_force_.IsLocked(ip_key) || brute_force_.IsLocked(user_key)) {
                spdlog::warn("AgentServer: brute-force lockout for user={} ip={}", username, client_ip);
                flatbuffers::FlatBufferBuilder fbb;
                auto resp = CreateLoginResponseDirect(fbb, LoginResult_InvalidCredentials, nullptr);
                fbb.Finish(resp);
                network_->SendPacket(PacketType_MP_USERCONN_LOGIN_NACK,
                    fbb.GetBufferPointer(), fbb.GetSize());
                break;
            }

            auto nack = HandleLoginNack(username, password, client_ip);
            if (nack == NackCode::SUCCESS) {
                auto result = HandleLogin(username, password, client_ip);
                flatbuffers::FlatBufferBuilder fbb;
                auto resp = CreateLoginResponse(fbb,
                    LoginResult_Success,
                    result.success ? fbb.CreateString(result.session_token) : 0);
                fbb.Finish(resp);
                network_->SendPacket(PacketType_MP_USERCONN_LOGIN_ACK,
                    fbb.GetBufferPointer(), fbb.GetSize());
                brute_force_.RecordSuccess(ip_key);
                brute_force_.RecordSuccess(user_key);
            } else {
                brute_force_.RecordFailure(ip_key);
                brute_force_.RecordFailure(user_key);
                flatbuffers::FlatBufferBuilder fbb;
                luna::protocol::LoginResult fb_result = LoginResult_InvalidCredentials;
                switch (nack) {
                case NackCode::BANNED:          fb_result = LoginResult_Banned; break;
                case NackCode::MAINTENANCE:     fb_result = LoginResult_Maintenance; break;
                case NackCode::VERSION_MISMATCH: fb_result = LoginResult_VersionMismatch; break;
                case NackCode::ALREADY_LOGGED_IN: fb_result = LoginResult_AlreadyLoggedIn; break;
                default:                        fb_result = LoginResult_InvalidCredentials; break;
                }
                auto resp = CreateLoginResponse(fbb, fb_result, 0);
                fbb.Finish(resp);
                network_->SendPacket(PacketType_MP_USERCONN_LOGIN_NACK,
                    fbb.GetBufferPointer(), fbb.GetSize());
            }
            break;
        }
        case PacketType_MP_USERCONN_CHARACTERLIST_SYN: {
            auto req = flatbuffers::GetRoot<CharacterListRequest>(payload);
            std::string token = req->session_token() ? req->session_token()->str() : "";
            int account_id = ValidateSession(token);
            if (account_id < 0) {
                flatbuffers::FlatBufferBuilder fbb;
                auto resp = CreateCharacterListResponseDirect(fbb, nullptr, 0);
                fbb.Finish(resp);
                network_->SendPacket(PacketType_MP_USERCONN_CHARACTERLIST_NACK,
                    fbb.GetBufferPointer(), fbb.GetSize());
                break;
            }
            auto chars = HandleCharacterList(token);
            flatbuffers::FlatBufferBuilder fbb;
            std::vector<flatbuffers::Offset<CharacterInfo>> fb_chars;
            for (auto& c : chars) {
                Vec3 pos(c.pos_x, c.pos_y, c.pos_z);
                fb_chars.push_back(CreateCharacterInfoDirect(fbb,
                    c.id, c.name.c_str(), c.level, c.class_id, c.gender,
                    c.map_id, &pos, c.hp, c.max_hp,
                    static_cast<uint8_t>(c.body_type),
                    static_cast<uint8_t>(c.face_id),
                    static_cast<uint8_t>(c.hair_id),
                    static_cast<uint32_t>(c.weapon_id),
                    static_cast<uint32_t>(c.dress_id),
                    static_cast<uint32_t>(c.shoes_id),
                    static_cast<uint32_t>(c.glove_id),
                    static_cast<uint8_t>(c.job_grade),
                    static_cast<uint8_t>(c.job[0]),
                    static_cast<uint8_t>(c.job[1]),
                    static_cast<uint8_t>(c.job[2]),
                    static_cast<uint8_t>(c.job[3]),
                    static_cast<uint8_t>(c.job[4]),
                    static_cast<uint8_t>(c.job[5]),
                    c.guild_name.c_str()));
            }
            auto resp = CreateCharacterListResponseDirect(fbb, &fb_chars, 4);
            fbb.Finish(resp);
            network_->SendPacket(PacketType_MP_USERCONN_CHARACTERLIST_ACK,
                fbb.GetBufferPointer(), fbb.GetSize());
            break;
        }
        case PacketType_MP_USERCONN_CHARACTER_MAKE_SYN: {
            auto req = flatbuffers::GetRoot<CreateCharacterRequest>(payload);
            std::string token = req->session_token() ? req->session_token()->str() : "";
            std::string name = req->name() ? req->name()->str() : "NewChar";
            int class_id = req->class_();
            int gender = req->gender();
            int body_type = req->body_type();
            int face_id = req->face_id();
            int hair_id = req->hair_id();
            bool ok = HandleCreateCharacter(token, name, class_id, gender, body_type, face_id, hair_id);

            flatbuffers::FlatBufferBuilder fbb;
            auto result_status = ok ? 0 : 1;
            CharacterInfoBuilder ci(fbb);
            ci.add_id(0);
            ci.add_name(fbb.CreateString(name));
            ci.add_level(1);
            ci.add_class_(static_cast<uint16_t>(class_id));
            ci.add_gender(static_cast<uint8_t>(gender));
            ci.add_map_id(51);
            ci.add_body_type(static_cast<uint8_t>(body_type));
            ci.add_face_id(static_cast<uint8_t>(face_id));
            ci.add_hair_id(static_cast<uint8_t>(hair_id));
            auto resp = CreateCreateCharacterResponse(fbb, result_status, ci.Finish());
            fbb.Finish(resp);
            if (ok) {
                network_->SendPacket(PacketType_MP_USERCONN_CHARACTER_MAKE_ACK,
                    fbb.GetBufferPointer(), fbb.GetSize());
            } else {
                network_->SendPacket(PacketType_MP_USERCONN_CHARACTER_MAKE_NACK,
                    fbb.GetBufferPointer(), fbb.GetSize());
            }
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
            if (ok) {
                network_->SendPacket(PacketType_MP_USERCONN_CHARACTER_DELETE_ACK,
                    fbb.GetBufferPointer(), fbb.GetSize());
            } else {
                network_->SendPacket(PacketType_MP_USERCONN_CHARACTER_REMOVE_NACK,
                    fbb.GetBufferPointer(), fbb.GetSize());
            }
            break;
        }
        case PacketType_MP_USERCONN_GAMEIN_SYN: {
            auto req = flatbuffers::GetRoot<EnterWorldRequest>(payload);
            std::string token = req->session_token() ? req->session_token()->str() : "";
            CharData ch{};
            bool ok = HandleEnterWorld(token, static_cast<int>(req->character_id()), ch);
            flatbuffers::FlatBufferBuilder fbb;
            Vec3 pos(ch.pos_x, ch.pos_y, ch.pos_z);
            auto resp = CreateEnterWorldResponse(fbb,
                ok ? 0 : 1,
                static_cast<uint16_t>(ch.map_id ? ch.map_id : 51),
                &pos,
                static_cast<uint32_t>(ch.id));
            fbb.Finish(resp);
            network_->SendPacket(ok ? PacketType_MP_USERCONN_GAMEIN_ACK : PacketType_MP_USERCONN_GAMEIN_NACK,
                fbb.GetBufferPointer(), fbb.GetSize());
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

LoginResult AgentServer::HandleLogin(const std::string& username, const std::string& password,
                                      const std::string& client_ip) {
    LoginResult result;
    auto rows = db_->Query(
        "SELECT id, password_hash, banned FROM accounts WHERE username='" + username + "'");
    if (rows.empty()) {
        result.message = "Invalid username or password";
        return result;
    }
    int account_id = std::stoi(rows[0][0]);
    std::string pw_hash = rows[0][1];
    int banned = std::stoi(rows[0][2]);

    if (banned) {
        result.message = "Account is banned";
        return result;
    }

    if (!BcryptUtils::Verify(password, pw_hash)) {
        result.message = "Invalid username or password";
        return result;
    }

    result.success = true;
    result.account_id = account_id;
    result.session_token = CreateSession(account_id);
    result.message = "Login successful";
    return result;
}

NackCode AgentServer::HandleLoginNack(const std::string& username, const std::string& password,
                                       const std::string& client_ip) {
    auto rows = db_->Query(
        "SELECT id, password_hash, banned FROM accounts WHERE username='" + username + "'");
    if (rows.empty()) {
        return NackCode::INVALID_CREDENTIALS;
    }
    int account_id = std::stoi(rows[0][0]);
    std::string pw_hash = rows[0][1];
    int banned = std::stoi(rows[0][2]);

    if (banned) {
        return NackCode::BANNED;
    }

    if (!BcryptUtils::Verify(password, pw_hash)) {
        return NackCode::INVALID_CREDENTIALS;
    }

    for (auto& [token, session] : sessions_) {
        if (session.account_id == account_id) {
            return NackCode::ALREADY_LOGGED_IN;
        }
    }

    return NackCode::SUCCESS;
}

std::vector<CharData> AgentServer::HandleCharacterList(const std::string& session_token) {
    int account_id = ValidateSession(session_token);
    if (account_id < 0) return {};
    auto rows = db_->Query(
        "SELECT id, name, level, class_id, gender, map_id, pos_x, pos_y, pos_z, hp, max_hp, "
        "body_type, face_id, hair_id, weapon_id, dress_id, shoes_id, glove_id, "
        "job_grade, job1, job2, job3, job4, job5, job6, guild_name "
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
        ci.body_type = row.size() > 11 ? std::stoi(row[11]) : 0;
        ci.face_id = row.size() > 12 ? std::stoi(row[12]) : 0;
        ci.hair_id = row.size() > 13 ? std::stoi(row[13]) : 0;
        ci.weapon_id = row.size() > 14 ? std::stoi(row[14]) : 0;
        ci.dress_id = row.size() > 15 ? std::stoi(row[15]) : 0;
        ci.shoes_id = row.size() > 16 ? std::stoi(row[16]) : 0;
        ci.glove_id = row.size() > 17 ? std::stoi(row[17]) : 0;
        ci.job_grade = row.size() > 18 ? std::stoi(row[18]) : 0;
        for (int j = 0; j < 6; j++) {
            ci.job[j] = row.size() > static_cast<size_t>(19 + j) ? std::stoi(row[19 + j]) : 0;
        }
        ci.guild_name = row.size() > 25 ? row[25] : "";
        chars.push_back(std::move(ci));
    }
    return chars;
}

bool AgentServer::HandleCreateCharacter(const std::string& session_token, const std::string& name,
                                         int class_id, int gender, int body_type, int face_id,
                                         int hair_id) {
    int account_id = ValidateSession(session_token);
    if (account_id < 0) return false;
    auto existing = db_->Query("SELECT id FROM characters WHERE name='" + name + "'");
    if (!existing.empty()) return false;
    auto count = db_->Query("SELECT COUNT(*) FROM characters WHERE account_id=" + std::to_string(account_id));
    if (!count.empty() && std::stoi(count[0][0]) >= 4) return false;
    std::string sql =
        "INSERT INTO characters (account_id, name, class_id, gender, body_type, face_id, hair_id) VALUES (" +
        std::to_string(account_id) + ",'" + name + "'," +
        std::to_string(class_id) + "," + std::to_string(gender) + "," +
        std::to_string(body_type) + "," + std::to_string(face_id) + "," +
        std::to_string(hair_id) + ")";
    return db_->Execute(sql);
}

bool AgentServer::HandleCharacterNameCheck(const std::string& session_token, const std::string& name) {
    int account_id = ValidateSession(session_token);
    if (account_id < 0) return false;
    if (name.empty() || name.size() > 16) return false;
    auto existing = db_->Query("SELECT id FROM characters WHERE name='" + name + "'");
    return existing.empty();
}

bool AgentServer::HandleDeleteCharacter(const std::string& session_token, int char_id) {
    int account_id = ValidateSession(session_token);
    if (account_id < 0) return false;
    return db_->Execute(
        "DELETE FROM characters WHERE id=" + std::to_string(char_id) +
        " AND account_id=" + std::to_string(account_id));
}

bool AgentServer::HandleEnterWorld(const std::string& session_token, int char_id, CharData& out) {
    int account_id = ValidateSession(session_token);
    if (account_id < 0) return false;
    auto rows = db_->Query(
        "SELECT id, name, level, class_id, gender, map_id, pos_x, pos_y, pos_z, hp, max_hp "
        "FROM characters WHERE id=" + std::to_string(char_id) +
        " AND account_id=" + std::to_string(account_id));
    if (rows.empty()) return false;
    out.id = std::stoi(rows[0][0]);
    out.name = rows[0][1];
    out.level = std::stoi(rows[0][2]);
    out.class_id = std::stoi(rows[0][3]);
    out.gender = std::stoi(rows[0][4]);
    out.map_id = std::stoi(rows[0][5]);
    out.pos_x = std::stof(rows[0][6]);
    out.pos_y = std::stof(rows[0][7]);
    out.pos_z = std::stof(rows[0][8]);
    out.hp = std::stoi(rows[0][9]);
    out.max_hp = std::stoi(rows[0][10]);
    return true;
}

std::string AgentServer::CreateSession(int account_id) {
    std::string token = GenerateUUID() + "-" + std::to_string(std::time(nullptr));
    sessions_[token] = {account_id, token, "", GetClientIp(), std::chrono::steady_clock::now()};
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

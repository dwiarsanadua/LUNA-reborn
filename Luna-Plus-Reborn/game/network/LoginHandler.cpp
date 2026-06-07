#include "LoginHandler.hpp"
#include "TcpServer.hpp"
#include "Login_generated.h"
#include "Types_generated.h"
#include <spdlog/spdlog.h>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <random>

using namespace luna::protocol;

static std::mt19937 token_rng(std::random_device{}());

LoginHandler::LoginHandler() = default;
LoginHandler::~LoginHandler() { if (db_) sqlite3_close(db_); }

bool LoginHandler::Init(const std::string& db_path) {
    if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK) {
        spdlog::error("LoginHandler: failed to open {}", db_path);
        return false;
    }
    spdlog::info("LoginHandler: account DB opened");
    return true;
}

bool LoginHandler::VerifyPassword(const std::string& username, const std::string& password_hash) {
    if (!db_) return false;
    const char* sql = "SELECT Password FROM AccountInfo WHERE AccountID = ? AND Blocked = 0";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        auto pw = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (pw && password_hash == pw) ok = true;
    }
    sqlite3_finalize(stmt);
    return ok;
}

std::string LoginHandler::GenerateToken() {
    std::stringstream ss;
    for (int i = 0; i < 8; i++)
        ss << std::hex << std::setw(8) << std::setfill('0') << token_rng();
    return ss.str();
}

bool LoginHandler::HandleLogin(std::shared_ptr<TcpConnection> conn, const uint8_t* data, size_t len,
                                std::string* out_token) {
    auto req = flatbuffers::GetRoot<LoginRequest>(data);
    std::string username = req->username() ? req->username()->str() : "";
    spdlog::info("Login attempt: user='{}' from conn={}", username, conn->id);

    LoginResult result = LoginResult_InvalidCredentials;
    std::string token;

    auto pw_vec = req->password_hash();
    if (pw_vec && pw_vec->size() > 0) {
        std::string pw_hash(reinterpret_cast<const char*>(pw_vec->data()), pw_vec->size());
        if (VerifyPassword(username, pw_hash)) {
            result = LoginResult_Success;
            token = GenerateToken();
            spdlog::info("Login SUCCESS: {}", username);
        } else {
            spdlog::warn("Login FAILED: {} (bad password)", username);
        }
    }

    flatbuffers::FlatBufferBuilder fbb;
    auto token_str = fbb.CreateString(token);
    auto response = CreateLoginResponse(fbb, result, token_str);
    fbb.Finish(response);

    PacketHeader hdr;
    hdr.magic = 0x4C4E50;
    hdr.length = static_cast<uint16_t>(fbb.GetSize());
    hdr.type = 0x0102;
    hdr.sequence = 0;
    hdr.checksum = 0;

    auto& buf = conn->write_buffer;
    buf.clear();
    buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&hdr), reinterpret_cast<uint8_t*>(&hdr) + sizeof(hdr));
    buf.insert(buf.end(), fbb.GetBufferPointer(), fbb.GetBufferPointer() + fbb.GetSize());

    if (out_token && result == LoginResult_Success) *out_token = token;
    return result == LoginResult_Success;
}

void LoginHandler::HandleLogout(uint32_t conn_id) {
    spdlog::info("Logout: conn={}", conn_id);
}

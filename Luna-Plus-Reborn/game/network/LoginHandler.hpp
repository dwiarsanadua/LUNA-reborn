#pragma once
#include <memory>
#include <string>
#include <sqlite3.h>

struct TcpConnection;

class LoginHandler {
public:
    LoginHandler();
    ~LoginHandler();

    bool Init(const std::string& db_path);
    bool HandleLogin(std::shared_ptr<TcpConnection> conn, const uint8_t* data, size_t len,
                     std::string* out_token = nullptr);
    void HandleLogout(uint32_t conn_id);

private:
    sqlite3* db_ = nullptr;
    bool VerifyPassword(const std::string& username, const std::string& password_hash);
    std::string GenerateToken();
};

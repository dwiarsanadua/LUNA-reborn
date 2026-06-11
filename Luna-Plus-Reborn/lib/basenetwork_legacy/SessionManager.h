#pragma once
#include <memory>
#include <unordered_map>
#include <cstdint>

class TcpConnection;

class SessionManager {
public:
    SessionManager();
    ~SessionManager();

    uint32_t AddSession(std::unique_ptr<TcpConnection> conn);
    bool RemoveSession(uint32_t id);
    TcpConnection* GetSession(uint32_t id);
    void CloseAll();
    void Broadcast(const char* data, size_t length);

    size_t GetSessionCount() const { return sessions_.size(); }

private:
    std::unordered_map<uint32_t, std::unique_ptr<TcpConnection>> sessions_;
    uint32_t next_id_ = 1;
};

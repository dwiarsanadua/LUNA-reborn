#include "SessionManager.h"
#include "TcpConnection.h"
#include <spdlog/spdlog.h>

SessionManager::SessionManager() = default;

SessionManager::~SessionManager() {
    CloseAll();
}

uint32_t SessionManager::AddSession(std::unique_ptr<TcpConnection> conn) {
    uint32_t id = next_id_++;
    conn->SetId(id);
    sessions_[id] = std::move(conn);
    spdlog::debug("SessionManager: added session {}", id);
    return id;
}

bool SessionManager::RemoveSession(uint32_t id) {
    auto it = sessions_.find(id);
    if (it == sessions_.end()) return false;
    it->second->Close();
    sessions_.erase(it);
    spdlog::debug("SessionManager: removed session {}", id);
    return true;
}

TcpConnection* SessionManager::GetSession(uint32_t id) {
    auto it = sessions_.find(id);
    return (it != sessions_.end()) ? it->second.get() : nullptr;
}

void SessionManager::CloseAll() {
    for (auto& [id, conn] : sessions_) {
        conn->Close();
    }
    sessions_.clear();
    spdlog::info("SessionManager: all sessions closed");
}

void SessionManager::Broadcast(const char* data, size_t length) {
    for (auto& [id, conn] : sessions_) {
        conn->Send(data, length);
    }
}

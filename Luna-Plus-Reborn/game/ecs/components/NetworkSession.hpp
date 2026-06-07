#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include <string>

struct NetworkSession {
    uint32_t session_id = 0;
    uint32_t account_id = 0;
    std::string username;
    bool is_authenticated = false;
    void* tcp_handle = nullptr;
    std::vector<uint8_t> send_buffer;
    std::vector<uint8_t> recv_buffer;
    float last_ping_time = 0.0f;
    float disconnect_timeout = 30.0f;
    bool is_connected = true;
    std::function<void(uint32_t)> on_disconnect;
};

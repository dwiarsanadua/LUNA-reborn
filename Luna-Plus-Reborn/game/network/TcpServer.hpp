#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>

#pragma pack(push, 1)
struct PacketHeader {
    uint32_t magic;     // 0x4C4E50 = "LNP"
    uint16_t length;    // payload length
    uint16_t type;      // PacketType enum
    uint16_t sequence;
    uint32_t checksum;
};
#pragma pack(pop)

struct TcpConnection {
    void* handle = nullptr;
    uint32_t id = 0;
    std::vector<uint8_t> read_buffer;
    std::vector<uint8_t> write_buffer;
    bool closing = false;
    int packet_count = 0;
    std::chrono::steady_clock::time_point rate_start = std::chrono::steady_clock::now();
};

class TcpServer {
public:
    using NewConnectionCallback = std::function<void(std::shared_ptr<TcpConnection>)>;
    using DataCallback = std::function<void(std::shared_ptr<TcpConnection>, uint8_t* data, size_t len, uint16_t type)>;
    using CloseCallback = std::function<void(std::shared_ptr<TcpConnection>)>;

    TcpServer();
    ~TcpServer();

    bool Start(uint16_t port);
    void Stop();
    void Poll();
    bool IsRunning() const { return running_; }
    void Send(std::shared_ptr<TcpConnection> conn, const uint8_t* data, size_t len);

    void SetNewConnectionCallback(NewConnectionCallback cb) { on_new_connection_ = cb; }
    void SetDataCallback(DataCallback cb) { on_data_ = cb; }
    void SetCloseCallback(CloseCallback cb) { on_close_ = cb; }
    void SetMaxPacketsPerSec(int max) { max_packets_per_sec_ = max; }
    int GetMaxPacketsPerSec() const { return max_packets_per_sec_; }

    uint32_t NextId();
    void OnNewConnection(std::shared_ptr<TcpConnection> c);
    void OnData(std::shared_ptr<TcpConnection> c, uint8_t* d, size_t l, uint16_t t);
    void OnClose(std::shared_ptr<TcpConnection> c);

private:
    void* loop_ = nullptr;
    void* server_ = nullptr;
    uint32_t next_id_ = 1;
    bool running_ = false;
    int max_packets_per_sec_ = 60;

    NewConnectionCallback on_new_connection_;
    DataCallback on_data_;
    CloseCallback on_close_;
};

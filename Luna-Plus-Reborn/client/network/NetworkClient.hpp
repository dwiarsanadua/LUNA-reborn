#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <memory>
#include <thread>

#pragma pack(push, 1)
struct PacketHeader {
    uint32_t magic;     // 0x4C4E50
    uint16_t length;    // payload length
    uint16_t type;      // PacketType enum
    uint16_t sequence;
    uint32_t checksum;
};
#pragma pack(pop)

class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();

    bool Connect(const std::string& host, uint16_t port);
    void Disconnect();
    bool IsConnected() const { return connected_; }
    bool Login(const std::string& username, const std::string& password);

    void Send(const uint8_t* data, size_t len);
    void SendPacket(uint16_t type, const uint8_t* payload, size_t len);

    using PacketHandler = std::function<void(uint16_t type, const std::vector<uint8_t>&)>;
    void SetPacketHandler(PacketHandler handler) { handler_ = handler; }

private:
    void ReadThread();
    bool connected_ = false;
    int sock_ = -1;
    std::vector<uint8_t> read_buf_;
    std::unique_ptr<std::thread> read_thread_;
    PacketHandler handler_;
    uint32_t login_result_ = 0;
    std::string session_token_;
};


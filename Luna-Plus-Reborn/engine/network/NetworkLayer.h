// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <memory>

struct PacketHeader {
    uint32_t magic;      // 0x4C4E50 = "LNP"
    uint32_t length;     // payload length
    uint16_t type;       // packet type
    uint32_t sequence;   // sequence number
    uint32_t crc32;      // integrity check
    // payload follows
};

class NetworkLayer {
public:
    using ReceiveCallback = std::function<void(const uint8_t* data, size_t size)>;

    NetworkLayer();
    ~NetworkLayer();

    bool Initialize(uint16_t port);
    void Shutdown();
    bool Connect(const std::string& host, uint16_t port);
    void Disconnect();
    void Send(const uint8_t* data, size_t size);
    void SetReceiveCallback(ReceiveCallback cb);
    void Update();
    bool IsConnected() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

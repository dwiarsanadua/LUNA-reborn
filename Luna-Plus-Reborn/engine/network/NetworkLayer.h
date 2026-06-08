// AGENT Nexus — DO NOT MODIFY WITHOUT COORDINATION
#pragma once
#include "LunaPacket.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <memory>

class NetworkLayer {
public:
    using ReceiveCallback = std::function<void(uint16_t type, const uint8_t* data, size_t size)>;

    NetworkLayer();
    ~NetworkLayer();

    bool Initialize(uint16_t port);
    void Shutdown();
    bool Connect(const std::string& host, uint16_t port);
    void Disconnect();
    void Send(const uint8_t* data, size_t size);
    void SendPacket(uint16_t type, const uint8_t* payload, size_t len);
    void SetReceiveCallback(ReceiveCallback cb);
    void Update();
    bool IsConnected() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

#include "NetworkClient.hpp"
#include <spdlog/spdlog.h>
#include <flatbuffers/flatbuffers.h>
#include <Login_generated.h>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

NetworkClient::NetworkClient() = default;
NetworkClient::~NetworkClient() { Disconnect(); }

bool NetworkClient::Connect(const std::string& host, uint16_t port) {
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) { spdlog::error("NetworkClient: socket failed"); return false; }

    struct hostent* server = gethostbyname(host.c_str());
    if (!server) { spdlog::error("NetworkClient: host lookup failed"); close(sock_); return false; }

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    std::memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);
    addr.sin_port = htons(port);

    if (connect(sock_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        spdlog::error("NetworkClient: connect failed"); close(sock_); return false;
    }

    connected_ = true;
    spdlog::info("NetworkClient: connected to {}:{}", host, port);
    read_thread_ = std::make_unique<std::thread>(&NetworkClient::ReadThread, this);
    return true;
}

void NetworkClient::Disconnect() {
    connected_ = false;
    if (sock_ >= 0) { close(sock_); sock_ = -1; }
    if (read_thread_ && read_thread_->joinable()) read_thread_->join();
}

void NetworkClient::Send(const uint8_t* data, size_t len) {
    if (!connected_) return;
    ::send(sock_, data, len, 0);
}

void NetworkClient::SendPacket(uint16_t type, const uint8_t* payload, size_t len) {
    PacketHeader hdr{};
    hdr.magic = 0x4C4E50;
    hdr.length = static_cast<uint16_t>(len);
    hdr.type = type;
    hdr.sequence = 0;
    hdr.checksum = 0;
    Send(reinterpret_cast<uint8_t*>(&hdr), sizeof(hdr));
    if (payload && len > 0) Send(payload, len);
}

bool NetworkClient::Login(const std::string& username, const std::string& password) {
    if (!Connect("127.0.0.1", 8100)) return false;
    std::vector<uint8_t> pv(password.begin(), password.end());
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateLoginRequestDirect(fbb, username.c_str(), &pv);
    fbb.Finish(req);
    SendPacket(0x0101, fbb.GetBufferPointer(), fbb.GetSize());
    return true;
}

void NetworkClient::ReadThread() {
    std::vector<uint8_t> buf;
    buf.resize(4096);
    while (connected_) {
        ssize_t n = ::recv(sock_, buf.data(), buf.size(), 0);
        if (n <= 0) { connected_ = false; break; }
        read_buf_.insert(read_buf_.end(), buf.data(), buf.data() + n);
        while (read_buf_.size() >= sizeof(PacketHeader)) {
            PacketHeader hdr;
            std::memcpy(&hdr, read_buf_.data(), sizeof(hdr));
            if (hdr.magic != 0x4C4E50) { read_buf_.clear(); break; }
            size_t total = sizeof(PacketHeader) + hdr.length;
            if (read_buf_.size() < total) break;
            if (handler_) {
                std::vector<uint8_t> payload(read_buf_.begin() + sizeof(PacketHeader),
                                              read_buf_.begin() + total);
                handler_(hdr.type, payload);
            }
            read_buf_.erase(read_buf_.begin(), read_buf_.begin() + total);
        }
    }
}

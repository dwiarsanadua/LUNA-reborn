#include "NetworkClient.hpp"
#include <spdlog/spdlog.h>
#include <flatbuffers/flatbuffers.h>
#include <Login_generated.h>
#include <PacketType_generated.h>
#include <cstring>
#include <atomic>
#include <thread>
#include <chrono>

static uint32_t crc32_table[256];
static bool crc32_ready = false;

static void InitCrc32() {
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++)
            crc = (crc & 1) ? (crc >> 1) ^ 0xEDB88320u : (crc >> 1);
        crc32_table[i] = crc;
    }
    crc32_ready = true;
}

static uint32_t PacketCrc32(const uint8_t* data, size_t size) {
    if (!crc32_ready) InitCrc32();
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < size; i++)
        crc = crc32_table[(crc ^ data[i]) & 0xFFu] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFu;
}
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// ── Reconnect system ──
// Uses file-scope state since NetworkClient.hpp cannot be modified.
static const int RECONNECT_MAX_RETRIES = 3;
static const int RECONNECT_DELAY_SEC = 10;

enum class ReconnectState {
    Idle,
    Waiting,
    Connecting,
    Failed,
};

static struct {
    ReconnectState state = ReconnectState::Idle;
    int retry_count = 0;
    float countdown = 0.0f;
    std::string host;
    uint16_t port = 0;
    std::thread timer_thread;
    std::atomic<bool> abort_flag{false};
    std::function<void(int, int)> notify_callback; // (retry_count, max_retries)
} g_reconnect;

static void ReconnectTimerThread(NetworkClient* client) {
    while (!g_reconnect.abort_flag && g_reconnect.state == ReconnectState::Waiting) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (g_reconnect.abort_flag) return;
        g_reconnect.countdown -= 0.1f;
        if (g_reconnect.notify_callback) {
            g_reconnect.notify_callback(g_reconnect.retry_count, RECONNECT_MAX_RETRIES);
        }
        if (g_reconnect.countdown <= 0.0f) {
            g_reconnect.state = ReconnectState::Connecting;
            if (!g_reconnect.host.empty()) {
                bool ok = client->Connect(g_reconnect.host, g_reconnect.port);
                if (ok) {
                    g_reconnect.state = ReconnectState::Idle;
                    g_reconnect.retry_count = 0;
                    if (g_reconnect.notify_callback)
                        g_reconnect.notify_callback(0, RECONNECT_MAX_RETRIES);
                    return;
                }
            }
            g_reconnect.retry_count++;
            if (g_reconnect.retry_count >= RECONNECT_MAX_RETRIES) {
                g_reconnect.state = ReconnectState::Failed;
                if (g_reconnect.notify_callback)
                    g_reconnect.notify_callback(RECONNECT_MAX_RETRIES, RECONNECT_MAX_RETRIES);
                return;
            }
            g_reconnect.countdown = static_cast<float>(RECONNECT_DELAY_SEC);
            g_reconnect.state = ReconnectState::Waiting;
        }
    }
}

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
    g_reconnect.abort_flag = true;
    if (g_reconnect.timer_thread.joinable()) g_reconnect.timer_thread.join();
    g_reconnect.state = ReconnectState::Idle;
    g_reconnect.retry_count = 0;
    g_reconnect.countdown = 0.0f;
    g_reconnect.abort_flag = false;

    connected_ = false;
    if (sock_ >= 0) { close(sock_); sock_ = -1; }
    if (read_thread_ && read_thread_->joinable()) read_thread_->join();
}

void NetworkClient::Send(const uint8_t* data, size_t len) {
    if (!connected_) return;
    ::send(sock_, data, len, 0);
}

void NetworkClient::SendPacket(uint16_t type, const uint8_t* payload, size_t len) {
    LunaPacketHeader hdr{};
    hdr.magic = LUNA_PACKET_MAGIC;
    hdr.length = static_cast<uint16_t>(len);
    hdr.type = type;
    hdr.sequence = 0;
    hdr.checksum = (payload && len > 0) ? PacketCrc32(payload, len) : 0;
    Send(reinterpret_cast<uint8_t*>(&hdr), LUNA_PACKET_HEADER_SIZE);
    if (payload && len > 0) Send(payload, len);
}

bool NetworkClient::Login(const std::string& username, const std::string& password) {
    if (!Connect("127.0.0.1", 8100)) return false;
    std::vector<uint8_t> pv(password.begin(), password.end());
    flatbuffers::FlatBufferBuilder fbb;
    auto req = luna::protocol::CreateLoginRequestDirect(fbb, username.c_str(), &pv);
    fbb.Finish(req);
    SendPacket(luna::protocol::PacketType_MP_USERCONN_LOGIN_SYN, fbb.GetBufferPointer(), fbb.GetSize());
    return true;
}

void NetworkClient::ProcessEvents() {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    while (!event_queue_.empty()) {
        auto evt = std::move(event_queue_.front());
        event_queue_.pop();
        lock.unlock();
        if (handler_) handler_(evt.type, evt.payload);
        lock.lock();
    }
}

void NetworkClient::ReadThread() {
    g_reconnect.host = "127.0.0.1";
    g_reconnect.port = 8100;

    std::vector<uint8_t> buf(4096);
    while (connected_) {
        ssize_t n = ::recv(sock_, buf.data(), buf.size(), 0);
        if (n <= 0) {
            connected_ = false;
            break;
        }
        read_buf_.insert(read_buf_.end(), buf.data(), buf.data() + n);
        while (read_buf_.size() >= LUNA_PACKET_HEADER_SIZE) {
            LunaPacketHeader hdr;
            std::memcpy(&hdr, read_buf_.data(), LUNA_PACKET_HEADER_SIZE);
            if (hdr.magic != LUNA_PACKET_MAGIC) { read_buf_.clear(); break; }
            size_t total = LUNA_PACKET_HEADER_SIZE + hdr.length;
            if (read_buf_.size() < total) break;
            const uint8_t* body = read_buf_.data() + LUNA_PACKET_HEADER_SIZE;
            if (hdr.length > 0 && PacketCrc32(body, hdr.length) != hdr.checksum) {
                read_buf_.erase(read_buf_.begin(), read_buf_.begin() + total);
                continue;
            }
            std::vector<uint8_t> payload(body, body + hdr.length);
            read_buf_.erase(read_buf_.begin(), read_buf_.begin() + total);
            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                event_queue_.push({hdr.type, std::move(payload)});
            }
        }
    }

    // Start reconnect sequence on disconnect
    if (g_reconnect.state == ReconnectState::Idle && !g_reconnect.abort_flag) {
        g_reconnect.state = ReconnectState::Waiting;
        g_reconnect.retry_count = 0;
        g_reconnect.countdown = static_cast<float>(RECONNECT_DELAY_SEC);
        g_reconnect.abort_flag = false;
        g_reconnect.host = "127.0.0.1";
        g_reconnect.port = 8100;

        if (g_reconnect.timer_thread.joinable()) g_reconnect.timer_thread.join();
        g_reconnect.timer_thread = std::thread(ReconnectTimerThread, this);
    }
}

// ── Public reconnect query API (free functions, called from main thread) ──

std::string NetworkClient_GetReconnectStatus() {
    switch (g_reconnect.state) {
        case ReconnectState::Idle: return "";
        case ReconnectState::Waiting: {
            char buf[64];
            snprintf(buf, sizeof(buf), "Reconnecting in %.0fs... (%d/%d)",
                     g_reconnect.countdown, g_reconnect.retry_count + 1, RECONNECT_MAX_RETRIES);
            return buf;
        }
        case ReconnectState::Connecting: return "Connecting...";
        case ReconnectState::Failed: return "Reconnect failed. Returning to login.";
    }
    return "";
}

void NetworkClient_SetReconnectCallback(std::function<void(int, int)> cb) {
    g_reconnect.notify_callback = std::move(cb);
}

void NetworkClient_CancelReconnect() {
    g_reconnect.abort_flag = true;
    if (g_reconnect.timer_thread.joinable()) g_reconnect.timer_thread.join();
    g_reconnect.state = ReconnectState::Idle;
    g_reconnect.retry_count = 0;
    g_reconnect.countdown = 0.0f;
    g_reconnect.abort_flag = false;
}

#include "TcpServer.hpp"
#include <uv.h>
#include <spdlog/spdlog.h>
#include <cstring>

struct uv_ctx {
    uv_loop_t loop;
    uv_tcp_t server;
    TcpServer* tcp_server;
};

struct conn_ctx {
    uv_tcp_t handle;
    TcpServer* tcp_server;
    std::shared_ptr<TcpConnection> conn;
};

static void alloc_buffer(uv_handle_t*, size_t suggested, uv_buf_t* buf) {
    buf->base = new char[suggested];
    buf->len = suggested;
}

static bool check_rate_limit(std::shared_ptr<TcpConnection> conn, int max_per_sec) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<float>(now - conn->rate_start).count();
    if (elapsed >= 1.0f) {
        conn->packet_count = 0;
        conn->rate_start = now;
    }
    conn->packet_count++;
    return conn->packet_count <= max_per_sec;
}

static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    auto ctx = static_cast<conn_ctx*>(stream->data);
    if (nread > 0) {
        ctx->conn->read_buffer.insert(ctx->conn->read_buffer.end(),
                                       buf->base, buf->base + nread);
        while (ctx->conn->read_buffer.size() >= sizeof(PacketHeader)) {
            PacketHeader hdr;
            std::memcpy(&hdr, ctx->conn->read_buffer.data(), sizeof(hdr));
            if (hdr.magic != 0x4C4E50) {
                spdlog::error("Bad magic from conn {}", ctx->conn->id);
                uv_close(reinterpret_cast<uv_handle_t*>(&ctx->handle), nullptr);
                break;
            }
            if (hdr.length > 65535) {
                spdlog::error("Oversized packet from conn {}", ctx->conn->id);
                uv_close(reinterpret_cast<uv_handle_t*>(&ctx->handle), nullptr);
                break;
            }
            size_t total = sizeof(PacketHeader) + hdr.length;
            if (ctx->conn->read_buffer.size() < total) break;
            if (ctx->tcp_server && ctx->conn) {
                if (check_rate_limit(ctx->conn, ctx->tcp_server->GetMaxPacketsPerSec())) {
                    ctx->tcp_server->OnData(ctx->conn,
                        ctx->conn->read_buffer.data() + sizeof(PacketHeader),
                        hdr.length, hdr.type);
                } else {
                    spdlog::warn("Rate limit exceeded for conn {}", ctx->conn->id);
                    uv_close(reinterpret_cast<uv_handle_t*>(&ctx->handle), nullptr);
                    break;
                }
            }
            ctx->conn->read_buffer.erase(ctx->conn->read_buffer.begin(),
                                          ctx->conn->read_buffer.begin() + total);
        }
    }
    delete[] buf->base;
    if (nread < 0) {
        if (ctx->tcp_server) ctx->tcp_server->OnClose(ctx->conn);
        delete ctx;
    }
}

static void on_write(uv_write_t* req, int status) {
    delete[] static_cast<uint8_t*>(req->data);
    delete req;
}

static void on_connection(uv_stream_t* server, int status) {
    if (status < 0) return;
    auto ctx = static_cast<uv_ctx*>(server->data);
    auto client = new conn_ctx;
    uv_tcp_init(&ctx->loop, &client->handle);
    client->tcp_server = ctx->tcp_server;
    if (uv_accept(server, reinterpret_cast<uv_stream_t*>(&client->handle)) == 0) {
        auto conn = std::make_shared<TcpConnection>();
        conn->handle = &client->handle;
        conn->id = ctx->tcp_server->NextId();
        client->conn = conn;
        client->handle.data = client;
        uv_read_start(reinterpret_cast<uv_stream_t*>(&client->handle), alloc_buffer, on_read);
        if (ctx->tcp_server) ctx->tcp_server->OnNewConnection(conn);
    } else {
        delete client;
    }
}

TcpServer::TcpServer() = default;
TcpServer::~TcpServer() { Stop(); }

bool TcpServer::Start(uint16_t port) {
    auto ctx = new uv_ctx;
    ctx->tcp_server = this;
    uv_loop_init(&ctx->loop);
    uv_tcp_init(&ctx->loop, &ctx->server);
    ctx->server.data = ctx;
    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", port, &addr);
    uv_tcp_bind(&ctx->server, reinterpret_cast<const sockaddr*>(&addr), 0);
    int r = uv_listen(reinterpret_cast<uv_stream_t*>(&ctx->server), 128, on_connection);
    if (r) {
        spdlog::error("TcpServer: listen failed on port {}", port);
        uv_loop_close(&ctx->loop);
        delete ctx;
        return false;
    }
    loop_ = ctx;
    running_ = true;
    spdlog::info("TcpServer: listening on port {}", port);
    return true;
}

void TcpServer::Poll() {
    if (loop_) {
        uv_run(&static_cast<uv_ctx*>(loop_)->loop, UV_RUN_NOWAIT);
    }
}

void TcpServer::Stop() {
    if (!running_) return;
    running_ = false;
    auto ctx = static_cast<uv_ctx*>(loop_);
    uv_close(reinterpret_cast<uv_handle_t*>(&ctx->server), nullptr);
    uv_run(&ctx->loop, UV_RUN_NOWAIT);
    uv_loop_close(&ctx->loop);
    delete ctx;
    loop_ = nullptr;
}

void TcpServer::Send(std::shared_ptr<TcpConnection> conn, const uint8_t* data, size_t len) {
    if (!conn || !conn->handle) return;
    auto buf = new uint8_t[len];
    std::memcpy(buf, data, len);
    auto req = new uv_write_t;
    req->data = buf;
    uv_buf_t uvbuf = uv_buf_init(reinterpret_cast<char*>(buf), len);
    uv_write(req, reinterpret_cast<uv_stream_t*>(conn->handle), &uvbuf, 1, on_write);
}

uint32_t TcpServer::NextId() { return next_id_++; }
void TcpServer::OnNewConnection(std::shared_ptr<TcpConnection> c) { if (on_new_connection_) on_new_connection_(c); }
void TcpServer::OnData(std::shared_ptr<TcpConnection> c, uint8_t* d, size_t l, uint16_t t) { if (on_data_) on_data_(c, d, l, t); }
void TcpServer::OnClose(std::shared_ptr<TcpConnection> c) { if (on_close_) on_close_(c); }

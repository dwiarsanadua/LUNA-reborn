#include "TcpConnection.h"
#include "NetworkThread.h"
#include <spdlog/spdlog.h>
#include <cstring>
#include <cstdlib>

TcpConnection::TcpConnection(NetworkThread* thread)
    : thread_(thread) {}

TcpConnection::~TcpConnection() {
    Close();
}

bool TcpConnection::Accept(uv_stream_t* server) {
    if (!thread_ || !thread_->GetLoop()) return false;

    tcp_ = new uv_tcp_t;
    uv_tcp_init(thread_->GetLoop(), tcp_);
    tcp_->data = this;

    int r = uv_accept(server, reinterpret_cast<uv_stream_t*>(tcp_));
    if (r != 0) {
        spdlog::error("TcpConnection: accept failed - {}", uv_strerror(r));
        delete tcp_;
        tcp_ = nullptr;
        return false;
    }

    struct sockaddr_storage addr;
    int addr_len = sizeof(addr);
    if (uv_tcp_getpeername(tcp_, reinterpret_cast<struct sockaddr*>(&addr), &addr_len) == 0) {
        char ip[INET6_ADDRSTRLEN];
        if (addr.ss_family == AF_INET) {
            auto* ip4 = reinterpret_cast<struct sockaddr_in*>(&addr);
            uv_ip4_name(ip4, ip, sizeof(ip));
            remote_port_ = ntohs(ip4->sin_port);
        }
        remote_ip_ = ip;
    }

    uv_read_start(reinterpret_cast<uv_stream_t*>(tcp_), OnAlloc, OnRead);
    connected_ = true;

    spdlog::debug("TcpConnection: accepted from {}:{}", remote_ip_, remote_port_);
    return true;
}

bool TcpConnection::Connect(const std::string& ip, int port) {
    if (!thread_ || !thread_->GetLoop()) return false;

    tcp_ = new uv_tcp_t;
    uv_tcp_init(thread_->GetLoop(), tcp_);
    tcp_->data = this;

    struct sockaddr_in addr;
    uv_ip4_addr(ip.c_str(), port, &addr);

    auto* connect_req = new uv_connect_t;
    connect_req->data = this;

    int r = uv_tcp_connect(connect_req, tcp_,
        reinterpret_cast<const struct sockaddr*>(&addr), OnConnect);
    if (r != 0) {
        spdlog::error("TcpConnection: connect failed to {}:{} - {}", ip, port, uv_strerror(r));
        delete connect_req;
        delete tcp_;
        tcp_ = nullptr;
        return false;
    }

    remote_ip_ = ip;
    remote_port_ = port;
    spdlog::debug("TcpConnection: connecting to {}:{}", ip, port);
    return true;
}

void TcpConnection::Close() {
    if (tcp_ && connected_) {
        connected_ = false;
        uv_read_stop(reinterpret_cast<uv_stream_t*>(tcp_));
        uv_close(reinterpret_cast<uv_handle_t*>(tcp_), OnClose);
    }
}

bool TcpConnection::Send(const char* data, size_t length) {
    if (!tcp_ || !connected_) return false;

    auto* write_req = new WriteRequest;
    write_req->req.data = this;
    write_req->data.resize(length);
    memcpy(write_req->data.data(), data, length);

    uv_buf_t buf = uv_buf_init(write_req->data.data(), write_req->data.size());

    int r = uv_write(reinterpret_cast<uv_write_t*>(write_req),
        reinterpret_cast<uv_stream_t*>(tcp_), &buf, 1, OnWrite);
    if (r != 0) {
        spdlog::error("TcpConnection: write failed - {}", uv_strerror(r));
        delete write_req;
        return false;
    }
    return true;
}

void TcpConnection::OnAlloc(uv_handle_t* handle, size_t suggested, uv_buf_t* buf) {
    buf->base = static_cast<char*>(malloc(suggested));
    buf->len = suggested;
}

void TcpConnection::OnRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    auto* conn = static_cast<TcpConnection*>(stream->data);
    if (!conn) {
        free(buf->base);
        return;
    }

    if (nread < 0) {
        if (nread != UV_EOF) {
            spdlog::error("TcpConnection: read error - {}", uv_strerror((int)nread));
            if (conn->error_cb_) conn->error_cb_(uv_strerror((int)nread));
        }
        free(buf->base);
        conn->Close();
        if (conn->close_cb_) conn->close_cb_();
        return;
    }

    if (nread > 0 && conn->data_cb_) {
        conn->data_cb_(buf->base, nread);
    }

    free(buf->base);
}

void TcpConnection::OnWrite(uv_write_t* req, int status) {
    auto* write_req = reinterpret_cast<WriteRequest*>(req);
    if (status != 0) {
        auto* conn = static_cast<TcpConnection*>(req->data);
        if (conn && conn->error_cb_) {
            conn->error_cb_(uv_strerror(status));
        }
    }
    delete write_req;
}

void TcpConnection::OnConnect(uv_connect_t* req, int status) {
    auto* conn = static_cast<TcpConnection*>(req->data);
    delete req;

    if (status != 0) {
        spdlog::error("TcpConnection: connect failed - {}", uv_strerror(status));
        if (conn->error_cb_) conn->error_cb_(uv_strerror(status));
        conn->Close();
        return;
    }

    conn->connected_ = true;
    uv_read_start(reinterpret_cast<uv_stream_t*>(conn->tcp_), OnAlloc, OnRead);
    spdlog::debug("TcpConnection: connected to {}:{}", conn->remote_ip_, conn->remote_port_);
}

void TcpConnection::OnClose(uv_handle_t* handle) {
    delete handle;
}

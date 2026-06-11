#include "TcpListener.h"
#include "TcpConnection.h"
#include "NetworkThread.h"
#include <spdlog/spdlog.h>
#include <cstring>

TcpListener::TcpListener(NetworkThread* thread)
    : thread_(thread) {}

TcpListener::~TcpListener() {
    Close();
}

bool TcpListener::Bind(const std::string& ip, int port) {
    if (!thread_ || !thread_->GetLoop()) {
        spdlog::error("TcpListener: no event loop");
        return false;
    }

    tcp_ = new uv_tcp_t;
    uv_tcp_init(thread_->GetLoop(), tcp_);
    tcp_->data = this;

    struct sockaddr_in addr;
    uv_ip4_addr(ip.c_str(), port, &addr);

    int r = uv_tcp_bind(tcp_, reinterpret_cast<const struct sockaddr*>(&addr), 0);
    if (r != 0) {
        spdlog::error("TcpListener: bind failed on {}:{} - {}", ip, port, uv_strerror(r));
        delete tcp_;
        tcp_ = nullptr;
        return false;
    }

    r = uv_listen(reinterpret_cast<uv_stream_t*>(tcp_), SOMAXCONN, OnConnection);
    if (r != 0) {
        spdlog::error("TcpListener: listen failed on {}:{} - {}", ip, port, uv_strerror(r));
        delete tcp_;
        tcp_ = nullptr;
        return false;
    }

    port_ = port;
    listening_ = true;
    spdlog::info("TcpListener: listening on {}:{}", ip, port);
    return true;
}

void TcpListener::Close() {
    if (tcp_ && listening_) {
        uv_close(reinterpret_cast<uv_handle_t*>(tcp_), [](uv_handle_t* handle) {
            delete handle;
        });
        tcp_ = nullptr;
        listening_ = false;
        spdlog::info("TcpListener: closed (port {})", port_);
    }
}

void TcpListener::OnConnection(uv_stream_t* server, int status) {
    auto* listener = static_cast<TcpListener*>(server->data);
    if (!listener) return;

    if (status < 0) {
        spdlog::error("TcpListener: accept error - {}", uv_strerror(status));
        if (listener->error_cb_) {
            listener->error_cb_(uv_strerror(status));
        }
        return;
    }

    auto conn = std::make_unique<TcpConnection>(listener->thread_);
    if (!conn->Accept(server)) {
        spdlog::warn("TcpListener: failed to accept connection");
        return;
    }

    if (listener->accept_cb_) {
        listener->accept_cb_(std::move(conn));
    }
}

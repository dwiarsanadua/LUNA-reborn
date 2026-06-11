#pragma once
#include <uv.h>
#include <functional>
#include <memory>
#include <string>

class NetworkThread;
class TcpConnection;

class TcpListener {
public:
    using AcceptCallback = std::function<void(std::unique_ptr<TcpConnection>)>;
    using ErrorCallback = std::function<void(const std::string&)>;

    TcpListener(NetworkThread* thread);
    ~TcpListener();

    bool Bind(const std::string& ip, int port);
    void Close();
    bool IsListening() const { return listening_; }

    void SetAcceptCallback(AcceptCallback cb) { accept_cb_ = std::move(cb); }
    void SetErrorCallback(ErrorCallback cb) { error_cb_ = std::move(cb); }

    int GetPort() const { return port_; }

private:
    static void OnConnection(uv_stream_t* server, int status);

    NetworkThread* thread_;
    uv_tcp_t* tcp_ = nullptr;
    bool listening_ = false;
    int port_ = 0;
    AcceptCallback accept_cb_;
    ErrorCallback error_cb_;
};

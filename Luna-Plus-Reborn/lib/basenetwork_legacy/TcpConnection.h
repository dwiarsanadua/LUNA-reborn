#pragma once
#include <uv.h>
#include <functional>
#include <vector>
#include <string>
#include <cstdint>

class NetworkThread;

class TcpConnection {
public:
    using DataCallback = std::function<void(const char* data, ssize_t size)>;
    using CloseCallback = std::function<void()>;
    using ErrorCallback = std::function<void(const std::string&)>;

    TcpConnection(NetworkThread* thread);
    ~TcpConnection();

    bool Accept(uv_stream_t* server);
    bool Connect(const std::string& ip, int port);
    void Close();
    bool Send(const char* data, size_t length);
    bool IsConnected() const { return connected_; }

    void SetDataCallback(DataCallback cb) { data_cb_ = std::move(cb); }
    void SetCloseCallback(CloseCallback cb) { close_cb_ = std::move(cb); }
    void SetErrorCallback(ErrorCallback cb) { error_cb_ = std::move(cb); }

    uint32_t GetId() const { return id_; }
    void SetId(uint32_t id) { id_ = id; }

    const std::string& GetRemoteIP() const { return remote_ip_; }
    int GetRemotePort() const { return remote_port_; }

private:
    static void OnAlloc(uv_handle_t* handle, size_t suggested, uv_buf_t* buf);
    static void OnRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
    static void OnWrite(uv_write_t* req, int status);
    static void OnConnect(uv_connect_t* req, int status);
    static void OnClose(uv_handle_t* handle);

    struct WriteRequest {
        uv_write_t req;
        std::vector<char> data;
    };

    NetworkThread* thread_;
    uv_tcp_t* tcp_ = nullptr;
    bool connected_ = false;
    uint32_t id_ = 0;
    std::string remote_ip_;
    int remote_port_ = 0;

    DataCallback data_cb_;
    CloseCallback close_cb_;
    ErrorCallback error_cb_;
};

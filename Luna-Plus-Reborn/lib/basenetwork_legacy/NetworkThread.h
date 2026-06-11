#pragma once
#include <uv.h>
#include <functional>
#include <thread>
#include <memory>

class NetworkThread {
public:
    NetworkThread();
    ~NetworkThread();

    bool Init();
    void Shutdown();
    void Run();
    void Stop();
    bool IsRunning() const { return running_; }

    uv_loop_t* GetLoop() { return loop_; }

    using Task = std::function<void()>;
    void PostTask(Task task);

private:
    static void OnAsync(uv_async_t* handle);
    static void OnClose(uv_handle_t* handle);
    static void OnSignal(uv_signal_t* handle, int signum);

    uv_loop_t* loop_ = nullptr;
    uv_async_t* async_ = nullptr;
    uv_signal_t* sigint_ = nullptr;
    uv_signal_t* sigterm_ = nullptr;
    bool running_ = false;
    std::unique_ptr<std::thread> thread_;
};

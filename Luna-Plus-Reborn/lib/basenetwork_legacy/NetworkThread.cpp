#include "NetworkThread.h"
#include <spdlog/spdlog.h>

NetworkThread::NetworkThread() = default;

NetworkThread::~NetworkThread() {
    Shutdown();
}

bool NetworkThread::Init() {
    loop_ = new uv_loop_t;
    uv_loop_init(loop_);

    async_ = new uv_async_t;
    uv_async_init(loop_, async_, OnAsync);
    async_->data = this;

    sigint_ = new uv_signal_t;
    uv_signal_init(loop_, sigint_);
    uv_signal_start(sigint_, OnSignal, SIGINT);
    sigint_->data = this;

    sigterm_ = new uv_signal_t;
    uv_signal_init(loop_, sigterm_);
    uv_signal_start(sigterm_, OnSignal, SIGTERM);
    sigterm_->data = this;

    spdlog::info("NetworkThread: initialized");
    return true;
}

void NetworkThread::Shutdown() {
    if (!loop_) return;
    running_ = false;

    if (sigint_) {
        uv_signal_stop(sigint_);
        uv_close(reinterpret_cast<uv_handle_t*>(sigint_), OnClose);
        sigint_ = nullptr;
    }
    if (sigterm_) {
        uv_signal_stop(sigterm_);
        uv_close(reinterpret_cast<uv_handle_t*>(sigterm_), OnClose);
        sigterm_ = nullptr;
    }
    if (async_) {
        uv_close(reinterpret_cast<uv_handle_t*>(async_), OnClose);
        async_ = nullptr;
    }

    if (thread_ && thread_->joinable()) {
        thread_->join();
        thread_.reset();
    }

    uv_loop_close(loop_);
    delete loop_;
    loop_ = nullptr;
    spdlog::info("NetworkThread: shutdown");
}

void NetworkThread::Run() {
    running_ = true;
    thread_ = std::make_unique<std::thread>([this]() {
        spdlog::info("NetworkThread: event loop started");
        while (running_) {
            uv_run(loop_, UV_RUN_NOWAIT);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        spdlog::info("NetworkThread: event loop ended");
    });
}

void NetworkThread::Stop() {
    running_ = false;
    if (async_) {
        uv_async_send(async_);
    }
}

void NetworkThread::PostTask(Task task) {
    if (!async_) return;
    auto* holder = new Task(std::move(task));
    async_->data = holder;
    uv_async_send(async_);
}

void NetworkThread::OnAsync(uv_async_t* handle) {
    auto* task = static_cast<Task*>(handle->data);
    if (task) {
        (*task)();
        delete task;
        handle->data = nullptr;
    }
}

void NetworkThread::OnClose(uv_handle_t* handle) {
    delete handle;
}

void NetworkThread::OnSignal(uv_signal_t* handle, int signum) {
    auto* self = static_cast<NetworkThread*>(handle->data);
    if (self) {
        spdlog::info("NetworkThread: caught signal {}", signum);
        self->Stop();
    }
}

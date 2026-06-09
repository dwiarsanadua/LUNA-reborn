#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <functional>
#include <string>

class ShoutDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm);
    void Close();
    bool IsOpen() const { return window_ != nullptr; }

    using SendCallback = std::function<void(const std::string& message)>;
    void SetSendCallback(SendCallback cb) { on_send_ = std::move(cb); }

private:
    Window* window_ = nullptr;
    class InputField* input_ = nullptr;
    SendCallback on_send_;
};

#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <functional>
#include <string>

class SystemMsgDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm, const std::string& title, const std::string& message);
    void Close();
    bool IsOpen() const { return window_ != nullptr; }

private:
    Window* window_ = nullptr;
};

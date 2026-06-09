#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>

class HousingWebDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm, const std::string& url);
    void Close();
    bool IsOpen() const { return window_ != nullptr; }

private:
    Window* window_ = nullptr;
};

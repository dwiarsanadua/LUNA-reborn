#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>

class GuildLevelUp {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm);
    void Open(WindowManager* wm, int new_level);
    void Close();
    bool IsOpen() const { return window_ != nullptr; }
    int GetNewLevel() const { return new_level_; }
private:
    Window* window_ = nullptr;
    int new_level_ = 1;
};

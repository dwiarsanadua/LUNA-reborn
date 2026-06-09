#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <functional>

enum class GameOutMode : uint8_t {
    GameExit,
    CharSelect,
    Cancel,
};

class GameOutDialog {
public:
    GameOutDialog();

    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm,
              std::function<void(GameOutMode mode)> on_exit,
              bool can_char_select = true);
    void Close();
    bool IsOpen() const { return window_ != nullptr; }

private:
    Window* window_ = nullptr;
    std::function<void(GameOutMode)> on_exit_;
};

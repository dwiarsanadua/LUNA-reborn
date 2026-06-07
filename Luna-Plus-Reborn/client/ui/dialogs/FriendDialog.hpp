#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/InputField.hpp>
#include <ui/widgets/TabPanel.hpp>

class FriendDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm = nullptr);
    void Close() { window_ = nullptr; }
private:
    Window* window_ = nullptr;
};

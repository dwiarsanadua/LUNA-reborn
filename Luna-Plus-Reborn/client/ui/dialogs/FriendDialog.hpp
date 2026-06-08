#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/TabPanel.hpp>

class FriendDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm = nullptr);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
private:
    Window* window_ = nullptr;
    ListBox* online_list_ = nullptr;
    ListBox* offline_list_ = nullptr;
};

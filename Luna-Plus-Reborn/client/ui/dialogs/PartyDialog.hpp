#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/ListBox.hpp>
#include <vector>

class PartyDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm = nullptr);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
private:
    Window* window_ = nullptr;
    ListBox* member_list_ = nullptr;
};

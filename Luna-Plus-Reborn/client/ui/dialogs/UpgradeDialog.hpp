#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <cstdint>

class UpgradeDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);

private:
    Window* window_ = nullptr;
    Label* info_label_ = nullptr;
    Label* result_label_ = nullptr;
    int selected_item_ = -1;
    void DoUpgrade(GameState* state);
    void DoSynthesize(GameState* state);
};

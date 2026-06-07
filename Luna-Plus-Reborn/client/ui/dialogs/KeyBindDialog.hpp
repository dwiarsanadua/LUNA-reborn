#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <config/KeyBindings.hpp>
#include <string>
#include <vector>

class KeyBindDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    Label* status_label_ = nullptr;
    std::vector<Grid*> category_grids_;
    void RefreshCategory(int cat_idx);
};

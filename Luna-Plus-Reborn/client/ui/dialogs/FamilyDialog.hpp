#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <gameobjects/FamilySystem.hpp>

class FamilyDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm, FamilySystem* family);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    Label* status_label_ = nullptr;
    Label* tab1_label_ = nullptr;
    Label* tab2_label_ = nullptr;
    Label* tab3_label_ = nullptr;
    FamilySystem* family_ = nullptr;
    void Refresh(GameState* state);
};

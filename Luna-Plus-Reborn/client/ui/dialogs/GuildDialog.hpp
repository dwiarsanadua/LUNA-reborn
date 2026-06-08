#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <gameobjects/SiegeSystem.hpp>

class GuildDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm = nullptr, SiegeSystem* siege = nullptr);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
private:
    Window* window_ = nullptr;
    ListBox* member_list_ = nullptr;
    ListBox* info_list_ = nullptr;
    void PopulateSiegeTab(ListBox* lb, SiegeSystem* siege);
};

#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <functional>

class DungeonSystem;

class DungeonDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm, DungeonSystem* dungeon);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
    void SetNetworkCallbacks(std::function<void(uint32_t)> enter_fn,
                             std::function<void(uint32_t)> info_fn);

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    Label* info_label_ = nullptr;
    Label* tab0_label_ = nullptr;
    Label* tab1_label_ = nullptr;
    Label* tab2_label_ = nullptr;
    DungeonSystem* dungeon_ = nullptr;
    std::function<void(uint32_t)> on_enter_dungeon_;
    std::function<void(uint32_t)> on_request_info_;
    void Refresh(GameState* state);
};

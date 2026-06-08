#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/InputField.hpp>
#include <gameobjects/SiegeSystem.hpp>
#include <functional>

class GuildDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(WindowManager* wm = nullptr, SiegeSystem* siege = nullptr);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
    void SetSiegeNetworkCallbacks(std::function<void(uint8_t, uint32_t, uint32_t)> action_fn,
                                  std::function<void()> refresh_fn);

private:
    Window* window_ = nullptr;
    ListBox* member_list_ = nullptr;
    ListBox* info_list_ = nullptr;
    ListBox* siege_list_ = nullptr;
    InputField* territory_input_ = nullptr;
    InputField* tax_input_ = nullptr;
    SiegeSystem* siege_ = nullptr;
    std::function<void(uint8_t, uint32_t, uint32_t)> siege_action_fn_;
    std::function<void()> siege_refresh_fn_;
    void PopulateSiegeTab(GameState* state);
    void PopulateSiegeTabOffline(SiegeSystem* siege);
    uint32_t SelectedTerritoryId() const;
};

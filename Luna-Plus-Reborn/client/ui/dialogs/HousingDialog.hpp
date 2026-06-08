#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <functional>
#include <string>
#include <vector>
#include <cstdint>

class HousingDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
    void SetNetworkCallbacks(
        std::function<void(uint8_t template_id)> buy_fn,
        std::function<void(uint32_t house_id)> enter_fn,
        std::function<void(uint32_t house_id, uint32_t item_id, float x, float y)> place_fn,
        std::function<void()> refresh_fn);

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    Label* info_label_ = nullptr;
    Label* overview_label_ = nullptr;
    Grid* furniture_grid_ = nullptr;
    std::function<void(uint8_t)> buy_fn_;
    std::function<void(uint32_t)> enter_fn_;
    std::function<void(uint32_t, uint32_t, float, float)> place_fn_;
    std::function<void()> refresh_fn_;
    int selected_template_ = 0;
    uint32_t selected_house_id_ = 0;
    uint32_t place_item_id_ = 9001;
    bool online_mode_ = false;
    void Refresh(GameState* state);
    const GameState::NetworkHouseInfo* SelectedHouse(const GameState* state) const;
    void ClampTemplateSelection(GameState* state);
};

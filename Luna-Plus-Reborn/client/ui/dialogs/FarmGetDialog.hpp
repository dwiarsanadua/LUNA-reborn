#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <functional>
#include <cstdint>

class FarmGetDialog {
public:
    FarmGetDialog();

    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm,
              std::function<void(uint8_t plot_id, uint32_t seed_id)> on_plant,
              std::function<void(uint8_t plot_id)> on_water,
              std::function<void(uint8_t plot_id)> on_harvest);
    void Close();
    void Refresh(GameState* state);
    void SelectSeed(uint32_t seed_id);

private:
    Window* window_ = nullptr;
    Label* status_label_ = nullptr;
    Label* seeds_label_ = nullptr;

    uint32_t selected_seed_ = 1;

    std::function<void(uint8_t, uint32_t)> plant_cb_;
    std::function<void(uint8_t)> water_cb_;
    std::function<void(uint8_t)> harvest_cb_;
};

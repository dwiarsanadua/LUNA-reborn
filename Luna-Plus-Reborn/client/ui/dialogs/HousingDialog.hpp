#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <string>
#include <vector>
#include <cstdint>

struct HousingFurniture {
    uint32_t id;
    std::string name;
    int slot;
};

class HousingDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    Label* info_label_ = nullptr;
    Grid* furniture_grid_ = nullptr;
    std::vector<HousingFurniture> furniture_;
    void Refresh(GameState* state);
};

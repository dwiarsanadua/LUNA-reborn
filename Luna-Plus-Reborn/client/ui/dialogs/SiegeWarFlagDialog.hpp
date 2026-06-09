#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <functional>
#include <cstdint>

class SiegeWarFlagDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);
    void SetFlagInfo(const std::string& flag_name, uint32_t territory_id);
    void SetPlantCallback(std::function<void(uint32_t)> on_plant);
    void SetRemoveCallback(std::function<void(uint32_t)> on_remove);

private:
    void DoPlant();
    void DoRemove();

    Window* window_ = nullptr;
    Label* title_label_ = nullptr;
    Label* flag_name_label_ = nullptr;
    Label* territory_label_ = nullptr;
    Button* plant_btn_ = nullptr;
    Button* remove_btn_ = nullptr;
    std::string flag_name_;
    uint32_t territory_id_ = 0;
    GameState* state_ = nullptr;
    std::function<void(uint32_t)> on_plant_;
    std::function<void(uint32_t)> on_remove_;
};

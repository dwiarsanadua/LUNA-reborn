#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <functional>
#include <cstdint>

class IdentificationDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);
    void SetIdentifiableItems(const std::vector<int>& inv_slots);
    void SetIdentifyCallback(std::function<void(int)> on_identify);

private:
    void RebuildGrid();
    void DoIdentify(int slot);

    Window* window_ = nullptr;
    Grid* grid_ = nullptr;
    Label* info_label_ = nullptr;
    Button* identify_btn_ = nullptr;
    Button* cancel_btn_ = nullptr;
    std::vector<int> identifiable_slots_;
    int selected_slot_ = -1;
    GameState* state_ = nullptr;
    std::function<void(int)> on_identify_;
};

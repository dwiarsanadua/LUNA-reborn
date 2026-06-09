#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <functional>
#include <cstdint>

struct EnchantScrollInfo {
    uint32_t scroll_item_id = 0;
    std::string scroll_name;
    uint32_t target_item_id = 0;
    std::string target_name;
    uint8_t success_rate = 50;
};

class EnchantDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);
    void SetEnchantInfo(const EnchantScrollInfo& info);
    void SetEnchantCallback(std::function<void(uint32_t, uint32_t)> on_enchant);

private:
    void DoEnchant();

    Window* window_ = nullptr;
    Label* info_label_ = nullptr;
    Label* result_label_ = nullptr;
    Label* cost_label_ = nullptr;
    Button* enchant_btn_ = nullptr;
    EnchantScrollInfo current_info_;
    GameState* state_ = nullptr;
    std::function<void(uint32_t, uint32_t)> on_enchant_;
};

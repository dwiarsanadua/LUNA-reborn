#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/ProgressBar.hpp>
#include <gameobjects/CashShopSystem.hpp>
#include <functional>

class CashShopDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm, CashShopSystem* shop);
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);
    void SetNetworkCallbacks(std::function<void(uint32_t)> buy_fn,
                             std::function<void()> refresh_fn);

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    Label* info_label_ = nullptr;
    Label* luna_label_ = nullptr;
    Label* shop_content_ = nullptr;
    Label* bp_content_ = nullptr;
    ProgressBar* bp_bar_ = nullptr;
    CashShopSystem* shop_ = nullptr;
    std::function<void(uint32_t)> buy_fn_;
    std::function<void()> refresh_fn_;
    void RefreshShop(int category_idx);
    void RefreshBattlePass(GameState* state);
};



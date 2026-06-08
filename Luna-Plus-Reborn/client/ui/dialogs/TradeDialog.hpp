#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <gameobjects/TradingSystem.hpp>
#include <cstdint>
#include <string>
#include <functional>

struct TradeNetworkCallbacks {
    bool enabled = false;
    std::function<void(uint8_t inv_slot)> on_add_item;
    std::function<void(uint32_t gold_add)> on_add_gold;
    std::function<void()> on_confirm;
    std::function<void()> on_cancel;
};

class TradeDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, TradingSystem* trading = nullptr, WindowManager* wm = nullptr);
    void SetNetworkCallbacks(const TradeNetworkCallbacks& cb) { net_cb_ = cb; }
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);

private:
    Window* window_ = nullptr;
    Label* my_gold_label_ = nullptr;
    Label* their_gold_label_ = nullptr;
    Label* status_label_ = nullptr;
    Grid* my_items_ = nullptr;
    Grid* their_items_ = nullptr;
    TradingSystem* trading_ = nullptr;
    TradeNetworkCallbacks net_cb_;
    uint32_t session_id_ = 0;
    void Refresh(GameState* state);
};

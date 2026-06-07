#pragma once
#include <ui/Window.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <gameobjects/TradingSystem.hpp>
#include <cstdint>
#include <string>

class TradeDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, TradingSystem* trading = nullptr);
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
    uint32_t session_id_ = 0;
    void Refresh(GameState* state);
};

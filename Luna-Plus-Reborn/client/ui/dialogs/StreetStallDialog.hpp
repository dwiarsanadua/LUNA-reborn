#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/InputField.hpp>
#include <ui/widgets/ListBox.hpp>
#include <cstdint>
#include <functional>

struct StallNetworkCallbacks {
    bool enabled = false;
    std::function<void(const std::string& title)> on_open;
    std::function<void(uint8_t inv_slot, uint32_t price)> on_add;
    std::function<void(uint32_t owner_id, uint8_t stall_slot)> on_buy;
    std::function<void()> on_close;
    std::function<void()> on_list;
};

class StreetStallDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void SetNetworkCallbacks(const StallNetworkCallbacks& cb) { net_cb_ = cb; }
    void Close() { window_ = nullptr; }
    void UpdateFromState(GameState* state);

private:
    Window* window_ = nullptr;
    ListBox* my_stall_list_ = nullptr;
    ListBox* nearby_list_ = nullptr;
    InputField* title_input_ = nullptr;
    InputField* price_input_ = nullptr;
    Label* status_label_ = nullptr;
    Label* gold_label_ = nullptr;
    StallNetworkCallbacks net_cb_;
    uint32_t selected_nearby_owner_ = 0;
    uint8_t selected_nearby_slot_ = 0;

    void RefreshMyStall(GameState* state);
    void RefreshNearby(GameState* state);
    void DoOpenStall(GameState* state);
    void DoAddItem(GameState* state);
    void DoBuyItem(GameState* state);
    void DoCloseStall(GameState* state);
    void DoRefresh(GameState* state);
};

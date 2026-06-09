#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <functional>
#include <vector>
#include <cstdint>

struct MallItem {
    uint32_t item_id = 0;
    std::string name;
    uint32_t seller_id = 0;
    std::string seller_name;
    uint16_t count = 1;
};

class ItemMallWarehouseDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);
    void SetItems(const std::vector<MallItem>& items);
    void SetWithdrawCallback(std::function<void(uint32_t, uint16_t)> on_withdraw);

private:
    void RebuildGrid();
    void DoWithdraw(int slot);

    Window* window_ = nullptr;
    Grid* grid_ = nullptr;
    Label* info_label_ = nullptr;
    std::vector<MallItem> items_;
    GameState* state_ = nullptr;
    std::function<void(uint32_t, uint16_t)> on_withdraw_;
};

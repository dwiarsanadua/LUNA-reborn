#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/InputField.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include <functional>

struct NpcShopEntry {
    uint32_t item_id = 0;
    std::string name;
    uint32_t price = 0;
    uint16_t count = 0;
    uint8_t category = 0;
};

class NPCShopDialog {
public:
    Window* GetWindow() { return window_; }
    void Open(GameState* state, uint32_t npc_id, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);
    void SetShopItems(const std::vector<NpcShopEntry>& items);
    void SetBuyCallback(std::function<void(uint32_t, uint16_t)> on_buy);
    void SetSellCallback(std::function<void(int, uint16_t)> on_sell);

private:
    void RebuildShopGrid();
    void RebuildSellGrid();
    void FilterByCategory(int cat);
    void DoBuy(int slot);
    void DoSell(int slot);

    Window* window_ = nullptr;
    Grid* shop_grid_ = nullptr;
    Grid* sell_grid_ = nullptr;
    Label* gold_label_ = nullptr;
    Label* title_label_ = nullptr;
    ListBox* category_list_ = nullptr;
    InputField* search_input_ = nullptr;
    Button* buy_btn_ = nullptr;
    Button* sell_btn_ = nullptr;

    uint32_t npc_id_ = 0;
    GameState* state_ = nullptr;
    std::vector<NpcShopEntry> all_items_;
    std::vector<NpcShopEntry> filtered_items_;
    int current_category_ = -1;

    std::function<void(uint32_t, uint16_t)> on_buy_;
    std::function<void(int, uint16_t)> on_sell_;
};

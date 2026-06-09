#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <vector>
#include <string>
#include <cstdint>

struct NPCDialogNode {
    std::string text;
    struct Option {
        std::string text;
        int next_node = -1;
    };
    std::vector<Option> options;
};

struct NpcShopItem {
    uint32_t item_id = 0;
    std::string name;
    uint32_t price = 0;
    uint16_t count = 1;
    uint16_t max_purchase = 99;
};

enum class NPCScreen : uint8_t {
    Dialog,
    Shop,
    Sell,
};

class NPCDialog {
public:
    NPCDialog();
    Window* GetWindow() { return window_; }
    void Open(GameState* state, uint32_t npc_id, const std::string& npc_name, WindowManager* wm = nullptr);
    void Close();
    void SetNode(int node_id);
    void UpdateFromState(GameState* state);
    void ShowShop();
    void ShowSell();
    void SetShopItems(const std::vector<NpcShopItem>& items);
    void SetNpcDialogTree(const std::vector<NPCDialogNode>& tree);

private:
    void BuildDialogTree();
    void RebuildShopGrid();
    void RebuildSellGrid();
    void SetScreen(NPCScreen screen);
    void DoBuyItem(uint32_t item_id, uint32_t price);
    void DoSellItem(int inv_slot);

    Window* window_ = nullptr;
    Label* text_label_ = nullptr;
    std::vector<Button*> option_btns_;
    TextureInfo bg_tex_;

    Grid* shop_grid_ = nullptr;
    Grid* sell_grid_ = nullptr;
    Label* shop_gold_label_ = nullptr;
    Label* shop_title_label_ = nullptr;
    Button* back_btn_ = nullptr;
    TabPanel* shop_tabs_ = nullptr;

    uint32_t current_npc_id_ = 0;
    std::string current_npc_name_;
    std::vector<NPCDialogNode> current_tree_;
    std::vector<NpcShopItem> shop_items_;
    GameState* state_ = nullptr;
    NPCScreen current_screen_ = NPCScreen::Dialog;
};

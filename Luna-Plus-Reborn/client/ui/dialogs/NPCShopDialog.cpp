#include "NPCShopDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void NPCShopDialog::Open(GameState* state, uint32_t npc_id, WindowManager* wm) {
    if (window_) return;
    state_ = state;
    npc_id_ = npc_id;

    window_ = wm->LoadFromScript("assets/interface/Windows/NPCShop.bin.txt");
    if (!window_) {
        spdlog::warn("NPCShopDialog: fallback to default window");
        window_ = new Window("NPC Shop", 115, 80, 260, 360);
        window_->SetMovable(true);
        window_->SetClosable(true);
    }

    char buf[64];
    title_label_ = window_->AddWidget<Label>("NPC Shop", 15, 10, ColorPalette::TEXT_GOLD);

    category_list_ = window_->AddWidget<ListBox>(10, 50, 80, 120);
    category_list_->AddItem("All");
    category_list_->AddItem("Weapons");
    category_list_->AddItem("Armor");
    category_list_->AddItem("Consumables");
    category_list_->AddItem("Materials");
    category_list_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) FilterByCategory(category_list_->GetSelected());
    });

    search_input_ = window_->AddWidget<InputField>(10, 180, 240, 22);
    search_input_->SetPlaceholder("Search...");

    shop_grid_ = new Grid(6, 4, 50, 50, 4, 4);
    window_->AddWidget<Grid>(shop_grid_, 10, 210, 240, 200);

    snprintf(buf, sizeof(buf), "Gold: %d", state ? state->gold : 0);
    gold_label_ = window_->AddWidget<Label>(buf, 10, 330, ColorPalette::TEXT_GOLD);

    buy_btn_ = window_->AddWidget<Button>("Buy", 60, 330, 60, 22);
    buy_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) spdlog::info("NPCShop: buy mode");
    });

    sell_btn_ = window_->AddWidget<Button>("Sell", 140, 330, 60, 22);
    sell_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) RebuildSellGrid();
    });

    window_->AddWidget<Button>("Close", 220, 330, 50, 22)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void NPCShopDialog::SetShopItems(const std::vector<NpcShopEntry>& items) {
    all_items_ = items;
    filtered_items_ = items;
    current_category_ = -1;
    RebuildShopGrid();
}

void NPCShopDialog::SetBuyCallback(std::function<void(uint32_t, uint16_t)> on_buy) {
    on_buy_ = on_buy;
}

void NPCShopDialog::SetSellCallback(std::function<void(int, uint16_t)> on_sell) {
    on_sell_ = on_sell;
}

void NPCShopDialog::FilterByCategory(int cat) {
    if (cat <= 0) {
        filtered_items_ = all_items_;
    } else {
        filtered_items_.clear();
        for (const auto& item : all_items_) {
            if (item.category == cat - 1) filtered_items_.push_back(item);
        }
    }
    current_category_ = cat;
    RebuildShopGrid();
}

void NPCShopDialog::RebuildShopGrid() {
    if (!shop_grid_) return;
    shop_grid_->ClearAll();

    int cols = 6;
    for (size_t i = 0; i < filtered_items_.size() && i < 24; i++) {
        const auto& item = filtered_items_[i];
        char label[64];
        snprintf(label, sizeof(label), "%s\n%ug", item.name.c_str(), item.price);
        GridSlot gs;
        gs.empty = false;
        gs.count = item.count;
        gs.text = label;
        gs.userdata = (int)item.item_id;
        int row = (int)i / cols;
        int col = (int)i % cols;
        shop_grid_->SetSlot(row, col, gs);
    }

    shop_grid_->OnSlotEvent([this](int row, int col, const UIEvent& e) {
        if (e.type == UIEvent::Click) DoBuy(row * 6 + col);
    });

    if (state_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Gold: %d", state_->gold);
        gold_label_->SetText(buf);
    }
}

void NPCShopDialog::RebuildSellGrid() {
    if (!shop_grid_ || !state_) return;
    shop_grid_->ClearAll();

    int cols = 6;
    for (size_t i = 0; i < state_->inventory.size(); i++) {
        const auto& inv = state_->inventory[i];
        if (inv.id == 0) continue;
        char label[64];
        snprintf(label, sizeof(label), "%s x%d", inv.name.c_str(), inv.count);
        GridSlot gs;
        gs.empty = false;
        gs.count = inv.count;
        gs.text = label;
        gs.userdata = (int)i;
        int row = (int)i / cols;
        int col = (int)i % cols;
        shop_grid_->SetSlot(row, col, gs);
    }

    shop_grid_->OnSlotEvent([this](int row, int col, const UIEvent& e) {
        if (e.type == UIEvent::Click) DoSell(row * 6 + col);
    });
}

void NPCShopDialog::DoBuy(int slot) {
    if (slot < 0 || slot >= (int)filtered_items_.size()) return;
    const auto& item = filtered_items_[slot];
    if (state_ && state_->gold < (int)item.price) {
        spdlog::warn("NPCShop: insufficient gold for item {}", item.item_id);
        return;
    }
    if (on_buy_) on_buy_(item.item_id, 1);
    if (state_) {
        state_->gold -= (int)item.price;
        char buf[64];
        snprintf(buf, sizeof(buf), "Gold: %d", state_->gold);
        gold_label_->SetText(buf);
    }
}

void NPCShopDialog::DoSell(int slot) {
    if (!state_ || slot < 0 || slot >= (int)state_->inventory.size()) return;
    if (on_sell_) on_sell_(slot, 1);
}

void NPCShopDialog::UpdateFromState(GameState* state) {
    state_ = state;
    if (window_ && state) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Gold: %d", state->gold);
        gold_label_->SetText(buf);
    }
}

void NPCShopDialog::Close() {
    window_ = nullptr;
    shop_grid_ = nullptr;
    sell_grid_ = nullptr;
    gold_label_ = nullptr;
    title_label_ = nullptr;
    category_list_ = nullptr;
    search_input_ = nullptr;
    buy_btn_ = nullptr;
    sell_btn_ = nullptr;
    all_items_.clear();
    filtered_items_.clear();
}

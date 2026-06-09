#include "NPCDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

NPCDialog::NPCDialog() {}

void NPCDialog::Open(GameState* state, uint32_t npc_id, const std::string& npc_name, WindowManager* wm) {
    if (window_) return;
    state_ = state;
    current_npc_id_ = npc_id;
    current_npc_name_ = npc_name;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/NpcImage.bin.txt");
    if (!window_) {
        spdlog::warn("NPCDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("", 300, 300, 720, 420);
        window_->SetMovable(true);
        window_->SetClosable(true);
        window_->SetTitleBarH(0);
    }

    window_->SetCustomBackground([this](UIRenderer& ui, float x, float y, float w, float h) {
        if (!bgfx::isValid(bg_tex_.handle)) {
            bg_tex_ = ui.LoadTexture("ui_npc_dialog", "ui_npctopbottom.png");
        }
        if (bgfx::isValid(bg_tex_.handle)) {
            ui.DrawImage(x, y, w, h, bg_tex_.handle);
        } else {
            ui.DrawRect(x, y, w, h, {20, 20, 30, 230});
            ui.DrawBorder(x, y, w, h, {100, 100, 150, 200});
        }
    });

    text_label_ = window_->AddWidget<Label>("", 120, 36, ColorPalette::TEXT_NORMAL);

    shop_title_label_ = window_->AddWidget<Label>("", 120, 36, ColorPalette::TEXT_GOLD);
    shop_title_label_->SetVisible(false);

    auto* tabs = window_->AddWidget<TabPanel>(10, 62, 700, 300);
    shop_tabs_ = tabs;

    shop_grid_ = new Grid(6, 6, 60, 60, 8, 8);
    tabs->AddTab("Buy", shop_grid_);

    sell_grid_ = new Grid(6, 6, 60, 60, 8, 8);
    tabs->AddTab("Sell", sell_grid_);
    tabs->SetVisible(false);

    char buf[64];
    snprintf(buf, sizeof(buf), "Gold: %d", state ? state->gold : 0);
    shop_gold_label_ = window_->AddWidget<Label>(buf, 120, 370, ColorPalette::TEXT_GOLD);
    shop_gold_label_->SetVisible(false);

    back_btn_ = window_->AddWidget<Button>("Back", 120, 370, 80, 24);
    back_btn_->SetVisible(false);
    back_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) SetScreen(NPCScreen::Dialog);
    });

    auto* close_btn = window_->AddWidget<Button>("Close", 620, 370, 80, 24);
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });

    BuildDialogTree();
    SetScreen(NPCScreen::Dialog);
    SetNode(0);
}

void NPCDialog::SetScreen(NPCScreen screen) {
    current_screen_ = screen;
    bool is_shop = (screen == NPCScreen::Shop || screen == NPCScreen::Sell);

    text_label_->SetVisible(screen == NPCScreen::Dialog);
    for (auto* btn : option_btns_) btn->SetVisible(screen == NPCScreen::Dialog);
    shop_gold_label_->SetVisible(is_shop);
    back_btn_->SetVisible(is_shop);
    shop_title_label_->SetVisible(is_shop);
    shop_tabs_->SetVisible(is_shop);

    if (is_shop && state_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Gold: %d", state_->gold);
        shop_gold_label_->SetText(buf);
        shop_title_label_->SetText("Shop - " + current_npc_name_);
    }

    if (screen == NPCScreen::Shop) {
        shop_tabs_->SetActive(0);
        RebuildShopGrid();
    } else if (screen == NPCScreen::Sell) {
        shop_tabs_->SetActive(1);
        RebuildSellGrid();
    }
}

void NPCDialog::ShowShop() {
    SetScreen(NPCScreen::Shop);
}

void NPCDialog::ShowSell() {
    SetScreen(NPCScreen::Sell);
}

void NPCDialog::SetNode(int node_id) {
    if (node_id < 0 || node_id >= (int)current_tree_.size()) {
        Close();
        return;
    }

    const auto& node = current_tree_[node_id];
    text_label_->SetText(node.text);

    for (auto* btn : option_btns_) {
        btn->SetVisible(false);
    }
    option_btns_.clear();

    float bx = 120, by = 100;
    for (size_t i = 0; i < node.options.size(); i++) {
        const auto& opt = node.options[i];
        char label[128];
        if (opt.next_node == -2) {
            snprintf(label, sizeof(label), "%s ->", opt.text.c_str());
        } else {
            snprintf(label, sizeof(label), "%s", opt.text.c_str());
        }
        auto* btn = window_->AddWidget<Button>(label, bx, by + i * 28, 400, 24);
        btn->OnEvent([this, opt](const UIEvent& e) {
            if (e.type == UIEvent::Click) {
                if (opt.next_node == -1) Close();
                else if (opt.next_node == -2) ShowShop();
                else if (opt.next_node == -3) { spdlog::info("NPC: quest dialog"); }
                else SetNode(opt.next_node);
            }
        });
        option_btns_.push_back(btn);
    }
}

void NPCDialog::BuildDialogTree() {
    current_tree_.clear();
    NPCDialogNode root;
    root.text = "Hello! I am " + current_npc_name_ + ". How can I help you today?";
    root.options.push_back({"Trade", -2});
    root.options.push_back({"Quest", -3});
    root.options.push_back({"Goodbye", -1});
    current_tree_.push_back(root);
}

void NPCDialog::SetNpcDialogTree(const std::vector<NPCDialogNode>& tree) {
    current_tree_ = tree;
    SetScreen(NPCScreen::Dialog);
    SetNode(0);
}

void NPCDialog::SetShopItems(const std::vector<NpcShopItem>& items) {
    shop_items_ = items;
    if (current_screen_ == NPCScreen::Shop) RebuildShopGrid();
}

void NPCDialog::RebuildShopGrid() {
    if (!shop_grid_) return;
    shop_grid_->ClearAll();

    int cols = 6;
    for (size_t i = 0; i < shop_items_.size(); i++) {
        const auto& item = shop_items_[i];
        char label[128];
        if (item.price > 0) {
            snprintf(label, sizeof(label), "%s\n%ug", item.name.c_str(), item.price);
        } else {
            snprintf(label, sizeof(label), "%s", item.name.c_str());
        }
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
        if (e.type == UIEvent::Click) {
            int idx = row * 6 + col;
            if (idx >= 0 && idx < (int)shop_items_.size()) {
                const auto& item = shop_items_[idx];
                DoBuyItem(item.item_id, item.price);
            }
        }
    });

    if (state_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Gold: %d", state_->gold);
        shop_gold_label_->SetText(buf);
    }
}

void NPCDialog::RebuildSellGrid() {
    if (!sell_grid_ || !state_) return;
    sell_grid_->ClearAll();

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
        sell_grid_->SetSlot(row, col, gs);
    }

    sell_grid_->OnSlotEvent([this](int row, int col, const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            int idx = row * 6 + col;
            if (idx >= 0 && idx < (int)state_->inventory.size()) {
                DoSellItem(idx);
            }
        }
    });
}

void NPCDialog::DoBuyItem(uint32_t item_id, uint32_t price) {
    if (!state_) return;
    if (state_->gold < (int)price) {
        spdlog::warn("NPCShop: not enough gold for item {} (price {})", item_id, price);
        return;
    }
    state_->gold -= (int)price;
    InvItem new_item;
    new_item.id = item_id;
    new_item.name = "Item_" + std::to_string(item_id);
    new_item.count = 1;
    new_item.slot = (int)state_->inventory.size();
    state_->inventory.push_back(new_item);

    char buf[64];
    snprintf(buf, sizeof(buf), "Gold: %d", state_->gold);
    shop_gold_label_->SetText(buf);

    spdlog::info("NPCShop: bought item {} for {}g", item_id, price);
}

void NPCDialog::DoSellItem(int inv_slot) {
    if (!state_ || inv_slot < 0 || inv_slot >= (int)state_->inventory.size()) return;
    auto& item = state_->inventory[inv_slot];
    if (item.id == 0) return;

    uint32_t sell_price = 1;
    state_->gold += (int)sell_price;
    item.id = 0;
    item.name.clear();
    item.count = 0;

    char buf[64];
    snprintf(buf, sizeof(buf), "Gold: %d", state_->gold);
    shop_gold_label_->SetText(buf);
    RebuildSellGrid();

    spdlog::info("NPCShop: sold item from slot {} for {}g", inv_slot, sell_price);
}

void NPCDialog::Close() {
    window_ = nullptr;
    text_label_ = nullptr;
    shop_grid_ = nullptr;
    sell_grid_ = nullptr;
    shop_gold_label_ = nullptr;
    shop_title_label_ = nullptr;
    back_btn_ = nullptr;
    shop_tabs_ = nullptr;
    option_btns_.clear();
    shop_items_.clear();
    current_screen_ = NPCScreen::Dialog;
}

void NPCDialog::UpdateFromState(GameState* state) {
    state_ = state;
    if (window_ && (current_screen_ == NPCScreen::Shop || current_screen_ == NPCScreen::Sell)) {
        if (state) {
            char buf[64];
            snprintf(buf, sizeof(buf), "Gold: %d", state->gold);
            shop_gold_label_->SetText(buf);
        }
    }
}

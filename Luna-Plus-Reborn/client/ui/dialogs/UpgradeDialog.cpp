#include "UpgradeDialog.hpp"
#include <cstdio>
#include <cstdlib>

void UpgradeDialog::Open(GameState* state, WindowManager* wm) {
    window_ = wm->Open("Item Upgrade & Synthesis", 180, 60, 440, 400);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("=== Forge ===", 10, 4, 0xffffcc88);
    info_label_ = window_->AddWidget<Label>("Select an item from inventory\nto upgrade or synthesize.", 10, 28, 0xffffffff);

    // Inventory grid for item selection
    auto* inv_grid = window_->AddWidget<Grid>(4, 2, 180, 28, 10, 100);
    inv_grid->SetPadding(3);
    for (int i = 0; i < (int)state->inventory.size() && i < 8; i++) {
        auto& item = state->inventory[i];
        GridSlot gs; gs.empty = false; gs.text = item.name + " +" + std::to_string(item.enchant);
        inv_grid->SetSlot(i / 2, i % 2, gs);
    }
    inv_grid->OnSlotEvent([this, state](int row, int col, const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            int idx = row * 2 + col;
            if (idx >= 0 && idx < (int)state->inventory.size()) {
                selected_item_ = idx;
                auto& item = state->inventory[idx];
                char buf[256];
                snprintf(buf, sizeof(buf), "Selected: %s +%d (x%d)\n\nUpgrade: +%d → +%d (50%% success)\nCost: %dg\nSynthesize: 3x same item = +1 tier",
                    item.name.c_str(), item.enchant, item.count,
                    item.enchant, item.enchant + 1,
                    (item.enchant + 1) * 100);
                if (info_label_) info_label_->SetText(buf);
            }
        }
    });

    result_label_ = window_->AddWidget<Label>("", 10, 200, 0xff88ff88);

    auto* upgrade_btn = window_->AddWidget<Button>("Upgrade (+1)", 10, 280, 120, 24);
    upgrade_btn->SetColors({80,80,40,220}, {130,130,80,220}, {50,50,30,220});
    upgrade_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoUpgrade(state);
    });

    auto* synthesize_btn = window_->AddWidget<Button>("Synthesize (3→1)", 150, 280, 140, 24);
    synthesize_btn->SetColors({40,40,80,220}, {80,80,130,220}, {30,30,50,220});
    synthesize_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoSynthesize(state);
    });

    auto* safe_btn = window_->AddWidget<Button>("Safe Upgrade (2x cost)", 10, 320, 180, 24);
    safe_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    safe_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click && selected_item_ >= 0 && selected_item_ < (int)state->inventory.size()) {
            auto& item = state->inventory[selected_item_];
            int cost = (item.enchant + 1) * 200;
            if (state->gold >= cost) {
                state->gold -= cost;
                item.enchant++;
                char buf[128];
                snprintf(buf, sizeof(buf), "Safe upgrade success! +%d (Cost: %dg)", item.enchant, cost);
                state->chat_messages.push_back(buf);
                if (result_label_) result_label_->SetText(buf);
            } else {
                if (result_label_) result_label_->SetText("Not enough gold!");
            }
        }
    });
}

void UpgradeDialog::DoUpgrade(GameState* state) {
    if (selected_item_ < 0 || selected_item_ >= (int)state->inventory.size()) {
        if (result_label_) result_label_->SetText("No item selected!");
        return;
    }
    auto& item = state->inventory[selected_item_];
    if (item.enchant >= 15) {
        if (result_label_) result_label_->SetText("Already max enchant (+15)!");
        return;
    }
    int cost = (item.enchant + 1) * 100;
    if (state->gold < cost) {
        if (result_label_) result_label_->SetText("Not enough gold!");
        return;
    }
    state->gold -= cost;
    int roll = rand() % 100;
    if (roll < 50) { // 50% success
        item.enchant++;
        char buf[128];
        snprintf(buf, sizeof(buf), "Upgrade success! +%d!", item.enchant);
        state->chat_messages.push_back(buf);
        if (result_label_) result_label_->SetText(buf);
    } else {
        if (item.enchant > 0) item.enchant--;
        if (result_label_) result_label_->SetText("Upgrade failed! Enchant decreased.");
        state->chat_messages.push_back("Upgrade failed!");
    }
}

void UpgradeDialog::DoSynthesize(GameState* state) {
    if (selected_item_ < 0 || selected_item_ >= (int)state->inventory.size()) {
        if (result_label_) result_label_->SetText("No item selected!");
        return;
    }
    auto& item = state->inventory[selected_item_];
    if (item.count < 3) {
        if (result_label_) result_label_->SetText("Need 3x of same item!");
        return;
    }
    item.count -= 3;
    item.enchant = std::min(item.enchant + 2, 15);
    char buf[128];
    snprintf(buf, sizeof(buf), "Synthesis complete! +%d!", item.enchant);
    state->chat_messages.push_back(buf);
    if (result_label_) result_label_->SetText(buf);
}

void UpgradeDialog::UpdateFromState(GameState*) {}

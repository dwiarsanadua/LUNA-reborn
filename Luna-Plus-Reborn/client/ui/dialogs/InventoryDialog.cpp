#include "InventoryDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <algorithm>

InventoryDialog::InventoryDialog() {}

void InventoryDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;

    // Load from legacy script
    if (wm) {
        window_ = wm->LoadFromScript("assets/interface/Windows/Inven.bin.txt");
    }

    if (!window_) {
        spdlog::warn("InventoryDialog: failed to load UI script, using C++ fallback");
        window_ = wm->Open("Inventory", 40, 50, 355, 361);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    // Map gold label (IN_MONEYEDIT in script)
    gold_label_ = window_->FindWidget<Label>("IN_MONEYEDIT");
    
    // Map main inventory grid (usually hardcoded in legacy or separate block)
    // For now we manually add it to the window at the correct position
    inv_grid_ = window_->AddWidget<Grid>(5, 6, 17, 185, 34, 34);
    
    UpdateFromState(state, *g_ui);
}

void InventoryDialog::Refresh(GameState* state) {
    if (!window_) return;
    UpdateFromState(state, *g_ui);
}

void InventoryDialog::UpdateFromState(GameState* state, UIRenderer& ui) {
    if (!inv_grid_) return;
    inv_grid_->ClearAll();
    for (int i = 0; i < (int)state->inventory.size() && i < 30; i++) {
        auto& item = state->inventory[i];
        GridSlot gs;
        gs.empty = false;
        char buf[64]; snprintf(buf, sizeof(buf), "%s+%d", item.name.c_str(), item.enchant);
        gs.text = ""; // Don't show text inside small grid slots
        gs.count = item.count;
        gs.userdata = (int)item.id;
        
        // Dummy icon mapping based on ID for demonstration
        if (item.id % 3 == 0) gs.texture = ui.LoadTexture("icon_wep", "item_weapon_dagger_01.png");
        else if (item.id % 3 == 1) gs.texture = ui.LoadTexture("icon_pot", "item_material_potion_01.png");
        else gs.texture = ui.LoadTexture("icon_arm", "item_armor_metal_body_01.png");
        
        int row = i / 6, col = i % 6;
        inv_grid_->SetSlot(row, col, gs);
    }
    if (gold_label_) {
        char buf[64]; snprintf(buf, sizeof(buf), "%d", state->gold);
        gold_label_->SetText(buf);
    }
}

void InventoryDialog::Close() {
    // Note: window pointer is managed by WindowManager, we just clean our refs
    window_ = nullptr;
    inv_grid_ = nullptr;
    gold_label_ = nullptr;
}

void InventoryDialog::DrawEquipmentPreview(UIRenderer& ui, GameState* state, float x, float y) {
    (void)state;
    // Will be drawn as part of the background texture
}

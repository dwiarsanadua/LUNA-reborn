#include "StorageDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <algorithm>

void StorageDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;

    // Try script layout
    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/Storage.bin.txt");

    if (!window_) {
        spdlog::warn("StorageDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("STORAGE", 300, 100, 400, 350);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    auto* tabs = window_->AddWidget<TabPanel>(10, 28, 380, 280);

    // Storage grid (5x8 = 40 slots)
    storage_grid_ = new Grid(5, 8, 38, 38, 10, 10);
    tabs->AddTab("Storage", storage_grid_);

    // Info label
    gold_label_ = window_->AddWidget<Label>("Gold: 0", 10, 320, ColorPalette::TEXT_GOLD);

    items_.resize(40);
    for (int i = 0; i < 40; i++) items_[i] = {0, "", 0, i};
    if (state) UpdateFromState(state);
}

void StorageDialog::UpdateFromState(GameState* state) {
    if (!storage_grid_ || !state) return;

    for (int i = 0; i < 40; i++) items_[i] = {0, "", 0, i};
    for (const auto& s : state->storage_items) {
        if (s.slot < 40) {
            items_[s.slot] = {s.id, s.name.empty() ? ("Item_" + std::to_string(s.id)) : s.name, s.count, s.slot};
        }
    }

    storage_grid_->ClearAll();
    for (int i = 0; i < 40; i++) {
        auto& item = items_[i];
        if (item.id > 0) {
            GridSlot gs;
            gs.empty = false;
            gs.count = item.count;
            gs.text = item.name;
            gs.userdata = (int)item.id;
            int row = i / 8, col = i % 8;
            storage_grid_->SetSlot(row, col, gs);
        }
    }
    if (gold_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Storage Gold: %u", state->storage_gold);
        gold_label_->SetText(buf);
    }
}

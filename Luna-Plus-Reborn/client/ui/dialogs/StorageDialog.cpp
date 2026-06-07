#include "StorageDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <cstdio>
#include <algorithm>

void StorageDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;

    // Try script layout
    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/Storage.bin.txt");

    if (!window_) {
        window_ = new Window("STORAGE", 300, 100, 400, 350);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    auto* tabs = window_->AddWidget<TabPanel>(10, 28, 380, 280);

    // Storage grid (5x8 = 40 slots)
    storage_grid_ = new Grid(5, 8, 38, 38, 10, 10);
    tabs->AddTab("Storage", storage_grid_);

    // Info label
    gold_label_ = window_->AddWidget<Label>("Gold: 0", 10, 320, 0xffffcc88);

    // Initialize empty storage
    items_.resize(40);
    for (int i = 0; i < 40; i++) items_[i] = {0, "", 0, i};

    // Fill with some demo items if empty
    if (state && state->gold > 0) {
        items_[0] = {1001, "Health Potion", 10, 0};
        items_[1] = {1002, "Mana Potion", 5, 1};
        items_[2] = {1003, "Scroll of Recall", 2, 2};
    }
}

void StorageDialog::UpdateFromState(GameState* state) {
    if (!storage_grid_ || !state) return;

    storage_grid_->ClearAll();
    for (int i = 0; i < (int)items_.size() && i < 40; i++) {
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
        char buf[64]; snprintf(buf, sizeof(buf), "Gold: %d", state->gold);
        gold_label_->SetText(buf);
    }
}

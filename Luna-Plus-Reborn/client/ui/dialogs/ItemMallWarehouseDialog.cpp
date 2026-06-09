#include "ItemMallWarehouseDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void ItemMallWarehouseDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;
    state_ = state;

    window_ = wm->LoadFromScript("assets/interface/Windows/ItemMallWarehouse.bin.txt");
    if (!window_) {
        spdlog::warn("ItemMallWarehouseDialog: fallback to default window");
        window_ = new Window("Cash Shop Storage", 30, 210, 260, 360);
        window_->SetMovable(true);
        window_->SetClosable(true);
    }

    info_label_ = window_->AddWidget<Label>("Cash Shop Item Storage", 15, 10, ColorPalette::TEXT_GOLD);

    grid_ = new Grid(6, 6, 38, 38, 1, 1);
    window_->AddWidget<Grid>(grid_, 13, 86, 233, 233);

    window_->AddWidget<Button>("Close", 230, 5, 28, 29)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void ItemMallWarehouseDialog::SetItems(const std::vector<MallItem>& items) {
    items_ = items;
    RebuildGrid();
}

void ItemMallWarehouseDialog::SetWithdrawCallback(std::function<void(uint32_t, uint16_t)> on_withdraw) {
    on_withdraw_ = on_withdraw;
}

void ItemMallWarehouseDialog::RebuildGrid() {
    if (!grid_) return;
    grid_->ClearAll();

    int cols = 6;
    for (size_t i = 0; i < items_.size() && i < 36; i++) {
        const auto& item = items_[i];
        char label[64];
        snprintf(label, sizeof(label), "%s x%d", item.name.c_str(), item.count);
        GridSlot gs;
        gs.empty = false;
        gs.count = item.count;
        gs.text = label;
        gs.userdata = (int)item.item_id;
        int row = (int)i / cols;
        int col = (int)i % cols;
        grid_->SetSlot(row, col, gs);
    }

    grid_->OnSlotEvent([this](int row, int col, const UIEvent& e) {
        if (e.type == UIEvent::Click) DoWithdraw(row * 6 + col);
    });
}

void ItemMallWarehouseDialog::DoWithdraw(int slot) {
    if (slot < 0 || slot >= (int)items_.size()) return;
    if (on_withdraw_) on_withdraw_(items_[slot].item_id, items_[slot].count);
    items_.erase(items_.begin() + slot);
    RebuildGrid();
}

void ItemMallWarehouseDialog::UpdateFromState(GameState* state) {
    state_ = state;
}

void ItemMallWarehouseDialog::Close() {
    window_ = nullptr;
    grid_ = nullptr;
    info_label_ = nullptr;
    items_.clear();
}

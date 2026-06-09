#include "IdentificationDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <algorithm>

void IdentificationDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;
    state_ = state;

    window_ = wm->LoadFromScript("assets/interface/Windows/IdentificationDlg.bin.txt");
    if (!window_) {
        spdlog::warn("IdentificationDialog: fallback to default window");
        window_ = new Window("Identification", 235, 200, 330, 191);
        window_->SetMovable(true);
        window_->SetClosable(true);
    }

    info_label_ = window_->AddWidget<Label>("Select an item to identify", 15, 42, ColorPalette::TEXT_NORMAL);

    grid_ = new Grid(5, 2, 50, 50, 6, 6);
    window_->AddWidget<Grid>(grid_, 15, 60, 290, 100);

    identify_btn_ = window_->AddWidget<Button>("Identify", 172, 160, 64, 23);
    identify_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoIdentify(selected_slot_);
    });

    cancel_btn_ = window_->AddWidget<Button>("Cancel", 255, 160, 64, 23);
    cancel_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });

    window_->AddWidget<Button>("", 294, 6, 28, 29)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void IdentificationDialog::SetIdentifiableItems(const std::vector<int>& inv_slots) {
    identifiable_slots_ = inv_slots;
    selected_slot_ = -1;
    RebuildGrid();
}

void IdentificationDialog::SetIdentifyCallback(std::function<void(int)> on_identify) {
    on_identify_ = on_identify;
}

void IdentificationDialog::RebuildGrid() {
    if (!grid_ || !state_) return;
    grid_->ClearAll();

    int cols = 5;
    for (size_t i = 0; i < identifiable_slots_.size() && i < 10; i++) {
        int slot_idx = identifiable_slots_[i];
        if (slot_idx < 0 || slot_idx >= (int)state_->inventory.size()) continue;
        const auto& inv = state_->inventory[slot_idx];
        if (inv.id == 0) continue;
        char label[64];
        snprintf(label, sizeof(label), "%s", inv.name.c_str());
        GridSlot gs;
        gs.empty = false;
        gs.count = inv.count;
        gs.text = label;
        gs.userdata = slot_idx;
        int row = (int)i / cols;
        int col = (int)i % cols;
        grid_->SetSlot(row, col, gs);
    }

    grid_->OnSlotEvent([this](int row, int col, const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            int idx = row * 5 + col;
            if (idx >= 0 && idx < (int)identifiable_slots_.size()) {
                selected_slot_ = identifiable_slots_[idx];
                char buf[64];
                snprintf(buf, sizeof(buf), "Selected slot %d - click Identify", selected_slot_);
                info_label_->SetText(buf);
            }
        }
    });
}

void IdentificationDialog::DoIdentify(int slot) {
    if (slot < 0) return;
    if (on_identify_) on_identify_(slot);
    identifiable_slots_.erase(
        std::remove(identifiable_slots_.begin(), identifiable_slots_.end(), slot),
        identifiable_slots_.end());
    selected_slot_ = -1;
    info_label_->SetText("Item identified! Select another or close.");
    RebuildGrid();
}

void IdentificationDialog::UpdateFromState(GameState* state) {
    state_ = state;
}

void IdentificationDialog::Close() {
    window_ = nullptr;
    grid_ = nullptr;
    info_label_ = nullptr;
    identify_btn_ = nullptr;
    cancel_btn_ = nullptr;
    identifiable_slots_.clear();
    selected_slot_ = -1;
}

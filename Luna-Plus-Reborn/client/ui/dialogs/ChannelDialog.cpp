#include "ChannelDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void ChannelDialog::Open(GameState* state, WindowManager* wm) {
    (void)state;
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/Channel.bin.txt",
        "Select Channel", 200, 150, 300, 250);
    window_->SetClosable(true);
    window_->SetMovable(true);

    title_label_ = window_->AddWidget<Label>("Select a Channel", 20, 10, ColorPalette::TEXT_GOLD);

    grid_ = new Grid(1, 6, 260, 30, 0, 0);
    window_->AddWidget<Grid>(grid_, 20, 40, 260, 180);

    window_->AddWidget<Button>("Close", 110, 220, 80, 24)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void ChannelDialog::SetChannels(const std::vector<ChannelInfo>& channels) {
    channels_ = channels;
    RebuildGrid();
}

void ChannelDialog::SetChannelCallback(std::function<void(int)> on_select) {
    on_select_ = on_select;
}

void ChannelDialog::RebuildGrid() {
    if (!grid_) return;
    grid_->ClearAll();

    for (size_t i = 0; i < channels_.size(); i++) {
        const auto& ch = channels_[i];
        char label[64];
        snprintf(label, sizeof(label), "%s (%d%%)", ch.name.c_str(), ch.load_pct);
        GridSlot gs;
        gs.empty = false;
        gs.text = label;
        gs.userdata = ch.id;
        grid_->SetSlot((int)i, 0, gs);
    }

    grid_->OnSlotEvent([this](int row, int, const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            if (row >= 0 && row < (int)channels_.size() && on_select_) {
                on_select_(channels_[row].id);
            }
        }
    });
}

void ChannelDialog::UpdateFromState(GameState* state) {
    (void)state;
}

void ChannelDialog::Close() {
    window_ = nullptr;
    grid_ = nullptr;
    title_label_ = nullptr;
    channels_.clear();
}

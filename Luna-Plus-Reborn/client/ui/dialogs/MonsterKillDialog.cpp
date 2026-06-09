#include "MonsterKillDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void MonsterKillDialog::Open(GameState* state, WindowManager* wm) {
    (void)state;
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/MonsterKill.bin.txt",
        "Monster Kill Tracker", 820, 520, 200, 160);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(20);

    title_label_ = window_->AddWidget<Label>("Kill Tracker", 10, 4, ColorPalette::TEXT_GOLD);

    window_->AddWidget<Button>("", 170, 3, 24, 24)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void MonsterKillDialog::SetKillTracking(const std::vector<KillTrackEntry>& entries) {
    entries_ = entries;
    RebuildDisplay();
}

void MonsterKillDialog::RebuildDisplay() {
    for (auto* lbl : entry_labels_) {
        lbl->SetVisible(false);
    }
    entry_labels_.clear();

    float y = 30;
    for (size_t i = 0; i < entries_.size() && i < 8; i++) {
        const auto& e = entries_[i];
        char buf[64];
        snprintf(buf, sizeof(buf), "%s: %d/%d", e.name.c_str(), e.killed, e.required);
        uint32_t color = (e.killed >= e.required) ? ColorPalette::TEXT_GOLD : ColorPalette::TEXT_NORMAL;
        auto* lbl = window_->AddWidget<Label>(buf, 10, y, color);
        entry_labels_.push_back(lbl);
        y += 20;
    }
}

void MonsterKillDialog::UpdateFromState(GameState* state) {
    (void)state;
}

void MonsterKillDialog::Close() {
    window_ = nullptr;
    title_label_ = nullptr;
    entry_labels_.clear();
    entries_.clear();
}

#include "QuestQuickViewDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void QuestQuickViewDialog::Open(GameState* state, WindowManager* wm) {
    (void)state;
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/QuestQuickView.bin.txt",
        "Quest Tracker", 800, 200, 220, 300);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(20);

    auto* title = window_->AddWidget<Label>("Quest Tracker", 10, 4, ColorPalette::TEXT_GOLD);
    (void)title;

    window_->AddWidget<Button>("", 190, 3, 24, 24)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void QuestQuickViewDialog::SetQuests(const std::vector<QuickQuestEntry>& quests) {
    quests_ = quests;
    RebuildDisplay();
}

void QuestQuickViewDialog::RebuildDisplay() {
    for (auto* lbl : quest_labels_) {
        lbl->SetVisible(false);
    }
    quest_labels_.clear();

    float y = 30;
    for (size_t i = 0; i < quests_.size() && i < 10; i++) {
        const auto& q = quests_[i];
        char buf[128];
        if (q.completed) {
            snprintf(buf, sizeof(buf), "[DONE] %s", q.name.c_str());
        } else {
            snprintf(buf, sizeof(buf), "%s: %d/%d", q.objective.c_str(), q.progress, q.required);
        }
        uint32_t color = q.completed ? ColorPalette::TEXT_GOLD : ColorPalette::TEXT_NORMAL;
        auto* lbl = window_->AddWidget<Label>(buf, 10, y, color);
        quest_labels_.push_back(lbl);
        y += 22;
    }
}

void QuestQuickViewDialog::UpdateFromState(GameState* state) {
    (void)state;
}

void QuestQuickViewDialog::Close() {
    window_ = nullptr;
    quest_labels_.clear();
    quests_.clear();
}

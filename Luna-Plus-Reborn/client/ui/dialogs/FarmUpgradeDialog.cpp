#include "FarmUpgradeDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <cstdio>
#include <algorithm>

int FarmUpgradeDialog::UpgradeGoldCost(int grade) {
    return grade * 5000;
}

int FarmUpgradeDialog::UpgradeHonorCost(int grade) {
    return grade * 200;
}

void FarmUpgradeDialog::Open(GameState* state, WindowManager* wm,
                             int farm_grade, int farm_zone, int farm_id,
                             UpgradeCallback on_upgrade) {
    if (window_) return;

    farm_grade_ = std::min(farm_grade, kMaxGrade);
    farm_zone_ = farm_zone;
    farm_id_ = farm_id;
    on_upgrade_ = std::move(on_upgrade);

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/Farm_Upgrade.bin.txt",
        "Farm Upgrade", 300, 160, 380, 280);
    window_->SetClosable(true);
    window_->SetMovable(false);
    window_->SetTitleBarH(24);

    Refresh(state);
}

void FarmUpgradeDialog::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
    on_upgrade_ = nullptr;
}

void FarmUpgradeDialog::Refresh(GameState* state) {
    if (!window_ || !state) return;

    bool max_grade = farm_grade_ >= kMaxGrade;

    // Current grade
    char buf[64];
    snprintf(buf, sizeof(buf), "Grade %d / %d", farm_grade_, kMaxGrade);
    auto* cur_label = window_->FindWidget<Label>("cur_grade");
    if (!cur_label) {
        cur_label = window_->AddWidget<Label>(buf, 92, 37, ColorPalette::TEXT_GOLD_BOLD);
        cur_label->SetID("cur_grade");
    } else {
        cur_label->SetText(buf);
    }

    // Next grade
    snprintf(buf, sizeof(buf), max_grade ? "MAX" : "Grade %d", std::min(farm_grade_ + 1, kMaxGrade));
    auto* next_label = window_->FindWidget<Label>("next_grade");
    if (!next_label) {
        next_label = window_->AddWidget<Label>(buf, 92, 64, ColorPalette::TEXT_GOLD_BOLD);
        next_label->SetID("next_grade");
    } else {
        next_label->SetText(buf);
    }

    // Required gold
    int gold_cost = max_grade ? 0 : UpgradeGoldCost(farm_grade_);
    uint32_t gold_color = (state->gold >= (uint32_t)gold_cost) ? ColorPalette::TEXT_GOLD_BOLD : ColorPalette::TEXT_ERROR;
    snprintf(buf, sizeof(buf), max_grade ? "--" : "%d g", gold_cost);
    auto* gold_label = window_->FindWidget<Label>("need_gold");
    if (!gold_label) {
        gold_label = window_->AddWidget<Label>(buf, 92, 91, gold_color);
        gold_label->SetID("need_gold");
    } else {
        gold_label->SetText(buf);
        gold_label->SetColor(gold_color);
    }

    // Required honor
    int honor_cost = max_grade ? 0 : UpgradeHonorCost(farm_grade_);
    snprintf(buf, sizeof(buf), max_grade ? "--" : "%d", honor_cost);
    auto* honor_label = window_->FindWidget<Label>("need_honor");
    if (!honor_label) {
        honor_label = window_->AddWidget<Label>(buf, 92, 118, ColorPalette::TEXT_GOLD_BOLD);
        honor_label->SetID("need_honor");
    } else {
        honor_label->SetText(buf);
    }

    // OK / Upgrade button
    auto* ok_btn = window_->FindWidget<Button>("upgrade_ok");
    if (!ok_btn) {
        ok_btn = window_->AddWidget<Button>(max_grade ? "Maxed" : "Upgrade", 15, 146, 75, 23);
        ok_btn->SetID("upgrade_ok");
        ok_btn->SetEnabled(!max_grade);
        ok_btn->SetColors({50, 80, 50, 220}, {80, 130, 80, 220}, {30, 60, 30, 220});
        ok_btn->OnEvent([this](const UIEvent& e) {
            if (e.type == UIEvent::Click && on_upgrade_ && farm_grade_ < kMaxGrade) {
                on_upgrade_(farm_zone_, farm_id_, 0);
                farm_grade_++;
                if (farm_grade_ >= kMaxGrade) {
                    on_upgrade_ = nullptr;
                }
            }
        });
    } else {
        ok_btn->SetText(max_grade ? "Maxed" : "Upgrade");
        ok_btn->SetEnabled(!max_grade);
    }

    // Cancel / Close button
    auto* cancel_btn = window_->FindWidget<Button>("upgrade_cancel");
    if (!cancel_btn) {
        cancel_btn = window_->AddWidget<Button>("Cancel", 110, 146, 75, 23);
        cancel_btn->SetID("upgrade_cancel");
        cancel_btn->SetColors({80, 50, 50, 220}, {130, 80, 80, 220}, {60, 30, 30, 220});
        cancel_btn->OnEvent([this](const UIEvent& e) {
            if (e.type == UIEvent::Click) Close();
        });
    }
}

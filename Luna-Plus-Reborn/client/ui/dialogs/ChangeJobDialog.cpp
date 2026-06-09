#include "ChangeJobDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/UiScriptParser.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

std::string ChangeJobDialog::ClassNameForId(int id) {
    switch (id) {
        case 0: return "Warrior";
        case 1: return "Mage";
        case 2: return "Archer";
        default: return "Unknown";
    }
}

std::string ChangeJobDialog::ClassDescription(int id) {
    switch (id) {
        case 0:
            return "A master of melee combat. Wields swords and axes with devastating power. High HP and defense.";
        case 1:
            return "A wielder of elemental magic. Casts powerful spells to damage enemies from afar.";
        case 2:
            return "A skilled ranged attacker. Uses bows and crossbows with precision and agility.";
        default:
            return "";
    }
}

void ChangeJobDialog::Open(GameState* state, WindowManager* wm,
                           int target_class_id, const std::string& class_name,
                           const std::string& description, int cost_gold,
                           ConfirmCallback on_confirm) {
    if (window_) return;

    target_class_id_ = target_class_id;
    class_name_ = class_name.empty() ? ClassNameForId(target_class_id) : class_name;
    description_ = description.empty() ? ClassDescription(target_class_id) : description;
    cost_gold_ = cost_gold;
    on_confirm_ = std::move(on_confirm);

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/Changejob.bin.txt");

    if (!window_) {
        spdlog::warn("ChangeJobDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("Change Job", 260, 140, 400, 320);
        window_->SetClosable(true);
        window_->SetMovable(true);
        window_->SetTitleBarH(24);
    }

    Refresh(state);
}

void ChangeJobDialog::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
    on_confirm_ = nullptr;
}

void ChangeJobDialog::Refresh(GameState* state) {
    if (!window_ || !state) return;

    auto find_or_add_label = [this](const std::string& id, float x, float y, uint32_t color) -> Label* {
        if (auto* lbl = window_->FindWidget<Label>(id)) {
            return lbl;
        }
        auto* lbl = window_->AddWidget<Label>("", x, y, color);
        lbl->SetID(id);
        return lbl;
    };

    char buf[256];

    // Info label: current -> target
    std::string cur_name = ClassNameForId(state->class_id);
    snprintf(buf, sizeof(buf), "Current: %s  ->  %s", cur_name.c_str(), class_name_.c_str());
    auto* info_label = find_or_add_label("cj_info", 14, 4, ColorPalette::TEXT_HIGHLIGHT);
    info_label->SetText(buf);

    // Description
    auto* desc_label = find_or_add_label("cj_desc", 14, 24, ColorPalette::TEXT_NORMAL);
    desc_label->SetText(description_);

    // Cost
    uint32_t cost_color = (state->gold >= (uint32_t)cost_gold_)
        ? ColorPalette::TEXT_GOLD : ColorPalette::TEXT_ERROR;
    snprintf(buf, sizeof(buf), "Cost: %d gold", cost_gold_);
    auto* cost_label = find_or_add_label("cj_cost", 14, 80, cost_color);
    cost_label->SetText(buf);
    cost_label->SetColor(cost_color);

    // OK / Confirm button
    auto* ok_btn = window_->FindWidget<Button>("cj_ok");
    if (!ok_btn) {
        ok_btn = window_->AddWidget<Button>("Change", 30, 118, 56, 19);
        ok_btn->SetID("cj_ok");
        ok_btn->SetColors({50, 80, 50, 220}, {80, 130, 80, 220}, {30, 60, 30, 220});
        ok_btn->OnEvent([this](const UIEvent& e) {
            if (e.type == UIEvent::Click && on_confirm_) {
                on_confirm_(target_class_id_);
            }
        });
    }

    // Cancel button
    auto* cancel_btn = window_->FindWidget<Button>("cj_cancel");
    if (!cancel_btn) {
        cancel_btn = window_->AddWidget<Button>("Cancel", 110, 118, 56, 19);
        cancel_btn->SetID("cj_cancel");
        cancel_btn->SetColors({80, 50, 50, 220}, {130, 80, 80, 220}, {60, 30, 30, 220});
        cancel_btn->OnEvent([this](const UIEvent& e) {
            if (e.type == UIEvent::Click) Close();
        });
    }

    // Disable OK if not enough gold
    if (ok_btn) {
        bool can_afford = state->gold >= (uint32_t)cost_gold_;
        ok_btn->SetEnabled(can_afford);
    }
}

#include "EnchantDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <cstdlib>

void EnchantDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;
    state_ = state;

    window_ = wm->LoadFromScript("assets/interface/Windows/EnchantDialog.bin.txt");
    if (!window_) {
        spdlog::warn("EnchantDialog: fallback to default window");
        window_ = new Window("Enchant Equipment", 297, 200, 200, 175);
        window_->SetMovable(true);
        window_->SetClosable(true);
    }

    info_label_ = window_->AddWidget<Label>("Select enchant scroll and target item", 10, 42, ColorPalette::TEXT_NORMAL);
    cost_label_ = window_->AddWidget<Label>("Cost: 500g", 90, 130, ColorPalette::TEXT_GOLD);
    result_label_ = window_->AddWidget<Label>("", 10, 150, ColorPalette::TEXT_NORMAL);

    enchant_btn_ = window_->AddWidget<Button>("Enchant", 60, 80, 80, 24);
    enchant_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoEnchant();
    });

    window_->AddWidget<Button>("Close", 160, 5, 28, 29)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void EnchantDialog::SetEnchantInfo(const EnchantScrollInfo& info) {
    current_info_ = info;
    char buf[128];
    snprintf(buf, sizeof(buf), "Scroll: %s -> %s (Success: %d%%)",
             info.scroll_name.c_str(), info.target_name.c_str(), info.success_rate);
    info_label_->SetText(buf);
    result_label_->SetText("");
}

void EnchantDialog::SetEnchantCallback(std::function<void(uint32_t, uint32_t)> on_enchant) {
    on_enchant_ = on_enchant;
}

void EnchantDialog::DoEnchant() {
    if (current_info_.scroll_item_id == 0 || current_info_.target_item_id == 0) {
        result_label_->SetText("Select items first!");
        return;
    }

    if (on_enchant_) on_enchant_(current_info_.scroll_item_id, current_info_.target_item_id);

    int roll = rand() % 100;
    if (roll < current_info_.success_rate) {
        result_label_->SetText("Enchantment successful!");
        spdlog::info("EnchantDialog: success on item {}", current_info_.target_item_id);
    } else {
        result_label_->SetText("Enchantment failed...");
        spdlog::info("EnchantDialog: failed on item {}", current_info_.target_item_id);
    }
}

void EnchantDialog::UpdateFromState(GameState* state) {
    state_ = state;
}

void EnchantDialog::Close() {
    window_ = nullptr;
    info_label_ = nullptr;
    result_label_ = nullptr;
    cost_label_ = nullptr;
    enchant_btn_ = nullptr;
    current_info_ = {};
}

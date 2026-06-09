#include "SiegeWarFlagDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void SiegeWarFlagDialog::Open(GameState* state, WindowManager* wm) {
    if (window_) return;
    state_ = state;

    window_ = wm->LoadFromScript("assets/interface/Windows/SiegeWarFlagDlg.bin.txt");
    if (!window_) {
        spdlog::warn("SiegeWarFlagDialog: fallback to default window");
        window_ = new Window("Siege Flag", 400, 200, 160, 100);
        window_->SetMovable(true);
        window_->SetClosable(true);
    }

    title_label_ = window_->AddWidget<Label>("Siege War Flag", 15, 10, ColorPalette::TEXT_GOLD);

    flag_name_label_ = window_->AddWidget<Label>("Flag: --", 15, 60, ColorPalette::TEXT_NORMAL);
    territory_label_ = window_->AddWidget<Label>("Territory: --", 15, 105, ColorPalette::TEXT_NORMAL);

    plant_btn_ = window_->AddWidget<Button>("Plant Flag", 20, 140, 100, 24);
    plant_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoPlant();
    });

    remove_btn_ = window_->AddWidget<Button>("Remove Flag", 20, 170, 100, 24);
    remove_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoRemove();
    });

    window_->AddWidget<Button>("Close", 120, 5, 30, 24)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void SiegeWarFlagDialog::SetFlagInfo(const std::string& flag_name, uint32_t territory_id) {
    flag_name_ = flag_name;
    territory_id_ = territory_id;
    if (flag_name_label_) flag_name_label_->SetText("Flag: " + flag_name);
    if (territory_label_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Territory: %u", territory_id);
        territory_label_->SetText(buf);
    }
}

void SiegeWarFlagDialog::SetPlantCallback(std::function<void(uint32_t)> on_plant) {
    on_plant_ = on_plant;
}

void SiegeWarFlagDialog::SetRemoveCallback(std::function<void(uint32_t)> on_remove) {
    on_remove_ = on_remove;
}

void SiegeWarFlagDialog::DoPlant() {
    if (on_plant_) {
        on_plant_(territory_id_);
        spdlog::info("SiegeWarFlag: flag planted on territory {}", territory_id_);
    }
}

void SiegeWarFlagDialog::DoRemove() {
    if (on_remove_) {
        on_remove_(territory_id_);
        spdlog::info("SiegeWarFlag: flag removed from territory {}", territory_id_);
    }
}

void SiegeWarFlagDialog::UpdateFromState(GameState* state) {
    state_ = state;
}

void SiegeWarFlagDialog::Close() {
    window_ = nullptr;
    title_label_ = nullptr;
    flag_name_label_ = nullptr;
    territory_label_ = nullptr;
    plant_btn_ = nullptr;
    remove_btn_ = nullptr;
}

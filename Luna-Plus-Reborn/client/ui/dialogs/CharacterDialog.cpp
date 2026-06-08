#include "CharacterDialog.hpp"
#include <spdlog/spdlog.h>
#include <cstdio>

void CharacterDialog::Open(GameState* state, WindowManager* wm) {
    if (wm) {
        window_ = wm->LoadFromScript("assets/interface/Windows/CharInfo.bin.txt");
    }

    if (!window_) {
        spdlog::warn("CharacterDialog: failed to load UI script, using C++ fallback");
        window_ = wm->Open("Character", 350, 60, 400, 420);
        window_->SetClosable(true);
        window_->SetMovable(true);
        window_->SetTitleBarH(24);
    }
    
    Refresh(state);
}

void CharacterDialog::Refresh(GameState* state) {
    if (!window_) return;

    auto set_text = [this](const std::string& id, const std::string& val) {
        if (auto* lbl = window_->FindWidget<Label>(id)) {
            lbl->SetText(val);
        }
    };

    set_text("CI_CHANGED_TEXT0", std::to_string(state->level));
    set_text("CI_CHANGED_TEXT1", state->name);
    set_text("CI_CHANGED_TEXT4", std::to_string(state->stat_str));
    set_text("CI_CHANGED_TEXT5", std::to_string(state->stat_dex));
    set_text("CI_CHANGED_TEXT6", std::to_string(state->stat_int));
    set_text("CI_CHANGED_TEXT7", std::to_string(state->stat_con));
    
    // Combat stats
    set_text("CI_CHANGED_TEXT10", std::to_string(state->hp) + "/" + std::to_string(state->max_hp));
    set_text("CI_CHANGED_TEXT11", std::to_string(state->mp) + "/" + std::to_string(state->max_mp));
    set_text("CI_CHANGED_TEXT12", std::to_string(state->attack));
    set_text("CI_CHANGED_TEXT13", std::to_string(state->defense));
}

void CharacterDialog::UpdateFromState(GameState* state) {
    if (window_) Refresh(state);
}

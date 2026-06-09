#include "CharDelDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void CharDelDialog::Open(GameState* state, WindowManager* wm) {
    (void)state;
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/CharDel.bin.txt",
        "Delete Character", 300, 200, 280, 160);
    window_->SetClosable(true);
    window_->SetMovable(true);

    info_label_ = window_->AddWidget<Label>("Enter password to confirm deletion", 20, 20, ColorPalette::TEXT_WARNING);

    password_input_ = window_->AddWidget<InputField>(20, 50, 240, 24);
    password_input_->SetPlaceholder("Password");

    confirm_btn_ = window_->AddWidget<Button>("Delete", 90, 90, 100, 28);
    confirm_btn_->SetColors({80, 30, 30, 220}, {130, 50, 50, 220}, {50, 20, 20, 220});
    confirm_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoDelete();
    });

    window_->AddWidget<Button>("Cancel", 200, 5, 28, 29)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void CharDelDialog::SetDeleteCallback(std::function<void(const std::string&)> on_delete) {
    on_delete_ = on_delete;
}

void CharDelDialog::DoDelete() {
    std::string pw = password_input_ ? password_input_->GetText() : "";
    if (pw.empty()) {
        if (info_label_) info_label_->SetText("Please enter your password");
        return;
    }
    if (on_delete_) on_delete_(pw);
    Close();
}

void CharDelDialog::UpdateFromState(GameState* state) {
    (void)state;
}

void CharDelDialog::Close() {
    window_ = nullptr;
    info_label_ = nullptr;
    password_input_ = nullptr;
    confirm_btn_ = nullptr;
}

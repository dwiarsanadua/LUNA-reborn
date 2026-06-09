#include "FriendInviteDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>

void FriendInviteDialog::Open(GameState* state, WindowManager* wm) {
    (void)state;
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/FriendInvite.bin.txt",
        "Invite Friend", 350, 250, 260, 140);
    window_->SetClosable(true);
    window_->SetMovable(true);

    info_label_ = window_->AddWidget<Label>("Enter friend's name to invite", 20, 20, ColorPalette::TEXT_NORMAL);

    name_input_ = window_->AddWidget<InputField>(20, 50, 220, 24);
    name_input_->SetPlaceholder("Character name");

    invite_btn_ = window_->AddWidget<Button>("Invite", 80, 90, 100, 28);
    invite_btn_->SetColors({50, 80, 50, 220}, {80, 130, 80, 220}, {30, 50, 30, 220});
    invite_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoInvite();
    });

    window_->AddWidget<Button>("Cancel", 220, 5, 28, 29)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void FriendInviteDialog::SetInviteCallback(std::function<void(const std::string&)> on_invite) {
    on_invite_ = on_invite;
}

void FriendInviteDialog::DoInvite() {
    std::string name = name_input_ ? name_input_->GetText() : "";
    if (name.empty()) {
        if (info_label_) info_label_->SetText("Please enter a character name");
        return;
    }
    if (on_invite_) on_invite_(name);
    name_input_->SetText("");
    Close();
}

void FriendInviteDialog::UpdateFromState(GameState* state) {
    (void)state;
}

void FriendInviteDialog::Close() {
    window_ = nullptr;
    info_label_ = nullptr;
    name_input_ = nullptr;
    invite_btn_ = nullptr;
}

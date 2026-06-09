#include "GuildCreateDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>

void GuildCreateDialog::Open(GameState* state, WindowManager* wm) {
    (void)state;
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/GuildCreate.bin.txt",
        "Create Guild", 350, 200, 320, 220);
    window_->SetClosable(true);
    window_->SetMovable(true);

    info_label_ = window_->AddWidget<Label>("Create a new Guild", 20, 15, ColorPalette::TEXT_GOLD);

    window_->AddWidget<Label>("Guild Name:", 20, 50, ColorPalette::TEXT_NORMAL);
    name_input_ = window_->AddWidget<InputField>(120, 48, 180, 22);
    name_input_->SetPlaceholder("Enter guild name");

    window_->AddWidget<Label>("Description:", 20, 85, ColorPalette::TEXT_NORMAL);
    desc_input_ = window_->AddWidget<InputField>(120, 83, 180, 22);
    desc_input_->SetPlaceholder("Optional description");

    create_btn_ = window_->AddWidget<Button>("Create Guild", 110, 130, 120, 28);
    create_btn_->SetColors({50, 80, 50, 220}, {80, 130, 80, 220}, {30, 50, 30, 220});
    create_btn_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) DoCreate();
    });

    window_->AddWidget<Button>("Cancel", 280, 5, 28, 29)->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void GuildCreateDialog::SetCreateCallback(std::function<void(const std::string&, const std::string&)> on_create) {
    on_create_ = on_create;
}

void GuildCreateDialog::DoCreate() {
    std::string name = name_input_ ? name_input_->GetText() : "";
    std::string desc = desc_input_ ? desc_input_->GetText() : "";
    if (name.empty()) {
        if (info_label_) info_label_->SetText("Please enter a guild name");
        return;
    }
    if (on_create_) on_create_(name, desc);
    Close();
}

void GuildCreateDialog::UpdateFromState(GameState* state) {
    (void)state;
}

void GuildCreateDialog::Close() {
    window_ = nullptr;
    info_label_ = nullptr;
    name_input_ = nullptr;
    desc_input_ = nullptr;
    create_btn_ = nullptr;
}

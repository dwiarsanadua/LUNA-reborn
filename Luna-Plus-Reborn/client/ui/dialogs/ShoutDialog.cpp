#include "ShoutDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <ui/widgets/InputField.hpp>

void ShoutDialog::Open(WindowManager* wm) {
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/ShoutDlg.bin.txt",
        "Shout", 300, 200, 360, 180);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    auto* lbl = window_->AddWidget<Label>("Send a message to everyone in the area:", 20, 10, ColorPalette::TEXT_NORMAL);
    (void)lbl;

    input_ = window_->AddWidget<InputField>(20, 40, 320, 28);
    input_->SetPlaceholder("Enter shout message...");
    input_->SetMaxLength(120);

    auto* send_btn = window_->AddWidget<Button>("Send", 100, 90, 80, 28);
    send_btn->SetColors({60, 100, 60, 220}, {90, 150, 90, 220}, {40, 60, 40, 220});
    send_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && input_ && on_send_) {
            std::string msg = input_->GetText();
            if (!msg.empty()) on_send_(msg);
        }
    });

    auto* close_btn = window_->AddWidget<Button>("Cancel", 200, 90, 80, 28);
    close_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void ShoutDialog::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
    input_ = nullptr;
}

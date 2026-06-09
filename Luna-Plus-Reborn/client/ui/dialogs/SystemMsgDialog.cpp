#include "SystemMsgDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>

void SystemMsgDialog::Open(WindowManager* wm, const std::string& title, const std::string& message) {
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/SystemMsg.bin.txt",
        title, 200, 160, 400, 150);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>(message, 20, 30, ColorPalette::TEXT_NORMAL);

    auto* btn = window_->AddWidget<Button>("OK", 160, 100, 80, 28);
    btn->SetColors({60, 80, 100, 220}, {100, 130, 160, 220}, {40, 50, 70, 220});
    btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void SystemMsgDialog::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

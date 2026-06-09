#include "BillingDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>

void BillingDialog::Open(WindowManager* wm) {
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/BillingDlg.bin.txt",
        "Billing Info", 250, 180, 380, 220);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Billing / Account Information", 20, 10, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("Status: Active", 20, 45, ColorPalette::HP_HIGH);
    window_->AddWidget<Label>("Plan: Standard", 20, 70, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("Next Payment: --", 20, 95, ColorPalette::TEXT_NORMAL);

    auto* close_btn = window_->AddWidget<Button>("Close", 150, 150, 80, 28);
    close_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void BillingDialog::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

void BillingDialog::UpdateFromState(GameState* state) {
    (void)state;
}

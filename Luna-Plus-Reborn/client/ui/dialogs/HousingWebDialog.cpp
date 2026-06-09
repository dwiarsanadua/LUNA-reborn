#include "HousingWebDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>

void HousingWebDialog::Open(WindowManager* wm, const std::string& url) {
    if (window_) return;

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/HousingWebDlg.bin.txt",
        "Housing Web", 150, 80, 600, 480);
    window_->SetClosable(true);
    window_->SetMovable(true);

    if (url.empty()) {
        window_->AddWidget<Label>("Housing Browser", 20, 20, ColorPalette::TEXT_GOLD);
        window_->AddWidget<Label>("Web view not available in offline mode.", 20, 60, ColorPalette::TEXT_NORMAL);
        window_->AddWidget<Label>("Use the Housing menu instead.", 20, 90, ColorPalette::TEXT_DISABLED);
    } else {
        window_->AddWidget<Label>("Loading: " + url, 20, 20, ColorPalette::TEXT_NORMAL);
    }

    auto* close_btn = window_->AddWidget<Button>("Close", 260, 420, 80, 28);
    close_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void HousingWebDialog::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

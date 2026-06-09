#include "ReinforceGuideDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>

void ReinforceGuideDialog::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/ReinforceGuideDialog.bin.txt",
        "Reinforce Guide", 400, 200, 320, 240);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Equipment Reinforce Guide", 40, 36, ColorPalette::TEXT_GOLD);
    window_->AddWidget<Label>("+1 to +3: 100% success", 40, 65, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("+4 to +6: 70% success", 40, 85, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("+7 to +9: 40% success", 40, 105, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("+10: 10% success", 40, 125, ColorPalette::TEXT_ERROR);

    auto* close_btn = window_->AddWidget<Button>("Close", 120, 160, 80, 28);
    close_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void ReinforceGuideDialog::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

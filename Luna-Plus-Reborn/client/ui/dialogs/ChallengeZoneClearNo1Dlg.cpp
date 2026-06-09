#include "ChallengeZoneClearNo1Dlg.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>

void ChallengeZoneClearNo1Dlg::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/ChallengeZoneClearNo1Dlg.bin.txt",
        "Challenge Clear", 430, 280, 280, 150);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Challenge Zone Cleared!", 40, 40, ColorPalette::TEXT_GOLD);
    window_->AddWidget<Label>("Rank: No. 1", 70, 65, ColorPalette::TEXT_HIGHLIGHT);

    auto* ok_btn = window_->AddWidget<Button>("OK", 100, 100, 80, 28);
    ok_btn->SetColors({60, 80, 100, 220}, {100, 130, 160, 220}, {40, 50, 70, 220});
    ok_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            spdlog::info("ChallengeZoneClearNo1Dlg: acknowledged");
            Close();
        }
    });
}

void ChallengeZoneClearNo1Dlg::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

#include "PartyMatchingSetting.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>

void PartyMatchingSetting::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/PartyMatchingSetting.bin.txt",
        "Party Matching", 400, 240, 300, 200);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Party Matching Settings", 40, 36, ColorPalette::TEXT_GOLD);
    window_->AddWidget<Label>("Role: Any", 50, 65, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("Level Range: 1-99", 50, 90, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("Auto-Accept: Off", 50, 115, ColorPalette::TEXT_NORMAL);

    auto* close_btn = window_->AddWidget<Button>("Close", 110, 150, 80, 28);
    close_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void PartyMatchingSetting::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

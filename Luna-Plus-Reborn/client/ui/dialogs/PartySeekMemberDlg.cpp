#include "PartySeekMemberDlg.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>

void PartySeekMemberDlg::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/PartySeekMemberDlg.bin.txt",
        "Seek Member", 400, 250, 300, 180);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Seek Party Members", 40, 36, ColorPalette::TEXT_GOLD);
    window_->AddWidget<Label>("Looking for: Any class", 40, 65, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("Level: 1-99", 40, 90, ColorPalette::TEXT_NORMAL);

    auto* seek_btn = window_->AddWidget<Button>("Seek", 50, 120, 80, 28);
    seek_btn->SetColors({60, 100, 60, 220}, {100, 160, 100, 220}, {40, 60, 40, 220});
    seek_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) spdlog::info("PartySeekMemberDlg: seeking members");
    });

    auto* cancel_btn = window_->AddWidget<Button>("Cancel", 160, 120, 80, 28);
    cancel_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    cancel_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void PartySeekMemberDlg::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

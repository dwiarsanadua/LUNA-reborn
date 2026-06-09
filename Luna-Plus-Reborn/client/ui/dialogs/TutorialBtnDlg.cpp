#include "TutorialBtnDlg.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>

void TutorialBtnDlg::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/TutorialBtnDlg.bin.txt",
        "Tutorial", 400, 300, 260, 120);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Tutorial Mode Active", 40, 36, ColorPalette::TEXT_GOLD);

    auto* next_btn = window_->AddWidget<Button>("Next", 40, 70, 80, 28);
    next_btn->SetColors({60, 80, 100, 220}, {100, 130, 160, 220}, {40, 50, 70, 220});
    next_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) spdlog::info("TutorialBtnDlg: next step");
    });

    auto* close_btn = window_->AddWidget<Button>("Close", 140, 70, 80, 28);
    close_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void TutorialBtnDlg::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

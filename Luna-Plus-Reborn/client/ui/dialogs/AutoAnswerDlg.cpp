#include "AutoAnswerDlg.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>

void AutoAnswerDlg::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/AutoAnswerDlg.bin.txt",
        "Auto Answer", 420, 260, 280, 160);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Auto Reply System", 30, 36, ColorPalette::TEXT_GOLD);

    auto* enable_btn = window_->AddWidget<Button>("Enable", 40, 70, 90, 28);
    enable_btn->SetColors({60, 100, 60, 220}, {100, 160, 100, 220}, {40, 60, 40, 220});
    enable_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            auto_reply_ = true;
            spdlog::info("AutoAnswerDlg: auto reply enabled");
        }
    });

    auto* disable_btn = window_->AddWidget<Button>("Disable", 150, 70, 90, 28);
    disable_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    disable_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            auto_reply_ = false;
            spdlog::info("AutoAnswerDlg: auto reply disabled");
        }
    });

    auto* close_btn = window_->AddWidget<Button>("Close", 100, 110, 80, 28);
    close_btn->SetColors({60, 60, 80, 220}, {100, 100, 130, 220}, {40, 40, 50, 220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void AutoAnswerDlg::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

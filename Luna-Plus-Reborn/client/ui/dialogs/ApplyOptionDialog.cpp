#include "ApplyOptionDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void ApplyOptionDialog::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/ApplyOptionDialog.bin.txt",
        "Apply Option", 400, 240, 300, 140);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Apply this option?", 40, 40, ColorPalette::TEXT_GOLD);

    auto* apply_btn = window_->AddWidget<Button>("Apply", 60, 80, 80, 28);
    apply_btn->SetColors({60, 100, 60, 220}, {100, 160, 100, 220}, {40, 60, 40, 220});
    apply_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) spdlog::info("ApplyOptionDialog: option applied");
    });

    auto* cancel_btn = window_->AddWidget<Button>("Cancel", 160, 80, 80, 28);
    cancel_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    cancel_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void ApplyOptionDialog::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

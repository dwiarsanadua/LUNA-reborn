#include "FarmAnimalCageDlg.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>

void FarmAnimalCageDlg::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/FarmAnimalCageDlg.bin.txt",
        "Animal Cage", 420, 260, 300, 200);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Farm Animal Cage", 50, 36, ColorPalette::TEXT_GOLD);
    window_->AddWidget<Label>("Animals: 0/10", 60, 65, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("Feed: Ready", 60, 90, ColorPalette::TEXT_NORMAL);

    auto* feed_btn = window_->AddWidget<Button>("Feed All", 40, 120, 90, 28);
    feed_btn->SetColors({60, 100, 60, 220}, {100, 160, 100, 220}, {40, 60, 40, 220});
    feed_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) spdlog::info("FarmAnimalCageDlg: feeding animals");
    });

    auto* close_btn = window_->AddWidget<Button>("Close", 160, 120, 80, 28);
    close_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void FarmAnimalCageDlg::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

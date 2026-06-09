#include "GuildRank.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>

void GuildRank::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/GuildRank.bin.txt",
        "Guild Ranking", 450, 200, 280, 240);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Guild Rankings", 50, 36, ColorPalette::TEXT_GOLD);
    window_->AddWidget<Label>("1. Alpha Knights - Lv.10", 30, 65, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("2. Moonlight - Lv.8", 30, 85, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("3. Star Guard - Lv.7", 30, 105, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("4. Your Guild - Lv.5", 30, 125, ColorPalette::TEXT_HIGHLIGHT);
    window_->AddWidget<Label>("5. Shadow Core - Lv.4", 30, 145, ColorPalette::TEXT_NORMAL);

    auto* close_btn = window_->AddWidget<Button>("Close", 90, 175, 90, 28);
    close_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void GuildRank::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

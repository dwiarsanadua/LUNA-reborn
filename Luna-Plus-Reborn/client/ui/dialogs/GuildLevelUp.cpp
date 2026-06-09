#include "GuildLevelUp.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void GuildLevelUp::Open(WindowManager* wm) {
    Open(wm, 1);
}

void GuildLevelUp::Open(WindowManager* wm, int new_level) {
    if (window_) return;
    new_level_ = new_level;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/GuildLevelUp.bin.txt",
        "Guild Level Up!", 440, 300, 280, 160);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    char buf[64];
    snprintf(buf, sizeof(buf), "Guild reached Level %d!", new_level);
    window_->AddWidget<Label>(buf, 30, 40, ColorPalette::TEXT_GOLD);
    window_->AddWidget<Label>("New perks unlocked!", 50, 65, ColorPalette::TEXT_HIGHLIGHT);

    auto* ok_btn = window_->AddWidget<Button>("OK", 100, 100, 80, 28);
    ok_btn->SetColors({60, 80, 100, 220}, {100, 130, 160, 220}, {40, 50, 70, 220});
    ok_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            spdlog::info("GuildLevelUp: acknowledged level {}", new_level_);
            Close();
        }
    });
}

void GuildLevelUp::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

#include "GameOutDialog.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>

GameOutDialog::GameOutDialog() {}

void GameOutDialog::Open(WindowManager* wm,
                         std::function<void(GameOutMode)> on_exit,
                         bool can_char_select) {
    if (window_) return;
    on_exit_ = std::move(on_exit);

    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/GameOut.bin.txt",
        "Exit Game", 300, 200, 360, 200);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    auto* msg_label = window_->AddWidget<Label>(
        "Are you sure you want to exit?", 20, 36, ColorPalette::TEXT_NORMAL);
    (void)msg_label;

    auto* confirm_btn = window_->AddWidget<Button>("Exit Game", 60, 80, 100, 28);
    confirm_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    confirm_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            spdlog::info("GameOut: exit game confirmed");
            Close();
            if (on_exit_) on_exit_(GameOutMode::GameExit);
        }
    });

    float cs_x = 200;
    if (!can_char_select) cs_x = 999;

    if (can_char_select) {
        auto* charsel_btn = window_->AddWidget<Button>("Character Select", cs_x, 80, 100, 28);
        charsel_btn->OnEvent([this](const UIEvent& e) {
            if (e.type == UIEvent::Click) {
                spdlog::info("GameOut: back to char select");
                Close();
                if (on_exit_) on_exit_(GameOutMode::CharSelect);
            }
        });
    }

    auto* cancel_btn = window_->AddWidget<Button>("Cancel", 60, 120, 100, 28);
    cancel_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            spdlog::info("GameOut: cancelled");
            Close();
        }
    });
}

void GameOutDialog::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

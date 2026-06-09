#include "ChatRoomOptionDlg.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>

void ChatRoomOptionDlg::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/ChatRoomOptionDlg.bin.txt",
        "Chat Room Options", 400, 240, 300, 180);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Chat Room Settings", 40, 36, ColorPalette::TEXT_GOLD);
    window_->AddWidget<Label>("Max Members: 10", 50, 65, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("Password: None", 50, 85, ColorPalette::TEXT_NORMAL);

    auto* close_btn = window_->AddWidget<Button>("Close", 110, 120, 80, 28);
    close_btn->SetColors({60, 60, 80, 220}, {100, 100, 130, 220}, {40, 40, 50, 220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void ChatRoomOptionDlg::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

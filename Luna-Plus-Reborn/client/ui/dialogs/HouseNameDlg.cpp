#include "HouseNameDlg.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>

void HouseNameDlg::Open(WindowManager* wm) {
    if (window_) return;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/HouseNameDlg.bin.txt",
        "Name Your House", 400, 260, 300, 160);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>("Enter house name:", 50, 36, ColorPalette::TEXT_GOLD);
    window_->AddWidget<Label>(house_name_, 60, 65, ColorPalette::TEXT_NORMAL);

    auto* confirm_btn = window_->AddWidget<Button>("Confirm", 50, 100, 90, 28);
    confirm_btn->SetColors({60, 100, 60, 220}, {100, 160, 100, 220}, {40, 60, 40, 220});
    confirm_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) {
            spdlog::info("HouseNameDlg: house named '{}'", house_name_);
        }
    });

    auto* cancel_btn = window_->AddWidget<Button>("Cancel", 160, 100, 90, 28);
    cancel_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    cancel_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void HouseNameDlg::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

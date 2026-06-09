#include "ItemPopupDlg.hpp"
#include <ui/ColorPalette.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/Button.hpp>
#include <spdlog/spdlog.h>

void ItemPopupDlg::Open(WindowManager* wm) {
    Open(wm, item_name_, description_);
}

void ItemPopupDlg::Open(WindowManager* wm, const std::string& item_name, const std::string& desc) {
    if (window_) return;
    item_name_ = item_name;
    description_ = desc;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/ItemPopupDlg.bin.txt",
        item_name_.c_str(), 400, 240, 300, 180);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    window_->AddWidget<Label>(item_name_, 40, 36, ColorPalette::TEXT_GOLD);
    window_->AddWidget<Label>(desc, 40, 65, ColorPalette::TEXT_NORMAL);
    window_->AddWidget<Label>("Rarity: Common", 50, 95, ColorPalette::TEXT_NORMAL);

    auto* close_btn = window_->AddWidget<Button>("Close", 110, 130, 80, 28);
    close_btn->SetColors({80, 40, 40, 220}, {130, 70, 70, 220}, {50, 25, 25, 220});
    close_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });
}

void ItemPopupDlg::Close() {
    if (window_) {
        window_->SetVisible(false);
        window_ = nullptr;
    }
}

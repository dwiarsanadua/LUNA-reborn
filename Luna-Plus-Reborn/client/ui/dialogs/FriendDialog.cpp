#include "FriendDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/InputField.hpp>
#include <ui/widgets/TabPanel.hpp>

void FriendDialog::Open(WindowManager* wm) {
    if (window_) return;

    // Try script layout first
    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/Friend.bin.txt");

    if (!window_) {
        window_ = new Window("FRIENDS", 400, 150, 300, 300);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    auto* tabs = window_->AddWidget<TabPanel>(10, 26, 280, 250);

    auto* online_list = new ListBox(0, 0, 260, 200);
    online_list->AddItem("-- Friends Online --");
    online_list->AddItem("   (No friends online)");
    tabs->AddTab("Online", online_list);

    auto* offline_list = new ListBox(0, 0, 260, 200);
    offline_list->AddItem("-- Offline --");
    offline_list->AddItem("   (No offline friends)");
    tabs->AddTab("Offline", offline_list);
}

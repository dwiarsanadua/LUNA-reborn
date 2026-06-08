#include "FriendDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void FriendDialog::Open(WindowManager* wm) {
    if (window_) return;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/Friend.bin.txt");

    if (!window_) {
        spdlog::warn("FriendDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("FRIENDS", 400, 150, 300, 300);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    auto* tabs = window_->AddWidget<TabPanel>(10, 26, 280, 250);

    online_list_ = new ListBox(0, 0, 260, 200);
    online_list_->AddItem("(No friends online)");
    tabs->AddTab("Online", online_list_);

    offline_list_ = new ListBox(0, 0, 260, 200);
    offline_list_->AddItem("(No offline friends)");
    tabs->AddTab("Offline", offline_list_);
}

void FriendDialog::UpdateFromState(GameState* state) {
    if (!state || !online_list_ || !offline_list_) return;
    online_list_->Clear();
    offline_list_->Clear();
    if (state->friends.empty()) {
        online_list_->AddItem("(No friends — /friend add [name])");
        offline_list_->AddItem("(No offline friends)");
        return;
    }
    for (const auto& f : state->friends) {
        char line[128];
        snprintf(line, sizeof(line), "%s  Lv%d  Map %u", f.name.c_str(), f.level, f.map_id);
        if (f.online)
            online_list_->AddItem(line);
        else
            offline_list_->AddItem(line);
    }
    if (online_list_->GetCount() == 0)
        online_list_->AddItem("(No friends online)");
    if (offline_list_->GetCount() == 0)
        offline_list_->AddItem("(No offline friends)");
}

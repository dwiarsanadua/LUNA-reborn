#include "PartyDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <ui/ColorPalette.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void PartyDialog::Open(WindowManager* wm) {
    if (window_) return;

    // Try script layout first
    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/PartySet.bin.txt");

    if (!window_) {
        spdlog::warn("PartyDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("PARTY", 200, 150, 300, 280);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    auto* tabs = window_->AddWidget<TabPanel>(10, 30, 280, 200);

    auto* member_list = new ListBox(0, 0, 260, 180);
    member_list->AddItem("-- Party Members --");
    member_list->AddItem("   (No party formed yet)");
    member_list->AddItem("   Press 1-6 to invite");
    tabs->AddTab("Members", member_list);

    auto* info_list = new ListBox(0, 0, 260, 180);
    info_list->AddItem("XP Share: Equal Distribution");
    info_list->AddItem("Loot Rule: Free For All");
    info_list->AddItem("Max Members: 6");
    tabs->AddTab("Settings", info_list);

    window_->AddWidget<Label>("[1-6] Invite  [0] Leave Party", 10, 245, ColorPalette::TEXT_DISABLED);
}

#include "PartyDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <ui/ColorPalette.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void PartyDialog::Open(WindowManager* wm) {
    if (window_) return;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/PartySet.bin.txt");

    if (!window_) {
        spdlog::warn("PartyDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("PARTY", 200, 150, 300, 280);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    auto* tabs = window_->AddWidget<TabPanel>(10, 30, 280, 200);

    member_list_ = new ListBox(0, 0, 260, 180);
    member_list_->AddItem("-- Party Members --");
    tabs->AddTab("Members", member_list_);

    auto* info_list = new ListBox(0, 0, 260, 180);
    info_list->AddItem("XP Share: Equal Distribution");
    info_list->AddItem("Loot Rule: Free For All");
    info_list->AddItem("Max Members: 6");
    info_list->AddItem("/party invite [name]");
    info_list->AddItem("/party leave");
    tabs->AddTab("Settings", info_list);

    window_->AddWidget<Label>("[P] Toggle  /party invite", 10, 245, ColorPalette::TEXT_DISABLED);
}

void PartyDialog::UpdateFromState(GameState* state) {
    if (!member_list_ || !state) return;
    member_list_->Clear();
    if (state->party_members.empty()) {
        member_list_->AddItem("(No party — open online to auto-create)");
        return;
    }
    char header[64];
    snprintf(header, sizeof(header), "Party #%u (%zu/6)", state->party_id, state->party_members.size());
    member_list_->AddItem(header);
    for (const auto& m : state->party_members) {
        char line[128];
        snprintf(line, sizeof(line), "%s%s Lv%d  HP %d/%d  Map %u",
            m.is_leader ? "* " : "  ", m.name.c_str(), m.level, m.hp, m.max_hp, m.map_id);
        member_list_->AddItem(line);
    }
}

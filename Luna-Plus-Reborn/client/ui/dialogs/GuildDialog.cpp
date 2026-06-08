#include "GuildDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void GuildDialog::Open(WindowManager* wm, SiegeSystem* siege) {
    if (window_) return;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/Guild.bin.txt");

    if (!window_) {
        spdlog::warn("GuildDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("GUILD", 300, 100, 380, 380);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    auto* tabs = window_->AddWidget<TabPanel>(10, 30, 360, 310);

    auto* member_list = new ListBox(0, 0, 340, 290);
    member_list->AddItem("-- Guild Roster --");
    member_list->AddItem("   (Not in a guild)");
    tabs->AddTab("Members", member_list);

    auto* info_list = new ListBox(0, 0, 340, 290);
    info_list->AddItem("Guild Level: 1");
    info_list->AddItem("Members: 0/30");
    info_list->AddItem("GP: 0");
    info_list->AddItem(" ");
    info_list->AddItem("To create a guild, visit the");
    info_list->AddItem("Guild Manager NPC in town.");
    tabs->AddTab("Info", info_list);

    auto* siege_list = new ListBox(0, 0, 340, 290);
    PopulateSiegeTab(siege_list, siege);
    tabs->AddTab("Siege/War", siege_list);
}

void GuildDialog::PopulateSiegeTab(ListBox* lb, SiegeSystem* siege) {
    if (!siege) {
        lb->AddItem("Siege system unavailable");
        return;
    }
    lb->AddItem("=== Territories ===");
    auto territories = siege->GetAllTerritories();
    if (territories.empty()) {
        lb->AddItem("  No territories");
    } else {
        for (auto& t : territories) {
            char buf[128];
            snprintf(buf, sizeof(buf), "  %s | Owner: %s | Tax: %d%%",
                t.name.c_str(),
                t.owner_guild_name.empty() ? "None" : t.owner_guild_name.c_str(),
                t.tax_rate);
            lb->AddItem(buf);
        }
    }
    lb->AddItem(" ");
    lb->AddItem("=== Upcoming Sieges ===");
    auto sieges = siege->GetUpcomingSieges();
    if (sieges.empty()) {
        lb->AddItem("  No scheduled sieges");
    } else {
        for (auto& s : sieges) {
            char buf[128];
            snprintf(buf, sizeof(buf), "  %s vs %s (Territory %d)",
                s.attacker_name.c_str(), s.defender_name.c_str(), s.territory_id);
            lb->AddItem(buf);
        }
    }
    lb->AddItem(" ");
    lb->AddItem("=== Wars ===");
    lb->AddItem("  Declare war via Guild Manager NPC");
}

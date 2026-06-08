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

    member_list_ = new ListBox(0, 0, 340, 290);
    member_list_->AddItem("(Not in a guild)");
    tabs->AddTab("Members", member_list_);

    info_list_ = new ListBox(0, 0, 340, 290);
    info_list_->AddItem("Use /guild create [name]");
    tabs->AddTab("Info", info_list_);

    auto* siege_list = new ListBox(0, 0, 340, 290);
    PopulateSiegeTab(siege_list, siege);
    tabs->AddTab("Siege/War", siege_list);
}

void GuildDialog::UpdateFromState(GameState* state) {
    if (!state || !member_list_ || !info_list_) return;
    member_list_->Clear();
    info_list_->Clear();
    if (state->guild_id == 0 || state->guild_members.empty()) {
        member_list_->AddItem("(Not in a guild)");
        info_list_->AddItem("/guild create [name]");
        info_list_->AddItem("/guild invite [name]");
        return;
    }
    char header[96];
    snprintf(header, sizeof(header), "[%s] Lv%u  GP %u",
        state->guild_name.c_str(), state->guild_level, state->guild_gp);
    info_list_->AddItem(header);
    snprintf(header, sizeof(header), "Members: %zu/30", state->guild_members.size());
    info_list_->AddItem(header);
    info_list_->AddItem("/guild invite [name]");
    info_list_->AddItem("/guild leave");
    for (const auto& m : state->guild_members) {
        const char* rank = m.rank >= 2 ? "Master" : (m.rank >= 1 ? "Officer" : "Member");
        char line[128];
        snprintf(line, sizeof(line), "%s%s  Lv%d  %s",
            m.online ? "" : "(off) ", m.name.c_str(), m.level, rank);
        member_list_->AddItem(line);
    }
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

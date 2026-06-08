#include "GuildDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/ListBox.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <cstdlib>

void GuildDialog::SetSiegeNetworkCallbacks(
    std::function<void(uint8_t, uint32_t, uint32_t)> action_fn,
    std::function<void()> refresh_fn) {
    siege_action_fn_ = std::move(action_fn);
    siege_refresh_fn_ = std::move(refresh_fn);
}

uint32_t GuildDialog::SelectedTerritoryId() const {
    if (!territory_input_ || territory_input_->GetText().empty()) return 1;
    return static_cast<uint32_t>(std::atoi(territory_input_->GetText().c_str()));
}

void GuildDialog::Open(WindowManager* wm, SiegeSystem* siege) {
    siege_ = siege;
    if (window_) return;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/Guild.bin.txt");

    if (!window_) {
        spdlog::warn("GuildDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("GUILD", 280, 80, 420, 460);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    auto* tabs = window_->AddWidget<TabPanel>(10, 30, 400, 300);

    member_list_ = new ListBox(0, 0, 380, 270);
    member_list_->AddItem("(Not in a guild)");
    tabs->AddTab("Members", member_list_);

    info_list_ = new ListBox(0, 0, 380, 270);
    info_list_->AddItem("Use /guild create [name]");
    tabs->AddTab("Info", info_list_);

    siege_list_ = new ListBox(0, 0, 380, 220);
    siege_list_->AddItem("Loading siege data...");
    tabs->AddTab("Siege/War", siege_list_);

    window_->AddWidget<Label>("Territory ID:", 10, 340, 0xffcccccc);
    territory_input_ = window_->AddWidget<InputField>(90, 338, 50, 22);
    territory_input_->SetPlaceholder("1");
    territory_input_->SetValidation(InputValidation::PositiveInteger);

    window_->AddWidget<Label>("Tax %:", 150, 340, 0xffcccccc);
    tax_input_ = window_->AddWidget<InputField>(195, 338, 40, 22);
    tax_input_->SetPlaceholder("10");
    tax_input_->SetValidation(InputValidation::PositiveInteger);
    tax_input_->SetNumericRange(0, 30);

    auto* declare_btn = window_->AddWidget<Button>("Declare Siege", 10, 368, 95, 24);
    declare_btn->SetColors({80,40,40,220}, {130,70,70,220}, {50,25,25,220});
    declare_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && siege_action_fn_)
            siege_action_fn_(0, SelectedTerritoryId(), 0);
    });

    auto* tax_btn = window_->AddWidget<Button>("Set Tax", 112, 368, 70, 24);
    tax_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && siege_action_fn_) {
            uint32_t rate = tax_input_ && !tax_input_->GetText().empty()
                ? static_cast<uint32_t>(std::atoi(tax_input_->GetText().c_str())) : 10;
            siege_action_fn_(1, SelectedTerritoryId(), rate);
        }
    });

    auto* collect_btn = window_->AddWidget<Button>("Collect Tax", 190, 368, 90, 24);
    collect_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && siege_action_fn_)
            siege_action_fn_(2, SelectedTerritoryId(), 0);
    });

    auto* cancel_btn = window_->AddWidget<Button>("Cancel Siege", 286, 368, 95, 24);
    cancel_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && siege_action_fn_)
            siege_action_fn_(3, SelectedTerritoryId(), 0);
    });

    auto* refresh_btn = window_->AddWidget<Button>("Refresh", 10, 400, 80, 24);
    refresh_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && siege_refresh_fn_) siege_refresh_fn_();
    });
}

void GuildDialog::UpdateFromState(GameState* state) {
    if (!state || !member_list_ || !info_list_) return;
    member_list_->Clear();
    info_list_->Clear();
    if (state->guild_id == 0 || state->guild_members.empty()) {
        member_list_->AddItem("(Not in a guild)");
        info_list_->AddItem("/guild create [name]");
        info_list_->AddItem("/guild invite [name]");
    } else {
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
    PopulateSiegeTab(state);
}

void GuildDialog::PopulateSiegeTabOffline(SiegeSystem* siege) {
    if (!siege_list_ || !siege) return;
    siege_list_->Clear();
    siege_list_->AddItem("=== Territories (offline) ===");
    for (auto& t : siege->GetAllTerritories()) {
        char buf[160];
        snprintf(buf, sizeof(buf), " #%d %s%s | %s | Tax %d%% | Stored %llug",
            t.id, t.is_castle ? "[Castle] " : "",
            t.name.c_str(),
            t.owner_guild_name.empty() ? "Unclaimed" : t.owner_guild_name.c_str(),
            t.tax_rate, static_cast<unsigned long long>(t.total_tax_collected));
        siege_list_->AddItem(buf);
    }
    siege_list_->AddItem(" ");
    siege_list_->AddItem("=== Upcoming Sieges ===");
    auto sieges = siege->GetUpcomingSieges();
    if (sieges.empty()) {
        siege_list_->AddItem("  None scheduled");
    } else {
        for (auto& s : sieges) {
            char buf[128];
            snprintf(buf, sizeof(buf), "  %s vs %s (T#%d)",
                s.attacker_name.c_str(), s.defender_name.c_str(), s.territory_id);
            siege_list_->AddItem(buf);
        }
    }
}

void GuildDialog::PopulateSiegeTab(GameState* state) {
    if (!siege_list_) return;
    if (!state || state->offline_mode || state->network_siege_territories.empty()) {
        PopulateSiegeTabOffline(siege_);
        return;
    }

    siege_list_->Clear();
    char header[128];
    snprintf(header, sizeof(header), "Guild: %s (ID %u)",
        state->network_siege_player_guild_name.empty()
            ? "None" : state->network_siege_player_guild_name.c_str(),
        state->network_siege_player_guild_id);
    siege_list_->AddItem(header);
    if (state->network_can_declare_siege)
        siege_list_->AddItem("  You can declare siege (5000g, 1h timer)");
    if (state->network_can_set_tax)
        siege_list_->AddItem("  You can manage tax on owned territories");
    siege_list_->AddItem(" ");

    siege_list_->AddItem("=== Territories ===");
    for (const auto& t : state->network_siege_territories) {
        char buf[200];
        const char* owner = t.owner_guild_name.empty() ? "Unclaimed" : t.owner_guild_name.c_str();
        snprintf(buf, sizeof(buf),
            " #%u %s%s | %s | Tax %u%% | Bank %ug%s%s",
            t.territory_id, t.is_castle ? "[Castle] " : "", t.name.c_str(), owner,
            t.tax_rate, t.tax_accumulated,
            t.can_attack ? " | CAN ATTACK" : "",
            t.owned_by_player_guild ? " | YOURS" : "");
        siege_list_->AddItem(buf);
        if (t.seconds_until_siege > 0 && t.attacker_guild_id) {
            char sbuf[128];
            snprintf(sbuf, sizeof(sbuf), "    Siege: %s in %u min",
                t.attacker_guild_name.c_str(), t.seconds_until_siege / 60);
            siege_list_->AddItem(sbuf);
        }
    }

    siege_list_->AddItem(" ");
    siege_list_->AddItem("=== Scheduled Sieges ===");
    if (state->network_siege_schedules.empty()) {
        siege_list_->AddItem("  None");
    } else {
        for (const auto& s : state->network_siege_schedules) {
            char buf[160];
            snprintf(buf, sizeof(buf), "  %s vs %s @ %s (%u min)",
                s.attacker_guild_name.c_str(),
                s.defender_guild_name.empty() ? "Unclaimed" : s.defender_guild_name.c_str(),
                s.territory_name.c_str(), s.seconds_until / 60);
            siege_list_->AddItem(buf);
        }
    }
}

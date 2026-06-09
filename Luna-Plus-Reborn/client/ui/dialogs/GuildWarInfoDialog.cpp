#include "GuildWarInfoDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <ui/ColorPalette.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void GuildWarInfoDialog::Open(WindowManager* wm) {
    if (window_) return;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/GuildWarInfo.bin.txt");

    if (!window_) {
        spdlog::warn("GuildWarInfoDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("Guild War Info", 300, 100, 350, 320);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    auto* tabs = window_->AddWidget<TabPanel>(10, 30, 330, 240);

    enemy_list_ = new ListBox(0, 0, 310, 220);
    enemy_list_->AddItem("(No enemy guilds)");
    tabs->AddTab("Enemies", enemy_list_);

    ally_list_ = new ListBox(0, 0, 310, 220);
    ally_list_->AddItem("(No allied guilds)");
    tabs->AddTab("Allies", ally_list_);

    war_record_ = window_->AddWidget<Label>("War Record: 0W 0D 0L", 10, 280, ColorPalette::TEXT_NORMAL);
}

void GuildWarInfoDialog::SetWarData(const std::vector<std::string>& enemies,
                                     const std::vector<std::string>& allies,
                                     int wins, int draws, int losses) {
    if (enemy_list_) {
        enemy_list_->Clear();
        if (enemies.empty()) {
            enemy_list_->AddItem("(No enemy guilds)");
        } else {
            for (const auto& e : enemies)
                enemy_list_->AddItem(e);
        }
    }
    if (ally_list_) {
        ally_list_->Clear();
        if (allies.empty()) {
            ally_list_->AddItem("(No allied guilds)");
        } else {
            for (const auto& a : allies)
                ally_list_->AddItem(a);
        }
    }
    if (war_record_) {
        char buf[64];
        snprintf(buf, sizeof(buf), "War Record: %dW %dD %dL", wins, draws, losses);
        war_record_->SetText(buf);
    }
}

void GuildWarInfoDialog::UpdateFromState(GameState* state) {
    if (!window_ || !state) return;
    std::vector<std::string> enemies;
    std::vector<std::string> allies;
    int wins = 0, draws = 0, losses = 0;

    if (!state->guild_name.empty()) {
        for (const auto& m : state->guild_members) {
            if (m.online) {
                char line[64];
                snprintf(line, sizeof(line), "%s (Lv.%d)", m.name.c_str(), m.level);
                allies.push_back(line);
            }
        }
    }

    if (enemies.empty()) enemies.push_back("(Use /guild war declare [name])");

    SetWarData(enemies, allies, wins, draws, losses);
}

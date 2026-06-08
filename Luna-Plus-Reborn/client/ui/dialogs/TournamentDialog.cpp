#include "TournamentDialog.hpp"
#include <cstdio>
#include <ctime>

void TournamentDialog::Open(GameState* state, WindowManager* wm, TournamentSystem* ts) {
    ts_ = ts;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/GTBattleTable_16.bin.txt",
        "Tournament Arena", 120, 40, 540, 460);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    auto* title = window_->AddWidget<Label>("Arena Tournament", 10, 4, 0xffffcc88);
    (void)title;

    tabs_ = window_->AddWidget<TabPanel>(10, 28, 520, 380);

    // Tab 0: Active tournaments
    info_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);

    // Tab 1: Bracket
    bracket_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);

    tabs_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::TabSelected) {
            info_label_->SetVisible(e.int_value == 0);
            bracket_label_->SetVisible(e.int_value == 1);
            if (e.int_value == 0) Refresh();
        }
    });
    bracket_label_->SetVisible(false);

    // Action buttons
    auto* join_btn = window_->AddWidget<Button>("Join Tournament", 14, 420, 140, 24);
    join_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    join_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click && ts_) {
            auto tournaments = ts_->GetActiveTournaments();
            if (!tournaments.empty()) {
                auto& t = tournaments[0];
                std::vector<uint32_t> members = {(uint32_t)state->selected_char};
                ts_->RegisterTeam(t.id, 1, state->name, members);
                state->chat_messages.push_back("Registered for tournament!");
                Refresh();
            } else {
                state->chat_messages.push_back("No active tournaments right now.");
            }
        }
    });

    auto* start_btn = window_->AddWidget<Button>("Start Tournament", 170, 420, 150, 24);
    start_btn->SetColors({80,40,80,220}, {130,80,130,220}, {50,30,50,220});
    start_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click && ts_) {
            auto tournaments = ts_->GetActiveTournaments();
            if (!tournaments.empty()) {
                ts_->StartTournament(tournaments[0].id);
                state->chat_messages.push_back("Tournament started! Good luck!");
                Refresh();
            }
        }
    });

    // Create initial tournament for demo
    if (ts_) {
        ts_->CreateTournament("Saturday Showdown", 8, 5000);
    }

    Refresh();
}

void TournamentDialog::Refresh() {
    if (!ts_ || !info_label_) return;

    auto tournaments = ts_->GetActiveTournaments();
    if (tournaments.empty()) {
        info_label_->SetText("No active tournaments.\nCreate one to start battling!");
        bracket_label_->SetText("No bracket data.");
        return;
    }

    char buf[1024];
    std::string text;
    for (auto& t : tournaments) {
        text += "=== " + t.name + " ===\n";
        text += "State: ";
        switch (t.state) {
        case TournamentState::Registration: text += "Registration"; break;
        case TournamentState::InProgress: text += "In Progress"; break;
        case TournamentState::Completed: text += "Completed"; break;
        default: text += "Unknown"; break;
        }
        text += "\nTeams: " + std::to_string(t.teams.size()) + "/" + std::to_string(t.max_teams);
        text += "\nPrize: " + std::to_string(t.prize_gold) + " gold";
        text += "\nRound: " + std::to_string(t.current_round);
        text += "\nMin Team Size: " + std::to_string(t.min_team_size);
        text += "\nMax Team Size: " + std::to_string(t.max_team_size);
        text += "\nRegistration ends: ";
        char time_buf[32];
        struct tm* tm_info = localtime(&t.registration_end);
        if (tm_info) {
            strftime(time_buf, sizeof(time_buf), "%m/%d %H:%M", tm_info);
            text += time_buf;
        } else {
            text += "TBD";
        }
        text += "\n\n";
        
        // Teams
        if (!t.teams.empty()) {
            text += "--- Registered Teams ---\n";
            for (auto& team : t.teams) {
                text += "  " + team.guild_name + " (" + std::to_string(team.member_ids.size()) + " members)\n";
            }
        }
    }
    snprintf(buf, sizeof(buf), "%s", text.c_str());
    info_label_->SetText(buf);

    // Bracket view
    if (!tournaments.empty()) {
        auto& t = tournaments[0];
        std::string bracket;
        bracket += "=== Bracket ===\n";
        for (auto& match : t.matches) {
            bracket += "Round " + std::to_string(match.round) + ": ";
            bracket += "Team " + std::to_string(match.team1_id) + " vs Team " + std::to_string(match.team2_id);
            if (match.completed) bracket += " [Done]";
            bracket += "\n";
        }
        if (bracket.empty()) bracket = "Bracket not yet generated.";
        snprintf(buf, sizeof(buf), "%s", bracket.c_str());
        bracket_label_->SetText(buf);
    }
}

void TournamentDialog::UpdateFromState(GameState* state) {
    (void)state;
}

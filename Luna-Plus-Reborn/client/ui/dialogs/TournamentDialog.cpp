#include "TournamentDialog.hpp"
#include <cstdio>
#include <cstdlib>
#include <ctime>

void TournamentDialog::SetNetworkCallbacks(std::function<void(uint32_t)> register_fn,
                                           std::function<void(uint32_t)> unregister_fn,
                                           std::function<void(uint32_t)> claim_fn,
                                           std::function<void()> refresh_fn) {
    register_fn_ = std::move(register_fn);
    unregister_fn_ = std::move(unregister_fn);
    claim_fn_ = std::move(claim_fn);
    refresh_fn_ = std::move(refresh_fn);
}

const char* TournamentDialog::StateLabel(uint8_t state) {
    switch (state) {
    case 1: return "In Progress";
    case 2: return "Completed";
    case 3: return "Cancelled";
    default: return "Registration";
    }
}

uint32_t TournamentDialog::SelectedTournamentId(GameState* state) const {
    if (tournament_id_input_ && !tournament_id_input_->GetText().empty())
        return static_cast<uint32_t>(std::atoi(tournament_id_input_->GetText().c_str()));
    if (state && !state->network_tournaments.empty())
        return state->network_tournaments[0].tournament_id;
    return 1;
}

void TournamentDialog::Open(GameState* state, WindowManager* wm, TournamentSystem* ts) {
    ts_ = ts;
    window_ = wm->LoadFromScriptOrOpen("assets/interface/Windows/GTBattleTable_16.bin.txt",
        "Tournament Arena", 120, 40, 540, 480);
    window_->SetClosable(true);
    window_->SetMovable(true);
    window_->SetTitleBarH(24);

    auto* title = window_->AddWidget<Label>("Arena Tournament", 10, 4, 0xffffcc88);
    (void)title;

    tabs_ = window_->AddWidget<TabPanel>(10, 28, 520, 360);
    info_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);
    bracket_label_ = window_->AddWidget<Label>("", 14, 56, 0xffffffff);

    tabs_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::TabSelected) {
            info_label_->SetVisible(e.int_value == 0);
            bracket_label_->SetVisible(e.int_value == 1);
        }
    });
    bracket_label_->SetVisible(false);

    window_->AddWidget<Label>("Tournament ID:", 14, 400, 0xffcccccc);
    tournament_id_input_ = window_->AddWidget<InputField>(100, 398, 50, 22);
    tournament_id_input_->SetPlaceholder("1");
    tournament_id_input_->SetValidation(InputValidation::PositiveInteger);

    auto* join_btn = window_->AddWidget<Button>("Register (500g)", 160, 398, 120, 24);
    join_btn->SetColors({40,80,40,220}, {80,130,80,220}, {30,50,30,220});
    join_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type != UIEvent::Click) return;
        const uint32_t tid = SelectedTournamentId(state);
        if (register_fn_) register_fn_(tid);
        else if (ts_) {
            std::vector<uint32_t> members = {static_cast<uint32_t>(state->selected_char)};
            ts_->RegisterTeam(tid, state->guild_id ? state->guild_id : 1,
                state->guild_name.empty() ? state->name : state->guild_name, members);
            state->chat_messages.push_back("Registered (offline)");
            Refresh(state);
        }
    });

    auto* leave_btn = window_->AddWidget<Button>("Unregister", 290, 398, 90, 24);
    leave_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click && unregister_fn_)
            unregister_fn_(SelectedTournamentId(state));
    });

    auto* claim_btn = window_->AddWidget<Button>("Claim Prize", 390, 398, 90, 24);
    claim_btn->SetColors({80,70,20,220}, {130,110,40,220}, {50,40,10,220});
    claim_btn->OnEvent([this, state](const UIEvent& e) {
        if (e.type == UIEvent::Click && claim_fn_)
            claim_fn_(SelectedTournamentId(state));
    });

    auto* refresh_btn = window_->AddWidget<Button>("Refresh", 14, 430, 80, 24);
    refresh_btn->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click && refresh_fn_) refresh_fn_();
    });

    if (ts_ && state && state->offline_mode)
        ts_->CreateTournament("Saturday Showdown", 8, 5000);

    Refresh(state);
}

void TournamentDialog::Refresh(GameState* state) {
    if (!info_label_) return;

    const bool use_network = state && !state->offline_mode && !state->network_tournaments.empty();
    char buf[1200];

    if (use_network) {
        std::string text;
        for (const auto& t : state->network_tournaments) {
            text += "=== " + t.name + " ===\n";
            text += "State: " + std::string(StateLabel(t.state)) + "\n";
            text += "Teams: " + std::to_string(t.registered) + "/" + std::to_string(t.max_teams);
            text += "\nPrize: " + std::to_string(t.prize_gold) + "g";
            text += "\nRound: " + std::to_string(t.current_round);
            if (t.seconds_until_start > 0)
                text += "\nStarts in: " + std::to_string(t.seconds_until_start / 60) + " min";
            if (t.winner_guild_id)
                text += "\nWinner guild ID: " + std::to_string(t.winner_guild_id);
            if (t.player_registered) text += "\n[You are registered]";
            if (t.can_register) text += "\n[Can register - 500g]";
            if (t.can_claim_prize) text += "\n[Prize ready to claim!]";
            text += "\n\n";
        }
        if (!state->network_tournament_teams.empty()) {
            text += "--- Registered Teams ---\n";
            for (const auto& team : state->network_tournament_teams) {
                text += "  T" + std::to_string(team.tournament_id) + ": "
                    + team.guild_name + " (seed " + std::to_string(team.seed) + ")";
                if (team.eliminated) text += " [OUT]";
                text += "\n";
            }
        }
        snprintf(buf, sizeof(buf), "%s", text.c_str());
        info_label_->SetText(buf);

        std::string bracket = "=== Bracket ===\n";
        if (state->network_tournament_matches.empty()) {
            bracket += "Bracket not generated yet.";
        } else {
            for (const auto& m : state->network_tournament_matches) {
                char line[128];
                snprintf(line, sizeof(line), "T%u R%u #%u: %u vs %u",
                    m.tournament_id, m.round, m.match_index,
                    m.team1_guild_id, m.team2_guild_id);
                bracket += line;
                if (m.completed)
                    bracket += " -> winner " + std::to_string(m.winner_guild_id);
                bracket += "\n";
            }
        }
        bracket_label_->SetText(bracket);
        if (refresh_fn_) refresh_fn_();
        return;
    }

    if (!ts_) return;
    auto tournaments = ts_->GetActiveTournaments();
    if (tournaments.empty()) {
        info_label_->SetText("No active tournaments.\nCreate one to start battling!");
        bracket_label_->SetText("No bracket data.");
        return;
    }

    std::string text;
    for (auto& t : tournaments) {
        text += "=== " + t.name + " ===\n";
        switch (t.state) {
        case TournamentState::Registration: text += "State: Registration\n"; break;
        case TournamentState::InProgress: text += "State: In Progress\n"; break;
        case TournamentState::Completed: text += "State: Completed\n"; break;
        default: text += "State: Unknown\n"; break;
        }
        text += "Teams: " + std::to_string(t.teams.size()) + "/" + std::to_string(t.max_teams);
        text += "\nPrize: " + std::to_string(t.prize_gold) + " gold\n\n";
    }
    snprintf(buf, sizeof(buf), "%s", text.c_str());
    info_label_->SetText(buf);
    bracket_label_->SetText("Offline bracket view.");
}

void TournamentDialog::UpdateFromState(GameState* state) {
    if (window_) Refresh(state);
}

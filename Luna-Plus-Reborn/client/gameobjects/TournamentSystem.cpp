#include "TournamentSystem.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <random>
#include <spdlog/spdlog.h>

void TournamentSystem::Init() {
    tournaments_.clear();
    spdlog::info("TournamentSystem: initialized");
}

void TournamentSystem::Update(float dt) {
    tick_timer_ += dt;
    if (tick_timer_ < 30.0f) return;
    tick_timer_ = 0;
    
    for (auto& t : tournaments_) {
        if (t.state == TournamentState::Registration && t.registration_end > 0) {
            if (time(nullptr) >= t.registration_end && t.teams.size() >= 2) {
                StartTournament(t.id);
            }
        }
    }
}

uint32_t TournamentSystem::CreateTournament(const std::string& name, int max_teams, uint32_t prize_gold) {
    Tournament t;
    t.id = next_id_++;
    t.name = name;
    t.max_teams = max_teams;
    t.state = TournamentState::Registration;
    t.registration_end = time(nullptr) + 86400; // 24h registration
    t.prize_gold = prize_gold;
    tournaments_.push_back(t);
    spdlog::info("Tournament: created '{}' ({} teams, {}g prize)", name, max_teams, prize_gold);
    return t.id;
}

bool TournamentSystem::RegisterTeam(uint32_t tournament_id, uint32_t guild_id, const std::string& guild_name,
                                     const std::vector<uint32_t>& members) {
    auto* t = GetTournament(tournament_id);
    if (!t || t->state != TournamentState::Registration) return false;
    if ((int)t->teams.size() >= t->max_teams) return false;
    
    TournamentTeam team;
    team.guild_id = guild_id;
    team.guild_name = guild_name;
    team.member_ids = members;
    team.team_leader_id = members.empty() ? 0 : members[0];
    t->teams.push_back(team);
    spdlog::info("Tournament: {} registered for '{}'", guild_name, t->name);
    return true;
}

bool TournamentSystem::UnregisterTeam(uint32_t tournament_id, uint32_t guild_id) {
    auto* t = GetTournament(tournament_id);
    if (!t || t->state != TournamentState::Registration) return false;
    t->teams.erase(std::remove_if(t->teams.begin(), t->teams.end(),
        [&](auto& tm) { return tm.guild_id == guild_id; }), t->teams.end());
    return true;
}

bool TournamentSystem::StartTournament(uint32_t tournament_id) {
    auto* t = GetTournament(tournament_id);
    if (!t || t->state != TournamentState::Registration) return false;
    if (t->teams.size() < 2) return false;
    
    t->state = TournamentState::InProgress;
    GenerateBracket(*t);
    spdlog::info("Tournament: '{}' started with {} teams", t->name, t->teams.size());
    return true;
}

void TournamentSystem::GenerateBracket(Tournament& t) {
    // Shuffle teams for random seeding
    std::shuffle(t.teams.begin(), t.teams.end(), std::mt19937{std::random_device{}()});
    for (size_t i = 0; i < t.teams.size(); i++) t.teams[i].seed = (int)i + 1;
    
    // Calculate bracket size (power of 2)
    int bracket_size = 1;
    while (bracket_size < (int)t.teams.size()) bracket_size *= 2;
    
    // Create first round matches
    int num_matches = bracket_size / 2;
    for (int i = 0; i < num_matches; i++) {
        TournamentMatch m;
        m.round = 1;
        m.match_index = i;
        m.team1_id = (i * 2 < (int)t.teams.size()) ? t.teams[i * 2].guild_id : 0;
        m.team2_id = (i * 2 + 1 < (int)t.teams.size()) ? t.teams[i * 2 + 1].guild_id : 0;
        // Bye: if no opponent, auto-advance
        if (m.team2_id == 0) { m.winner_id = m.team1_id; m.completed = true; }
        if (m.team1_id == 0) { m.winner_id = m.team2_id; m.completed = true; }
        t.matches.push_back(m);
    }
    t.current_round = 1;
}

bool TournamentSystem::AdvanceMatch(uint32_t tournament_id, uint32_t winner_id) {
    auto* t = GetTournament(tournament_id);
    if (!t || t->state != TournamentState::InProgress) return false;
    
    // Find incomplete match in current round
    for (auto& m : t->matches) {
        if (m.round == t->current_round && !m.completed) {
            if (m.team1_id == winner_id || m.team2_id == winner_id) {
                m.winner_id = winner_id;
                m.completed = true;
                
                // Check if round is complete
                bool round_complete = true;
                int wins_needed = 0;
                for (auto& m2 : t->matches) {
                    if (m2.round == t->current_round) {
                        if (!m2.completed) round_complete = false;
                        wins_needed++;
                    }
                }
                
                if (round_complete) {
                    int next_round = t->current_round + 1;
                    int next_matches = wins_needed / 2;
                    
                    if (next_matches == 0) {
                        // Tournament over
                        CompleteTournament(tournament_id);
                        return true;
                    }
                    
                    // Create next round
                    std::vector<uint32_t> winners;
                    for (auto& m2 : t->matches) {
                        if (m2.round == t->current_round) winners.push_back(m2.winner_id);
                    }
                    for (int i = 0; i < next_matches; i++) {
                        TournamentMatch nm;
                        nm.round = next_round;
                        nm.match_index = i;
                        nm.team1_id = winners[i * 2];
                        nm.team2_id = (i * 2 + 1 < (int)winners.size()) ? winners[i * 2 + 1] : 0;
                        if (nm.team2_id == 0) { nm.winner_id = nm.team1_id; nm.completed = true; }
                        t->matches.push_back(nm);
                    }
                    t->current_round = next_round;
                }
                return true;
            }
        }
    }
    return false;
}

void TournamentSystem::CompleteTournament(uint32_t tournament_id) {
    auto* t = GetTournament(tournament_id);
    if (!t) return;
    t->state = TournamentState::Completed;
    t->completed = true;
    
    // Find winner and runner-up
    if (!t->matches.empty()) {
        auto& final_match = t->matches.back();
        for (auto& team : t->teams) {
            if (team.guild_id == final_match.winner_id) team.placement = 1;
        }
        if (t->on_complete) t->on_complete(final_match.winner_id, 0);
    }
    spdlog::info("Tournament: '{}' completed", t->name);
}

Tournament* TournamentSystem::GetTournament(uint32_t tournament_id) {
    for (auto& t : tournaments_) if (t.id == tournament_id) return &t;
    return nullptr;
}

std::vector<Tournament> TournamentSystem::GetActiveTournaments() const {
    std::vector<Tournament> result;
    for (auto& t : tournaments_) {
        if (t.state == TournamentState::Registration || t.state == TournamentState::InProgress)
            result.push_back(t);
    }
    return result;
}

std::vector<Tournament> TournamentSystem::GetGuildTournaments(uint32_t guild_id) const {
    std::vector<Tournament> result;
    for (auto& t : tournaments_) {
        for (auto& team : t.teams) {
            if (team.guild_id == guild_id) { result.push_back(t); break; }
        }
    }
    return result;
}

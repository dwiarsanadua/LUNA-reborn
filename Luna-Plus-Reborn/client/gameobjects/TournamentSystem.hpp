#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <ctime>
#include <functional>

enum class TournamentState {
    Registration, InProgress, Completed, Cancelled
};

struct TournamentTeam {
    uint32_t guild_id = 0;
    std::string guild_name;
    uint32_t team_leader_id = 0;
    std::vector<uint32_t> member_ids;
    int seed = 0;
    bool eliminated = false;
    int placement = 0; // 1 = winner, 2 = runner-up, etc.
};

struct TournamentMatch {
    int round = 0;
    int match_index = 0;
    uint32_t team1_id = 0;
    uint32_t team2_id = 0;
    uint32_t winner_id = 0;
    bool completed = false;
    time_t scheduled_time = 0;
};

struct Tournament {
    uint32_t id = 0;
    std::string name;
    int max_teams = 8;
    int min_team_size = 3;
    int max_team_size = 6;
    TournamentState state = TournamentState::Registration;
    std::vector<TournamentTeam> teams;
    std::vector<TournamentMatch> matches;
    int current_round = 0;
    time_t registration_end = 0;
    uint32_t prize_gold = 0;
    std::string prize_item;
    bool started = false;
    bool completed = false;
    
    std::function<void(uint32_t winner_id, uint32_t runner_up_id)> on_complete;
};

class TournamentSystem {
public:
    void Init();
    void Update(float dt);
    
    // Tournament lifecycle
    uint32_t CreateTournament(const std::string& name, int max_teams, uint32_t prize_gold);
    bool RegisterTeam(uint32_t tournament_id, uint32_t guild_id, const std::string& guild_name,
                      const std::vector<uint32_t>& members);
    bool UnregisterTeam(uint32_t tournament_id, uint32_t guild_id);
    bool StartTournament(uint32_t tournament_id);
    bool AdvanceMatch(uint32_t tournament_id, uint32_t winner_id);
    void CompleteTournament(uint32_t tournament_id);
    
    // Queries
    Tournament* GetTournament(uint32_t tournament_id);
    std::vector<Tournament> GetActiveTournaments() const;
    std::vector<Tournament> GetGuildTournaments(uint32_t guild_id) const;
    
    int GetActiveCount() const { return (int)tournaments_.size(); }

private:
    std::vector<Tournament> tournaments_;
    uint32_t next_id_ = 1;
    void GenerateBracket(Tournament& t);
    float tick_timer_ = 0;
};

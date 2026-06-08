#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <ctime>

struct Territory {
    int id = 0;
    std::string name;
    uint32_t owner_guild_id = 0;
    std::string owner_guild_name;
    time_t conquered_date = 0;
    int tax_rate = 10; // percentage
    uint64_t total_tax_collected = 0;
    int defense_bonus = 0;
    bool is_castle = false;
};

struct SiegeSchedule {
    int territory_id = 0;
    time_t siege_time = 0;
    uint32_t attacker_guild_id = 0;
    std::string attacker_name;
    uint32_t defender_guild_id = 0;
    std::string defender_name;
    bool completed = false;
    uint32_t winner_guild_id = 0;
};

struct NetworkSiegeTerritoryView {
    uint32_t territory_id = 0;
    std::string name;
    uint32_t owner_guild_id = 0;
    std::string owner_guild_name;
    uint16_t tax_rate = 10;
    bool is_castle = false;
    uint8_t defense_bonus = 0;
    uint32_t attacker_guild_id = 0;
    std::string attacker_guild_name;
    uint32_t seconds_until_siege = 0;
    uint32_t tax_accumulated = 0;
};

struct NetworkSiegeScheduleView {
    uint32_t territory_id = 0;
    std::string territory_name;
    uint32_t attacker_guild_id = 0;
    std::string attacker_guild_name;
    uint32_t defender_guild_id = 0;
    std::string defender_guild_name;
    uint32_t seconds_until = 0;
};

class SiegeSystem {
public:
    void Init();
    void Update(float dt);
    
    // Territory management
    bool ConquerTerritory(int territory_id, uint32_t guild_id, const std::string& guild_name);
    bool LoseTerritory(int territory_id);
    Territory* GetTerritory(int territory_id);
    std::vector<Territory> GetAllTerritories() const;
    std::vector<Territory> GetGuildTerritories(uint32_t guild_id) const;
    
    // Siege scheduling
    bool ScheduleSiege(int territory_id, time_t siege_time, uint32_t attacker_id, const std::string& attacker_name);
    bool ResolveSiege(int territory_id, uint32_t winner_guild_id);
    std::vector<SiegeSchedule> GetUpcomingSieges() const;
    std::vector<SiegeSchedule> GetGuildSieges(uint32_t guild_id) const;
    
    // Taxation
    uint64_t CollectTax(int territory_id);
    uint64_t GetTotalTax(uint32_t guild_id) const;
    bool SetTaxRate(int territory_id, int rate);
    
    // Guild war declaration
    bool DeclareWar(uint32_t from_guild, uint32_t to_guild);
    bool EndWar(uint32_t guild1, uint32_t guild2);
    bool IsAtWar(uint32_t guild1, uint32_t guild2) const;

    void SyncFromNetwork(const std::vector<NetworkSiegeTerritoryView>& territories,
                         const std::vector<NetworkSiegeScheduleView>& schedules);
    
    int GetTerritoryCount() const { return (int)territories_.size(); }
    int GetActiveSieges() const { return (int)active_sieges_.size(); }

private:
    std::vector<Territory> territories_;
    std::vector<SiegeSchedule> active_sieges_;
    std::vector<std::pair<uint32_t, uint32_t>> wars_; // guild pairs at war
    float tick_timer_ = 0;
};

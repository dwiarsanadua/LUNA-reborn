#include "SiegeSystem.hpp"
#include <algorithm>
#include <cstdlib>
#include <spdlog/spdlog.h>

void SiegeSystem::Init() {
    // Default territories
    territories_ = {
        {1, "Alker Harbor", 0, "", 0, 10, 0, 5, false},
        {2, "Moonlight Forest", 0, "", 0, 8, 0, 3, false},
        {3, "Crystal Valley", 0, "", 0, 12, 0, 7, false},
        {4, "Dragon's Peak", 0, "", 0, 15, 0, 10, false},
        {5, "Royal Castle", 0, "", 0, 20, 0, 15, true},
    };
    spdlog::info("SiegeSystem: initialized with {} territories", territories_.size());
}

void SiegeSystem::Update(float dt) {
    tick_timer_ += dt;
    if (tick_timer_ < 60.0f) return; // Check every minute
    tick_timer_ = 0;
    
    // Check pending sieges
    time_t now = time(nullptr);
    for (auto& siege : active_sieges_) {
        if (!siege.completed && siege.siege_time <= now) {
            // Auto-resolve: attacker wins if no defender, else defender wins
            siege.winner_guild_id = siege.defender_guild_id > 0 ? siege.defender_guild_id : siege.attacker_guild_id;
            siege.completed = true;
            auto* t = GetTerritory(siege.territory_id);
            if (t) {
                if (siege.attacker_guild_id > 0 && siege.defender_guild_id == 0) {
                    t->owner_guild_id = siege.attacker_guild_id;
                    t->owner_guild_name = siege.attacker_name;
                }
            }
        }
    }
}

bool SiegeSystem::ConquerTerritory(int territory_id, uint32_t guild_id, const std::string& guild_name) {
    auto* t = GetTerritory(territory_id);
    if (!t) return false;
    t->owner_guild_id = guild_id;
    t->owner_guild_name = guild_name;
    t->conquered_date = time(nullptr);
    spdlog::info("Siege: guild {} conquered {}", guild_name, t->name);
    return true;
}

bool SiegeSystem::LoseTerritory(int territory_id) {
    auto* t = GetTerritory(territory_id);
    if (!t) return false;
    t->owner_guild_id = 0;
    t->owner_guild_name.clear();
    return true;
}

Territory* SiegeSystem::GetTerritory(int territory_id) {
    for (auto& t : territories_) if (t.id == territory_id) return &t;
    return nullptr;
}

std::vector<Territory> SiegeSystem::GetAllTerritories() const { return territories_; }

std::vector<Territory> SiegeSystem::GetGuildTerritories(uint32_t guild_id) const {
    std::vector<Territory> result;
    for (auto& t : territories_) if (t.owner_guild_id == guild_id) result.push_back(t);
    return result;
}

bool SiegeSystem::ScheduleSiege(int territory_id, time_t siege_time, uint32_t attacker_id, const std::string& attacker_name) {
    auto* t = GetTerritory(territory_id);
    if (!t) return false;
    SiegeSchedule s;
    s.territory_id = territory_id;
    s.siege_time = siege_time;
    s.attacker_guild_id = attacker_id;
    s.attacker_name = attacker_name;
    s.defender_guild_id = t->owner_guild_id;
    s.defender_name = t->owner_guild_name;
    active_sieges_.push_back(s);
    spdlog::info("Siege: scheduled siege on {} by {} vs {}", t->name, attacker_name, t->owner_guild_name);
    return true;
}

bool SiegeSystem::ResolveSiege(int territory_id, uint32_t winner_guild_id) {
    for (auto& s : active_sieges_) {
        if (s.territory_id == territory_id && !s.completed) {
            s.completed = true;
            s.winner_guild_id = winner_guild_id;
            return true;
        }
    }
    auto* t = GetTerritory(territory_id);
    if (t) { t->owner_guild_id = winner_guild_id; return true; }
    return false;
}

std::vector<SiegeSchedule> SiegeSystem::GetUpcomingSieges() const {
    std::vector<SiegeSchedule> result;
    time_t now = time(nullptr);
    for (auto& s : active_sieges_) {
        if (!s.completed && s.siege_time > now) result.push_back(s);
    }
    return result;
}

std::vector<SiegeSchedule> SiegeSystem::GetGuildSieges(uint32_t guild_id) const {
    std::vector<SiegeSchedule> result;
    for (auto& s : active_sieges_) {
        if (s.attacker_guild_id == guild_id || s.defender_guild_id == guild_id)
            result.push_back(s);
    }
    return result;
}

uint64_t SiegeSystem::CollectTax(int territory_id) {
    auto* t = GetTerritory(territory_id);
    if (!t || t->owner_guild_id == 0) return 0;
    uint64_t tax = 1000 + (rand() % 500); // Simulated tax income
    t->total_tax_collected += tax;
    return tax;
}

uint64_t SiegeSystem::GetTotalTax(uint32_t guild_id) const {
    uint64_t total = 0;
    for (auto& t : territories_) if (t.owner_guild_id == guild_id) total += t.total_tax_collected;
    return total;
}

bool SiegeSystem::SetTaxRate(int territory_id, int rate) {
    auto* t = GetTerritory(territory_id);
    if (!t) return false;
    t->tax_rate = std::max(0, std::min(50, rate));
    return true;
}

bool SiegeSystem::DeclareWar(uint32_t from_guild, uint32_t to_guild) {
    if (IsAtWar(from_guild, to_guild)) return false;
    wars_.push_back({from_guild, to_guild});
    return true;
}

bool SiegeSystem::EndWar(uint32_t guild1, uint32_t guild2) {
    wars_.erase(std::remove_if(wars_.begin(), wars_.end(),
        [&](auto& w) { return (w.first == guild1 && w.second == guild2) || (w.first == guild2 && w.second == guild1); }),
        wars_.end());
    return true;
}

bool SiegeSystem::IsAtWar(uint32_t guild1, uint32_t guild2) const {
    for (auto& w : wars_) {
        if ((w.first == guild1 && w.second == guild2) || (w.first == guild2 && w.second == guild1))
            return true;
    }
    return false;
}

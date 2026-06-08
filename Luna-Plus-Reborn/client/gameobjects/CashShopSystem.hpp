#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct CashItem {
    uint32_t id = 0;
    std::string name;
    std::string description;
    int price_luna = 0; // Luna Points (premium currency)
    uint32_t item_id = 0;
    int item_count = 1;
    int max_purchase = 99;
    std::string category;
    bool limited = false;
};

class CashShopSystem {
public:
    void Init();
    
    // Shop items
    std::vector<CashItem> GetItems(const std::string& category = "") const;
    std::vector<std::string> GetCategories() const;
    
    void SyncFromNetwork(const std::vector<CashItem>& items, int luna_points,
                         int bp_level, int bp_xp, int bp_max_xp, bool bp_active,
                         const std::string& season_name);
    
    bool CanAfford(int luna_points, uint32_t price) const { return luna_points >= (int)price; }
    bool PurchaseItem(int& luna_points, uint32_t item_id);
    
    // Luna Points management
    void AddLunaPoints(int& luna_points, int amount) { luna_points += amount; }
    
    // Battle pass / Season pass
    int GetBattlePassLevel() const { return battle_pass_level_; }
    int GetBattlePassXP() const { return battle_pass_xp_; }
    int GetBattlePassMaxXP() const { return 1000; }
    bool IsBattlePassActive() const { return battle_pass_active_; }
    void SetBattlePassActive(bool a) { battle_pass_active_ = a; }
    void AddBattlePassXP(int xp);
    std::vector<std::string> GetBattlePassRewards(int level) const;
    
    // Season pass rotation
    int GetCurrentSeason() const { return season_; }
    std::string GetSeasonName() const;
    const std::string& GetNetworkSeasonName() const { return season_name_; }
    time_t GetSeasonEndTime() const { return season_end_; }
    int GetSeasonDaysRemaining() const;
    void AdvanceSeason();
    bool IsSeasonLimitedItem(const CashItem& item) const;
    void UpdateSeason(float dt);

private:
    std::vector<CashItem> items_;
    int battle_pass_level_ = 1;
    int battle_pass_xp_ = 0;
    bool battle_pass_active_ = false;
    std::string season_name_;
    int season_ = 1;
    time_t season_end_ = 0;
    float season_timer_ = 0;
    void GenerateItems();
};

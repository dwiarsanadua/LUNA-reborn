#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <ctime>

struct EconomyEvent {
    time_t timestamp;
    std::string type; // "gold_earned", "gold_spent", "item_bought", "item_sold", "item_crafted"
    std::string item_name;
    int64_t amount = 0;
    int gold_change = 0;
};

struct ItemPriceSnapshot {
    std::string item_name;
    int avg_price = 0;
    int min_price = 0;
    int max_price = 0;
    int volume = 0;
    time_t recorded_at;
};

class EconomyMonitor {
public:
    void Init();
    void Update(float dt);
    
    // Track economy events
    void LogGoldChange(int delta, const std::string& reason);
    void LogItemTrade(const std::string& item_name, int count, int price, bool bought);
    void LogItemCrafted(const std::string& item_name, int cost);
    
    // Queries
    int GetGoldEarnedToday() const;
    int GetGoldSpentToday() const;
    int GetNetWorthChange() const;
    std::vector<EconomyEvent> GetRecentActivity(int count = 20) const;
    
    // Market prices
    int GetAveragePrice(const std::string& item_name) const;
    std::vector<ItemPriceSnapshot> GetMarketReport() const;
    std::string GenerateReport() const;
    
    // Callbacks
    using ThresholdCallback = std::function<void(const std::string& msg)>;
    void SetThresholdCallback(ThresholdCallback cb) { threshold_cb_ = cb; }

private:
    std::vector<EconomyEvent> events_;
    std::vector<ItemPriceSnapshot> price_history_;
    std::map<std::string, std::vector<int>> price_map_;
    int gold_start_ = 0;
    int current_gold_ = 0;
    time_t day_start_ = 0;
    static constexpr int MAX_EVENTS = 500;
    
    void AggregatePrices();
    void CheckThresholds();
    ThresholdCallback threshold_cb_;
};

#include "EconomyMonitor.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>
#include <cstdio>

void EconomyMonitor::Init() {
    events_.clear();
    price_history_.clear();
    price_map_.clear();
    gold_start_ = 0;
    current_gold_ = 0;
    day_start_ = time(nullptr);
    spdlog::info("EconomyMonitor: initialized");
}

void EconomyMonitor::Update(float dt) {
    (void)dt;
    // Daily reset check
    time_t now = time(nullptr);
    if (difftime(now, day_start_) >= 86400.0) {
        day_start_ = now;
        AggregatePrices();
        spdlog::info("EconomyMonitor: daily report generated");
    }
}

void EconomyMonitor::LogGoldChange(int delta, const std::string& reason) {
    EconomyEvent e;
    e.timestamp = time(nullptr);
    e.gold_change = delta;
    e.amount = std::abs(delta);
    e.type = delta > 0 ? "gold_earned" : "gold_spent";
    e.item_name = reason;
    events_.push_back(e);
    current_gold_ += delta;
    if ((int)events_.size() > MAX_EVENTS) events_.erase(events_.begin());
    CheckThresholds();
}

void EconomyMonitor::LogItemTrade(const std::string& item_name, int count, int price, bool bought) {
    EconomyEvent e;
    e.timestamp = time(nullptr);
    e.type = bought ? "item_bought" : "item_sold";
    e.item_name = item_name;
    e.amount = count;
    e.gold_change = bought ? -price : price;
    events_.push_back(e);
    
    // Track price
    price_map_[item_name].push_back(price / count);
    if ((int)price_map_[item_name].size() > 20)
        price_map_[item_name].erase(price_map_[item_name].begin());
    
    if ((int)events_.size() > MAX_EVENTS) events_.erase(events_.begin());
}

void EconomyMonitor::LogItemCrafted(const std::string& item_name, int cost) {
    EconomyEvent e;
    e.timestamp = time(nullptr);
    e.type = "item_crafted";
    e.item_name = item_name;
    e.amount = 1;
    e.gold_change = -cost;
    events_.push_back(e);
    current_gold_ -= cost;
    if ((int)events_.size() > MAX_EVENTS) events_.erase(events_.begin());
}

int EconomyMonitor::GetGoldEarnedToday() const {
    int total = 0;
    for (auto& e : events_) {
        if (e.gold_change > 0 && difftime(time(nullptr), e.timestamp) < 86400)
            total += e.gold_change;
    }
    return total;
}

int EconomyMonitor::GetGoldSpentToday() const {
    int total = 0;
    for (auto& e : events_) {
        if (e.gold_change < 0 && difftime(time(nullptr), e.timestamp) < 86400)
            total += std::abs(e.gold_change);
    }
    return total;
}

int EconomyMonitor::GetNetWorthChange() const {
    return GetGoldEarnedToday() - GetGoldSpentToday();
}

std::vector<EconomyEvent> EconomyMonitor::GetRecentActivity(int count) const {
    std::vector<EconomyEvent> result;
    int start = std::max(0, (int)events_.size() - count);
    for (int i = start; i < (int)events_.size(); i++) result.push_back(events_[i]);
    return result;
}

int EconomyMonitor::GetAveragePrice(const std::string& item_name) const {
    auto it = price_map_.find(item_name);
    if (it == price_map_.end() || it->second.empty()) return 0;
    int sum = 0;
    for (int p : it->second) sum += p;
    return sum / (int)it->second.size();
}

std::vector<ItemPriceSnapshot> EconomyMonitor::GetMarketReport() const {
    std::vector<ItemPriceSnapshot> report;
    for (auto& [name, prices] : price_map_) {
        if (prices.empty()) continue;
        ItemPriceSnapshot s;
        s.item_name = name;
        s.avg_price = 0;
        s.min_price = *std::min_element(prices.begin(), prices.end());
        s.max_price = *std::max_element(prices.begin(), prices.end());
        s.volume = (int)prices.size();
        s.recorded_at = time(nullptr);
        int sum = 0;
        for (int p : prices) sum += p;
        s.avg_price = sum / (int)prices.size();
        report.push_back(s);
    }
    return report;
}

std::string EconomyMonitor::GenerateReport() const {
    char buf[1024];
    int earned = GetGoldEarnedToday();
    int spent = GetGoldSpentToday();
    int net = GetNetWorthChange();
    int trade_count = 0, craft_count = 0;
    for (auto& e : events_) {
        if (e.type == "item_bought" || e.type == "item_sold") trade_count++;
        if (e.type == "item_crafted") craft_count++;
    }
    snprintf(buf, sizeof(buf),
        "=== Economy Report ===\n"
        "Gold Earned Today: %dg\n"
        "Gold Spent Today: %dg\n"
        "Net Change: %+dg\n"
        "Total Trades: %d\n"
        "Total Crafts: %d\n"
        "Tracked Items: %zu",
        earned, spent, net, trade_count, craft_count, price_map_.size());
    return std::string(buf);
}

void EconomyMonitor::AggregatePrices() {
    for (auto& [name, prices] : price_map_) {
        if (prices.empty()) continue;
        ItemPriceSnapshot s;
        s.item_name = name;
        s.avg_price = 0;
        int sum = 0;
        s.min_price = *std::min_element(prices.begin(), prices.end());
        s.max_price = *std::max_element(prices.begin(), prices.end());
        s.volume = (int)prices.size();
        for (int p : prices) sum += p;
        s.avg_price = sum / (int)prices.size();
        s.recorded_at = day_start_;
        price_history_.push_back(s);
    }
    if ((int)price_history_.size() > 100) {
        price_history_.erase(price_history_.begin(), price_history_.begin() + (price_history_.size() - 100));
    }
}

void EconomyMonitor::CheckThresholds() {
    if (!threshold_cb_) return;
    int spent = GetGoldSpentToday();
    if (spent > 10000) {
        threshold_cb_("Warning: You've spent over 10,000 gold today!");
    }
    if (current_gold_ - gold_start_ > 50000) {
        threshold_cb_("Achievement: Earned 50,000+ gold net!");
    }
}

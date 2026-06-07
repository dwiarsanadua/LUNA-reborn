#include "CashShopSystem.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

void CashShopSystem::Init() {
    items_.clear();
    GenerateItems();
    season_ = 1;
    season_end_ = time(nullptr) + 30 * 86400; // 30 days per season
    spdlog::info("CashShopSystem: initialized with {} items, season {} ends in {}d",
                 items_.size(), season_, 30);
}

void CashShopSystem::UpdateSeason(float dt) {
    season_timer_ += dt;
    if (season_timer_ >= 86400.0f) { // Check once per day
        season_timer_ = 0;
        if (time(nullptr) >= season_end_) {
            AdvanceSeason();
        }
    }
}

int CashShopSystem::GetSeasonDaysRemaining() const {
    return (int)std::max(0.0, difftime(season_end_, time(nullptr)) / 86400.0);
}

std::string CashShopSystem::GetSeasonName() const {
    switch (season_) {
    case 1: return "Spring Blossom";
    case 2: return "Summer Festival";
    case 3: return "Autumn Harvest";
    case 4: return "Winter Wonderland";
    default: return "Season " + std::to_string(season_);
    }
}

void CashShopSystem::AdvanceSeason() {
    season_++;
    season_end_ = time(nullptr) + 30 * 86400;
    battle_pass_level_ = 1;
    battle_pass_xp_ = 0;
    spdlog::info("CashShop: season advanced to {} ({})", season_, GetSeasonName());
}

bool CashShopSystem::IsSeasonLimitedItem(const CashItem& item) const {
    return item.limited;
}

void CashShopSystem::GenerateItems() {
    // Premium items
    CashItem i;
    i = {}; i.id = 1; i.name = "Luna Points Pack (100)"; i.description = "100 Luna Points"; i.price_luna = 0; i.item_id = 1; i.item_count = 100; i.category = "Currency"; items_.push_back(i);
    i = {}; i.id = 2; i.name = "EXP Boost (1h)"; i.description = "Double EXP for 1 hour"; i.price_luna = 50; i.item_id = 101; i.item_count = 1; i.category = "Boosts"; items_.push_back(i);
    i = {}; i.id = 3; i.name = "Gold Boost (1h)"; i.description = "Double Gold for 1 hour"; i.price_luna = 40; i.item_id = 102; i.item_count = 1; i.category = "Boosts"; items_.push_back(i);
    i = {}; i.id = 4; i.name = "Pet Summon Scroll"; i.description = "Summon a cute pet"; i.price_luna = 200; i.item_id = 201; i.item_count = 1; i.category = "Pets"; items_.push_back(i);
    i = {}; i.id = 5; i.name = "Storage Expansion"; i.description = "+8 storage slots"; i.price_luna = 150; i.item_id = 301; i.item_count = 8; i.category = "Inventory"; items_.push_back(i);
    i = {}; i.id = 6; i.name = "Inventory Expansion"; i.description = "+8 inventory slots"; i.price_luna = 150; i.item_id = 302; i.item_count = 8; i.category = "Inventory"; items_.push_back(i);
    i = {}; i.id = 7; i.name = "Fashion Box"; i.description = "Random costume piece"; i.price_luna = 300; i.item_id = 401; i.item_count = 1; i.max_purchase = 5; i.category = "Fashion"; items_.push_back(i);
    i = {}; i.id = 8; i.name = "Mount: White Horse"; i.description = "Legendary white horse mount"; i.price_luna = 500; i.item_id = 501; i.item_count = 1; i.category = "Mounts"; items_.push_back(i);
    i = {}; i.id = 9; i.name = "Dungeon Skip Ticket"; i.description = "Skip one dungeon floor"; i.price_luna = 80; i.item_id = 601; i.item_count = 1; i.category = "Consumables"; items_.push_back(i);
    i = {}; i.id = 10; i.name = "Auto-Loot Pet 30d"; i.description = "Auto-loots items for 30 days"; i.price_luna = 400; i.item_id = 202; i.item_count = 1; i.category = "Pets"; items_.push_back(i);
    i = {}; i.id = 11; i.name = "Premium Hair Dye"; i.description = "Rare hair color change"; i.price_luna = 100; i.item_id = 402; i.item_count = 1; i.category = "Fashion"; items_.push_back(i);
    i = {}; i.id = 12; i.name = "Lucky Enchant Scroll"; i.description = "+15% enchant success rate"; i.price_luna = 250; i.item_id = 701; i.item_count = 1; i.category = "Consumables"; items_.push_back(i);
}

std::vector<CashItem> CashShopSystem::GetItems(const std::string& category) const {
    if (category.empty()) return items_;
    std::vector<CashItem> result;
    for (auto& i : items_) if (i.category == category) result.push_back(i);
    return result;
}

std::vector<std::string> CashShopSystem::GetCategories() const {
    std::vector<std::string> cats;
    for (auto& i : items_) {
        if (std::find(cats.begin(), cats.end(), i.category) == cats.end())
            cats.push_back(i.category);
    }
    return cats;
}

bool CashShopSystem::PurchaseItem(int& luna_points, uint32_t item_id) {
    for (auto& i : items_) {
        if (i.id == item_id) {
            if (luna_points >= i.price_luna) {
                luna_points -= i.price_luna;
                spdlog::info("CashShop: purchased {} ({} LP)", i.name, i.price_luna);
                return true;
            }
            return false;
        }
    }
    return false;
}

void CashShopSystem::AddBattlePassXP(int xp) {
    battle_pass_xp_ += xp;
    int needed = GetBattlePassMaxXP();
    while (battle_pass_xp_ >= needed) {
        battle_pass_xp_ -= needed;
        battle_pass_level_++;
        spdlog::info("BattlePass: reached level {}", battle_pass_level_);
    }
}

std::vector<std::string> CashShopSystem::GetBattlePassRewards(int level) const {
    std::vector<std::string> rewards;
    switch (level) {
    case 1: rewards = {"100 Gold", "Pet Scroll"}; break;
    case 2: rewards = {"200 Gold", "EXP Potion"}; break;
    case 3: rewards = {"Fashion Hat", "300 Gold"}; break;
    case 4: rewards = {"500 Gold", "Storage Slot +1"}; break;
    case 5: rewards = {"Mount: Black Horse", "1000 Gold", "Title: Adventurer"}; break;
    case 6: rewards = {"1000 Gold", "Lucky Enchant Scroll"}; break;
    case 7: rewards = {"1500 Gold", "Fashion Body"}; break;
    case 8: rewards = {"2000 Gold", "Inventory Slot +2"}; break;
    case 9: rewards = {"2500 Gold", "Premium Hair Dye"}; break;
    case 10: rewards = {"5000 Gold", "Legendary Weapon Skin", "Title: Battle Master"}; break;
    default: rewards = {"500 Gold"}; break;
    }
    return rewards;
}

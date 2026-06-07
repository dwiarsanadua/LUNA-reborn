#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <sqlite3.h>

struct MonsterData {
    uint32_t monster_id;
    std::string name;
    std::string model_file;
    uint16_t level;
    int32_t hp, mp, attack, defense;
    uint32_t exp;
    uint32_t gold_min, gold_max;
    uint32_t drop_table_id;
};

struct ItemData {
    uint32_t item_id;
    std::string name;
    uint16_t item_type;
    uint16_t item_subtype;
    uint16_t level_required;
    int32_t attack, defense, magic_attack, magic_defense;
    int32_t price_buy, price_sell;
};

struct SkillData {
    uint32_t skill_id;
    std::string name;
    uint16_t class_id;
    uint16_t level_required;
    uint16_t target_type;
    float range;
    int32_t cost_hp, cost_mp;
    uint32_t cooldown_ms;
    float damage_mult;
    int32_t damage_fixed;
    uint32_t buff_id;
    uint32_t buff_duration;
};

struct QuestData {
    uint32_t quest_id;
    std::string title;
    uint16_t level_required;
    uint32_t reward_exp;
    uint32_t reward_gold;
    uint32_t reward_item_id;
    uint16_t reward_item_count;
    uint32_t giver_npc;
    uint32_t completer_npc;
};

struct DropEntry {
    uint32_t item_id;
    uint16_t min_count;
    uint16_t max_count;
    float probability;
};

struct ShopEntry {
    uint32_t item_id;
    uint32_t price;
};

class GameDataDB {
public:
    bool Open(const std::string& path);
    void Close();

    MonsterData GetMonster(uint32_t monster_id) const;
    std::vector<MonsterData> GetAllMonsters() const;
    ItemData GetItem(uint32_t item_id) const;
    std::vector<ItemData> GetAllItems() const;
    SkillData GetSkill(uint32_t skill_id) const;
    std::vector<SkillData> GetAllSkills() const;
    QuestData GetQuest(uint32_t quest_id) const;
    std::vector<DropEntry> GetDrops(uint32_t monster_id) const;
    std::vector<ShopEntry> GetShopItems(uint32_t npc_id) const;

private:
    sqlite3* db_ = nullptr;
    template<typename T> T Col(sqlite3_stmt* s, int i) const;
};

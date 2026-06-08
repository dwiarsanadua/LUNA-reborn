#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <sqlite3.h>
#include <fstream>
#include <sstream>
#include <cmath>

struct MonsterData {
    uint32_t monster_id;
    std::string name;
    std::string model_file;
    uint16_t level;
    int32_t hp, mp, attack, defense;
    uint32_t exp;
    uint32_t gold_min, gold_max;
    uint32_t drop_table_id;
    uint16_t element_type;
    uint16_t ai_type;
    uint16_t aggro_range;
    float speed;
    float size_scale;
};

struct ItemData {
    uint32_t item_id;
    std::string name;
    uint16_t item_type;
    uint16_t item_subtype;
    uint16_t level_required;
    int32_t attack, defense, magic_attack, magic_defense;
    int32_t price_buy, price_sell;
    uint16_t rarity;
    uint16_t max_stack;
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
    uint16_t skill_type;
    uint16_t weapon_type;
    uint16_t sp_cost;
};

struct QuestData {
    uint32_t quest_id;
    std::string title;
    std::string description;
    uint16_t level_required;
    uint32_t reward_exp;
    uint32_t reward_gold;
    uint32_t reward_item_id;
    uint16_t reward_item_count;
    uint32_t giver_npc;
    uint32_t completer_npc;
};

struct QuestCondition {
    uint32_t condition_id;
    uint32_t quest_id;
    uint16_t condition_type;
    uint32_t target_id;
    uint16_t target_count;
    uint32_t map_id;
    float pos_x, pos_y;
    float radius;
};

struct DropEntry {
    uint32_t item_id;
    std::string item_name;
    uint16_t min_count;
    uint16_t max_count;
    float probability;
    uint32_t drop_table_id;
};

struct ShopEntry {
    uint32_t item_id;
    uint32_t price;
    int32_t stock;
};

struct NPCTemplate {
    uint32_t npc_id;
    std::string name;
    uint16_t npc_type;
    uint16_t shop_type;
    std::string dialog_text;
};

struct NPCPosition {
    uint32_t position_id;
    uint32_t map_id;
    uint32_t npc_id;
    std::string name;
    uint16_t npc_type;
    float pos_x, pos_y, pos_z;
    float rotation;
};

struct MapWarp {
    uint32_t warp_id;
    uint32_t map_from;
    uint32_t map_to;
    float from_x, from_z;
    float to_x, to_z;
    std::string name;
    std::string dest_name;
    uint32_t fee;
};

struct MapBoundary {
    uint32_t boundary_id;
    uint32_t map_a;
    uint32_t map_b;
    float boundary_x_a, boundary_z_a;
    float boundary_x_b, boundary_z_b;
    std::string name_a;
    std::string name_b;
    uint32_t level_required;
};

struct MapData {
    uint32_t map_id;
    std::string name;
    std::string file_path;
    std::string hgt_file;
    float box_min_x, box_min_y, box_min_z;
    float box_max_x, box_max_y, box_max_z;
};

struct MonsterSpawn {
    uint32_t spawn_id;
    uint32_t map_id;
    uint32_t monster_id;
    uint16_t count;
    uint32_t respawn_time;
    float spawn_radius;
};

struct BuffSkill {
    uint32_t buff_id;
    std::string name;
    uint16_t buff_level;
    uint32_t skill_ref_id;
    uint32_t duration_ms;
    uint16_t buff_type;
    int32_t buff_value;
    uint16_t buff_chance;
    uint32_t icon_id;
};

struct SkillTreeDBEntry {
    uint32_t entry_id;
    uint16_t class_id;
    uint16_t tree_level;
    uint16_t slot_index;
    uint32_t skill_id;
};

// JSON-loaded template structs (from assets/data/*.json)
struct ItemTemplate {
    uint32_t id = 0;
    std::string name;
    uint16_t item_type = 0;
    uint16_t item_subtype = 0;
    uint16_t rarity = 0;
    uint16_t required_level = 0;
    int32_t stats[6] = {0};
    int32_t buy_price = 0;
    int32_t sell_price = 0;
    float drop_rate = 0.0f;
    uint16_t durability = 0;
    uint16_t max_stack = 1;
};

struct MonsterTemplate {
    uint32_t id = 0;
    std::string name;
    uint16_t level = 0;
    int32_t hp = 0, mp = 0;
    int32_t atk = 0, def = 0;
    int32_t matk = 0, mdef = 0;
    float speed = 1.0f;
    uint32_t exp = 0;
    uint32_t gold_min = 0, gold_max = 0;
    std::vector<uint32_t> loot_table;
    std::vector<uint32_t> spawn_map;
    uint16_t aggro_range = 0;
    uint16_t attack_range = 3;
    uint16_t ai_type = 0;
};

struct SkillTemplate {
    uint32_t id = 0;
    std::string name;
    uint16_t type = 0;
    uint16_t required_level = 0;
    uint16_t required_class = 0;
    int32_t mp_cost = 0;
    uint32_t cooldown_ms = 0;
    float damage_mult = 1.0f;
    float range = 0.0f;
    float duration = 0.0f;
    uint16_t target_type = 0;
    std::string animation;
    uint32_t effect_id = 0;
};

#ifndef GAMEDATADB_QUEST_TEMPLATE_DEFINED
#define GAMEDATADB_QUEST_TEMPLATE_DEFINED
struct QuestTemplate {
    uint32_t id = 0;
    std::string name;
    uint16_t level_required = 0;
    uint32_t npc_start_id = 0;
    uint32_t npc_complete_id = 0;
    std::vector<uint32_t> conditions;
    std::vector<uint32_t> rewards;
    std::string dialog_start;
    std::string dialog_progress;
    std::string dialog_complete;
};
#endif

struct NPCData {
    uint32_t id = 0;
    std::string name;
    uint32_t map_id = 0;
    float pos_x = 0.0f, pos_y = 0.0f, pos_z = 0.0f;
    uint16_t type = 0;
    std::vector<uint32_t> shop_items;
    std::vector<std::string> dialog_texts;
};

class GameDataDB {
public:
    bool Open(const std::string& path);
    void Close();

    // Legacy DB (game_data_legacy.db) — in-memory loaders
    bool OpenLegacy(const std::string& path);
    void CloseLegacy();

    void LoadMonsterTemplates();
    void LoadNPCTemplates();
    void LoadQuestData();
    void LoadSkillData();
    void LoadItemTemplates();
    void LoadMapData();

    // Initialize: load all JSON templates from a directory
    void Initialize(const std::string& json_dir = "assets/data/");

    // JSON-based loaders (from assets/data/*.json exported by data_parser.py)
    void LoadItemTemplates(const std::string& json_path);
    void LoadMonsterTemplates(const std::string& json_path);
    void LoadSkillTemplates(const std::string& json_path);
    void LoadQuestTemplates(const std::string& json_path);
    void LoadNPCTemplates(const std::string& json_path);

    // Accessors (primary game_data.db)
    MonsterData GetMonster(uint32_t monster_id) const;
    std::vector<MonsterData> GetAllMonsters() const;
    ItemData GetItem(uint32_t item_id) const;
    std::vector<ItemData> GetAllItems() const;
    SkillData GetSkill(uint32_t skill_id) const;
    std::vector<SkillData> GetAllSkills() const;
    QuestData GetQuest(uint32_t quest_id) const;
    std::vector<DropEntry> GetDrops(uint32_t monster_id) const;
    std::vector<ShopEntry> GetShopItems(uint32_t npc_id) const;

    // Accessors (legacy DB — in-memory)
    const MonsterData* GetMonsterLegacy(uint32_t monster_id) const;
    const ItemData* GetItemLegacy(uint32_t item_id) const;
    const SkillData* GetSkillLegacy(uint32_t skill_id) const;
    const QuestData* GetQuestLegacy(uint32_t quest_id) const;
    const NPCTemplate* GetNPCTemplate(uint32_t npc_id) const;
    const BuffSkill* GetBuffSkill(uint32_t buff_id) const;

    std::vector<DropEntry> GetMonsterDrops(uint32_t monster_id) const;
    std::vector<NPCPosition> GetNPCPositions(uint32_t map_id) const;
    std::vector<ShopEntry> GetNPCShop(uint32_t npc_id) const;
    std::vector<QuestCondition> GetQuestConditions(uint32_t quest_id) const;
    std::vector<MapWarp> GetMapWarps(uint32_t map_id) const;
    std::vector<MonsterSpawn> GetMonsterSpawns(uint32_t map_id) const;
    std::vector<SkillTreeDBEntry> GetSkillTree(uint16_t class_id) const;

    // JSON template accessors
    const ItemTemplate* GetItemTemplate(uint32_t id) const;
    const MonsterTemplate* GetMonsterTemplate(uint32_t id) const;
    const SkillTemplate* GetSkillTemplate(uint32_t id) const;
    const QuestTemplate* GetQuestTemplate(uint32_t id) const;
    const NPCData* GetNPCData(uint32_t id) const;

private:
    sqlite3* db_ = nullptr;
    sqlite3* legacy_db_ = nullptr;
    template<typename T> T Col(sqlite3_stmt* s, int i) const;

    // In-memory caches (populated by Load* functions)
    std::unordered_map<uint32_t, MonsterData> monsters_legacy_;
    std::unordered_map<uint32_t, ItemData> items_legacy_;
    std::unordered_map<uint32_t, SkillData> skills_legacy_;
    std::unordered_map<uint32_t, QuestData> quests_legacy_;
    std::unordered_map<uint32_t, NPCTemplate> npc_templates_;
    std::unordered_map<uint32_t, BuffSkill> buff_skills_;
    std::unordered_map<uint32_t, std::vector<DropEntry>> monster_drops_;
    std::unordered_map<uint32_t, std::vector<NPCPosition>> npc_positions_;
    std::unordered_map<uint32_t, std::vector<ShopEntry>> npc_shop_;
    std::unordered_map<uint32_t, std::vector<QuestCondition>> quest_conditions_;
    std::unordered_map<uint32_t, std::vector<MapWarp>> map_warps_;
    std::unordered_map<uint32_t, std::vector<MonsterSpawn>> monster_spawns_;
    std::unordered_map<uint32_t, std::vector<SkillTreeDBEntry>> skill_trees_;
    std::unordered_map<uint16_t, std::vector<SkillTreeDBEntry>> skill_trees_by_class_;
    std::vector<MapData> map_data_;
    std::vector<MapBoundary> map_boundaries_;

    // JSON-loaded template caches
    std::unordered_map<uint32_t, ItemTemplate> items_json_;
    std::unordered_map<uint32_t, MonsterTemplate> monsters_json_;
    std::unordered_map<uint32_t, SkillTemplate> skills_json_;
    std::unordered_map<uint32_t, QuestTemplate> quests_json_;
    std::unordered_map<uint32_t, NPCData> npcs_json_;
};

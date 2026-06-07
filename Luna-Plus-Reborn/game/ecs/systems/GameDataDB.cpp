#include "GameDataDB.hpp"
#include <spdlog/spdlog.h>
#include <cstring>

template<> int32_t GameDataDB::Col<int32_t>(sqlite3_stmt* s, int i) const { return sqlite3_column_int(s, i); }
template<> uint32_t GameDataDB::Col<uint32_t>(sqlite3_stmt* s, int i) const { return static_cast<uint32_t>(sqlite3_column_int(s, i)); }
template<> uint16_t GameDataDB::Col<uint16_t>(sqlite3_stmt* s, int i) const { return static_cast<uint16_t>(sqlite3_column_int(s, i)); }
template<> float GameDataDB::Col<float>(sqlite3_stmt* s, int i) const { return static_cast<float>(sqlite3_column_double(s, i)); }
template<> std::string GameDataDB::Col<std::string>(sqlite3_stmt* s, int i) const {
    auto t = sqlite3_column_text(s, i);
    return t ? reinterpret_cast<const char*>(t) : "";
}

static sqlite3_stmt* prepare(sqlite3* db, const char* sql) {
    sqlite3_stmt* s = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &s, nullptr);
    return s;
}

bool GameDataDB::Open(const std::string& path) {
    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        spdlog::error("GameDataDB: failed to open {}", path);
        return false;
    }
    spdlog::info("GameDataDB: opened {}", path);
    return true;
}

void GameDataDB::Close() {
    if (db_) sqlite3_close(db_);
    db_ = nullptr;
}

static MonsterData ReadMonster(sqlite3_stmt* stmt) {
    MonsterData m{};
    m.monster_id = sqlite3_column_int(stmt, 0);
    auto name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    if (name) m.name = name;
    auto model = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    if (model) m.model_file = model;
    m.level = static_cast<uint16_t>(sqlite3_column_int(stmt, 3));
    m.hp = sqlite3_column_int(stmt, 4);
    m.attack = sqlite3_column_int(stmt, 5);
    m.defense = sqlite3_column_int(stmt, 6);
    m.exp = static_cast<uint32_t>(sqlite3_column_int(stmt, 7));
    return m;
}

MonsterData GameDataDB::GetMonster(uint32_t monster_id) const {
    MonsterData m{};
    if (!db_) return m;
    auto s = prepare(db_, "SELECT col_0000,col_0001,col_0003,col_0008,col_0014,col_0016,col_0017,col_0026 FROM game_monsterlist WHERE col_0000=?");
    if (!s) return m;
    sqlite3_bind_int(s, 1, static_cast<int>(monster_id));
    if (sqlite3_step(s) == SQLITE_ROW) m = ReadMonster(s);
    sqlite3_finalize(s);
    return m;
}

std::vector<MonsterData> GameDataDB::GetAllMonsters() const {
    std::vector<MonsterData> monsters;
    if (!db_) return monsters;
    auto s = prepare(db_, "SELECT col_0000,col_0001,col_0003,col_0008,col_0014,col_0016,col_0017,col_0026 FROM game_monsterlist");
    if (!s) return monsters;
    while (sqlite3_step(s) == SQLITE_ROW) monsters.push_back(ReadMonster(s));
    sqlite3_finalize(s);
    return monsters;
}

ItemData GameDataDB::GetItem(uint32_t item_id) const {
    ItemData item{};
    if (!db_) return item;
    auto s = prepare(db_, "SELECT col_0000,col_0001,col_0002,col_0003,col_0012,col_0019,col_0021,col_0022,col_0023 FROM game_itemlist WHERE col_0000=?");
    if (!s) return item;
    sqlite3_bind_int(s, 1, static_cast<int>(item_id));
    if (sqlite3_step(s) == SQLITE_ROW) {
        item.item_id = Col<uint32_t>(s, 0);
        item.name = Col<std::string>(s, 1);
        item.item_type = Col<uint16_t>(s, 2);
        item.item_subtype = Col<uint16_t>(s, 3);
        item.level_required = Col<uint16_t>(s, 4);
        item.attack = Col<int32_t>(s, 5);
        item.defense = Col<int32_t>(s, 6);
        item.magic_attack = Col<int32_t>(s, 7);
        item.magic_defense = Col<int32_t>(s, 8);
    }
    sqlite3_finalize(s);
    return item;
}

std::vector<ItemData> GameDataDB::GetAllItems() const {
    std::vector<ItemData> items;
    if (!db_) return items;
    auto s = prepare(db_, "SELECT col_0000,col_0001,col_0002,col_0003,col_0012,col_0019,col_0021,col_0022,col_0023 FROM game_itemlist");
    if (!s) return items;
    while (sqlite3_step(s) == SQLITE_ROW) {
        ItemData item{};
        item.item_id = Col<uint32_t>(s, 0);
        item.name = Col<std::string>(s, 1);
        item.item_type = Col<uint16_t>(s, 2);
        item.item_subtype = Col<uint16_t>(s, 3);
        item.level_required = Col<uint16_t>(s, 4);
        item.attack = Col<int32_t>(s, 5);
        item.defense = Col<int32_t>(s, 6);
        item.magic_attack = Col<int32_t>(s, 7);
        item.magic_defense = Col<int32_t>(s, 8);
        items.push_back(item);
    }
    sqlite3_finalize(s);
    return items;
}

SkillData GameDataDB::GetSkill(uint32_t skill_id) const {
    SkillData sk{};
    if (!db_) return sk;
    auto s = prepare(db_, "SELECT col_0000,col_0001,col_0002,col_0003,col_0008,col_0019,col_0020,col_0021,col_0022 FROM game_skilllist WHERE col_0000=?");
    if (!s) return sk;
    sqlite3_bind_int(s, 1, static_cast<int>(skill_id));
    if (sqlite3_step(s) == SQLITE_ROW) {
        sk.skill_id = Col<uint32_t>(s, 0);
        sk.name = Col<std::string>(s, 1);
        sk.class_id = Col<uint16_t>(s, 2);
        sk.level_required = Col<uint16_t>(s, 3);
        sk.range = Col<float>(s, 4);
        sk.cost_hp = Col<int32_t>(s, 5);
        sk.cost_mp = Col<int32_t>(s, 6);
        sk.cooldown_ms = Col<uint32_t>(s, 7);
    }
    sqlite3_finalize(s);
    return sk;
}

std::vector<SkillData> GameDataDB::GetAllSkills() const {
    std::vector<SkillData> skills;
    if (!db_) return skills;
    auto s = prepare(db_, "SELECT col_0000,col_0001 FROM game_skilllist");
    if (!s) return skills;
    while (sqlite3_step(s) == SQLITE_ROW) {
        SkillData sk{};
        sk.skill_id = Col<uint32_t>(s, 0);
        sk.name = Col<std::string>(s, 1);
        skills.push_back(sk);
    }
    sqlite3_finalize(s);
    return skills;
}

QuestData GameDataDB::GetQuest(uint32_t quest_id) const {
    QuestData q{};
    if (!db_) return q;
    auto s = prepare(db_, "SELECT col_0000,col_0001,col_0002,col_0003,col_0004,col_0005,col_0006,col_0007 FROM game_quest WHERE col_0000=?");
    if (!s) return q;
    sqlite3_bind_int(s, 1, static_cast<int>(quest_id));
    if (sqlite3_step(s) == SQLITE_ROW) {
        q.quest_id = Col<uint32_t>(s, 0);
        q.title = Col<std::string>(s, 1);
        q.level_required = Col<uint16_t>(s, 2);
        q.reward_exp = Col<uint32_t>(s, 3);
        q.reward_gold = Col<uint32_t>(s, 4);
        q.reward_item_id = Col<uint32_t>(s, 5);
        q.reward_item_count = Col<uint16_t>(s, 6);
    }
    sqlite3_finalize(s);
    return q;
}

std::vector<DropEntry> GameDataDB::GetDrops(uint32_t monster_id) const {
    std::vector<DropEntry> drops;
    if (!db_) return drops;
    auto s = prepare(db_, "SELECT col_0000,col_0001,col_0002,col_0003 FROM game_monsterdropitemlist WHERE col_0000=?");
    if (!s) return drops;
    sqlite3_bind_int(s, 1, static_cast<int>(monster_id));
    while (sqlite3_step(s) == SQLITE_ROW) {
        DropEntry d;
        d.item_id = Col<uint32_t>(s, 1);
        d.min_count = Col<uint16_t>(s, 2);
        d.max_count = Col<uint16_t>(s, 3);
        d.probability = 0.5f;
        drops.push_back(d);
    }
    sqlite3_finalize(s);
    return drops;
}

std::vector<ShopEntry> GameDataDB::GetShopItems(uint32_t npc_id) const {
    std::vector<ShopEntry> items;
    if (!db_) return items;
    auto s = prepare(db_, "SELECT col_0001,col_0002 FROM game_npcshop WHERE col_0000=?");
    if (!s) return items;
    sqlite3_bind_int(s, 1, static_cast<int>(npc_id));
    while (sqlite3_step(s) == SQLITE_ROW) {
        ShopEntry e;
        e.item_id = Col<uint32_t>(s, 0);
        e.price = Col<uint32_t>(s, 1);
        items.push_back(e);
    }
    sqlite3_finalize(s);
    return items;
}

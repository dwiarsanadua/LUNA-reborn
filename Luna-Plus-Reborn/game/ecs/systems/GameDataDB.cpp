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

static std::string col_str(sqlite3_stmt* s, int i) {
    auto t = sqlite3_column_text(s, i);
    return t ? reinterpret_cast<const char*>(t) : "";
}

// ═══════════════════════════════════════════════════════════════════════
//  Primary DB (game_data.db)
// ═══════════════════════════════════════════════════════════════════════

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

// ═══════════════════════════════════════════════════════════════════════
//  Legacy DB (game_data_legacy.db) — Open / Close
// ═══════════════════════════════════════════════════════════════════════

bool GameDataDB::OpenLegacy(const std::string& path) {
    if (!legacy_db_) {
        if (sqlite3_open(path.c_str(), &legacy_db_) != SQLITE_OK) {
            spdlog::error("GameDataDB: failed to open legacy DB {}", path);
            return false;
        }
    }
    spdlog::info("GameDataDB: legacy DB opened {}", path);
    return true;
}

void GameDataDB::CloseLegacy() {
    if (legacy_db_) sqlite3_close(legacy_db_);
    legacy_db_ = nullptr;
    monsters_legacy_.clear();
    items_legacy_.clear();
    skills_legacy_.clear();
    quests_legacy_.clear();
    npc_templates_.clear();
    buff_skills_.clear();
    monster_drops_.clear();
    npc_positions_.clear();
    npc_shop_.clear();
    quest_conditions_.clear();
    map_warps_.clear();
    monster_spawns_.clear();
    skill_trees_.clear();
    skill_trees_by_class_.clear();
    map_data_.clear();
    map_boundaries_.clear();
}

// ═══════════════════════════════════════════════════════════════════════
//  LoadMonsterTemplates  (Task D2)
// ═══════════════════════════════════════════════════════════════════════

void GameDataDB::LoadMonsterTemplates() {
    if (!legacy_db_) return;
    monsters_legacy_.clear();
    monster_drops_.clear();
    monster_spawns_.clear();

    auto s = prepare(legacy_db_,
        "SELECT id, name, model_file, level, hp, mp, attack, defense, "
        "speed, exp_reward, gold_min, gold_max, element_type, ai_type, "
        "aggro_range, size_scale, monster_type FROM monster_templates");
    if (!s) { spdlog::error("LoadMonsterTemplates: prepare failed"); return; }

    int count = 0;
    while (sqlite3_step(s) == SQLITE_ROW) {
        MonsterData m{};
        m.monster_id = Col<uint32_t>(s, 0);
        m.name = col_str(s, 1);
        m.model_file = col_str(s, 2);
        m.level = Col<uint16_t>(s, 3);
        m.hp = Col<int32_t>(s, 4);
        m.mp = Col<int32_t>(s, 5);
        m.attack = Col<int32_t>(s, 6);
        m.defense = Col<int32_t>(s, 7);
        m.speed = Col<float>(s, 8);
        m.exp = Col<uint32_t>(s, 9);
        m.gold_min = Col<uint32_t>(s, 10);
        m.gold_max = Col<uint32_t>(s, 11);
        m.element_type = Col<uint16_t>(s, 12);
        m.ai_type = Col<uint16_t>(s, 13);
        m.aggro_range = Col<uint16_t>(s, 14);
        m.size_scale = Col<float>(s, 15);
        monsters_legacy_[m.monster_id] = m;
        count++;
    }
    sqlite3_finalize(s);

    // Load monster drops
    auto sd = prepare(legacy_db_,
        "SELECT monster_id, item_id, min_count, max_count, probability FROM monster_drops");
    if (sd) {
        int drop_count = 0;
        while (sqlite3_step(sd) == SQLITE_ROW) {
            DropEntry d{};
            auto mid = Col<uint32_t>(sd, 0);
            d.item_id = Col<uint32_t>(sd, 1);
            d.min_count = Col<uint16_t>(sd, 2);
            d.max_count = Col<uint16_t>(sd, 3);
            d.probability = Col<float>(sd, 4);
            monster_drops_[mid].push_back(d);
            drop_count++;
        }
        sqlite3_finalize(sd);
        spdlog::info("LoadMonsterTemplates: {} drops loaded", drop_count);
    }

    // Load monster spawns
    auto ss = prepare(legacy_db_,
        "SELECT map_id, monster_id, count, respawn_time, spawn_radius FROM monster_spawns");
    if (ss) {
        int spawn_count = 0;
        while (sqlite3_step(ss) == SQLITE_ROW) {
            MonsterSpawn sp{};
            sp.spawn_id = static_cast<uint32_t>(spawn_count + 1);
            sp.map_id = Col<uint32_t>(ss, 0);
            sp.monster_id = Col<uint32_t>(ss, 1);
            sp.count = Col<uint16_t>(ss, 2);
            sp.respawn_time = Col<uint32_t>(ss, 3);
            sp.spawn_radius = Col<float>(ss, 4);
            monster_spawns_[sp.map_id].push_back(sp);
            spawn_count++;
        }
        sqlite3_finalize(ss);
        spdlog::info("LoadMonsterTemplates: {} spawns loaded", spawn_count);
    }

    spdlog::info("LoadMonsterTemplates: {} monsters loaded", count);
}

// ═══════════════════════════════════════════════════════════════════════
//  LoadNPCTemplates  (Task D3)
// ═══════════════════════════════════════════════════════════════════════

void GameDataDB::LoadNPCTemplates() {
    if (!legacy_db_) return;
    npc_templates_.clear();
    npc_positions_.clear();
    npc_shop_.clear();

    // NPC templates
    auto s = prepare(legacy_db_,
        "SELECT id, name, npc_type, shop_type FROM npc_templates");
    if (!s) { spdlog::error("LoadNPCTemplates: prepare failed"); return; }

    int count = 0;
    while (sqlite3_step(s) == SQLITE_ROW) {
        NPCTemplate npc{};
        npc.npc_id = Col<uint32_t>(s, 0);
        npc.name = col_str(s, 1);
        npc.npc_type = Col<uint16_t>(s, 2);
        npc.shop_type = Col<uint16_t>(s, 3);
        npc_templates_[npc.npc_id] = npc;
        count++;
    }
    sqlite3_finalize(s);

    // NPC positions per map
    auto sp = prepare(legacy_db_,
        "SELECT map_id, npc_id, name, npc_type, pos_x, pos_y, pos_z, rotation FROM npc_positions");
    if (sp) {
        int pos_count = 0;
        while (sqlite3_step(sp) == SQLITE_ROW) {
            NPCPosition np{};
            np.position_id = static_cast<uint32_t>(pos_count + 1);
            np.map_id = Col<uint32_t>(sp, 0);
            np.npc_id = Col<uint32_t>(sp, 1);
            np.name = col_str(sp, 2);
            np.npc_type = Col<uint16_t>(sp, 3);
            np.pos_x = Col<float>(sp, 4);
            np.pos_y = Col<float>(sp, 5);
            np.pos_z = Col<float>(sp, 6);
            np.rotation = Col<float>(sp, 7);
            npc_positions_[np.map_id].push_back(np);
            pos_count++;
        }
        sqlite3_finalize(sp);
        spdlog::info("LoadNPCTemplates: {} positions loaded", pos_count);
    }

    // NPC shop entries
    auto sh = prepare(legacy_db_,
        "SELECT npc_id, item_id, price, stock FROM npc_shop_entries");
    if (sh) {
        int shop_count = 0;
        while (sqlite3_step(sh) == SQLITE_ROW) {
            ShopEntry e{};
            auto npc_id = Col<uint32_t>(sh, 0);
            e.item_id = Col<uint32_t>(sh, 1);
            e.price = Col<uint32_t>(sh, 2);
            e.stock = Col<int32_t>(sh, 3);
            npc_shop_[npc_id].push_back(e);
            shop_count++;
        }
        sqlite3_finalize(sh);
        spdlog::info("LoadNPCTemplates: {} shop entries loaded", shop_count);
    }

    spdlog::info("LoadNPCTemplates: {} NPCs loaded", count);
}

// ═══════════════════════════════════════════════════════════════════════
//  LoadQuestData  (Task D4)
// ═══════════════════════════════════════════════════════════════════════

void GameDataDB::LoadQuestData() {
    if (!legacy_db_) return;
    quests_legacy_.clear();
    quest_conditions_.clear();

    auto s = prepare(legacy_db_,
        "SELECT id, title, description, level_required, giver_npc_id, "
        "completer_npc_id, reward_exp, reward_gold, reward_item_id, reward_item_count "
        "FROM quest_templates");
    if (!s) { spdlog::error("LoadQuestData: prepare failed"); return; }

    int count = 0;
    while (sqlite3_step(s) == SQLITE_ROW) {
        QuestData q{};
        q.quest_id = Col<uint32_t>(s, 0);
        q.title = col_str(s, 1);
        q.description = col_str(s, 2);
        q.level_required = Col<uint16_t>(s, 3);
        q.giver_npc = Col<uint32_t>(s, 4);
        q.completer_npc = Col<uint32_t>(s, 5);
        q.reward_exp = Col<uint32_t>(s, 6);
        q.reward_gold = Col<uint32_t>(s, 7);
        q.reward_item_id = Col<uint32_t>(s, 8);
        q.reward_item_count = Col<uint16_t>(s, 9);
        quests_legacy_[q.quest_id] = q;
        count++;
    }
    sqlite3_finalize(s);

    // Quest conditions
    auto sc = prepare(legacy_db_,
        "SELECT quest_id, condition_type, target_id, target_count, "
        "map_id, pos_x, pos_y, radius FROM quest_conditions");
    if (sc) {
        int cond_count = 0;
        while (sqlite3_step(sc) == SQLITE_ROW) {
            QuestCondition c{};
            c.condition_id = static_cast<uint32_t>(cond_count + 1);
            c.quest_id = Col<uint32_t>(sc, 0);
            c.condition_type = Col<uint16_t>(sc, 1);
            c.target_id = Col<uint32_t>(sc, 2);
            c.target_count = Col<uint16_t>(sc, 3);
            c.map_id = Col<uint32_t>(sc, 4);
            c.pos_x = Col<float>(sc, 5);
            c.pos_y = Col<float>(sc, 6);
            c.radius = Col<float>(sc, 7);
            quest_conditions_[c.quest_id].push_back(c);
            cond_count++;
        }
        sqlite3_finalize(sc);
        spdlog::info("LoadQuestData: {} conditions loaded", cond_count);
    }

    spdlog::info("LoadQuestData: {} quests loaded", count);
}

// ═══════════════════════════════════════════════════════════════════════
//  LoadSkillData  (Task D5)
// ═══════════════════════════════════════════════════════════════════════

void GameDataDB::LoadSkillData() {
    if (!legacy_db_) return;
    skills_legacy_.clear();
    buff_skills_.clear();
    skill_trees_.clear();
    skill_trees_by_class_.clear();

    // Skills
    auto s = prepare(legacy_db_,
        "SELECT id, name, class_id, skill_type, level_required, "
        "cost_hp, cost_mp, cooldown_ms, damage_mult, damage_fixed, "
        "weapon_type, sp_cost FROM skill_data");
    if (!s) { spdlog::error("LoadSkillData: prepare failed"); return; }

    int count = 0;
    while (sqlite3_step(s) == SQLITE_ROW) {
        SkillData sk{};
        sk.skill_id = Col<uint32_t>(s, 0);
        sk.name = col_str(s, 1);
        sk.class_id = Col<uint16_t>(s, 2);
        sk.skill_type = Col<uint16_t>(s, 3);
        sk.level_required = Col<uint16_t>(s, 4);
        sk.cost_hp = Col<int32_t>(s, 5);
        sk.cost_mp = Col<int32_t>(s, 6);
        sk.cooldown_ms = Col<uint32_t>(s, 7);
        sk.damage_mult = Col<float>(s, 8);
        sk.damage_fixed = Col<int32_t>(s, 9);
        sk.weapon_type = Col<uint16_t>(s, 10);
        sk.sp_cost = Col<uint16_t>(s, 11);
        skills_legacy_[sk.skill_id] = sk;
        count++;
    }
    sqlite3_finalize(s);

    // Buff skills
    auto sb = prepare(legacy_db_,
        "SELECT id, name, buff_level, skill_ref_id, duration_ms, "
        "buff_type, buff_value, buff_chance, icon_id FROM buff_skills");
    if (sb) {
        int buff_count = 0;
        while (sqlite3_step(sb) == SQLITE_ROW) {
            BuffSkill b{};
            b.buff_id = Col<uint32_t>(sb, 0);
            b.name = col_str(sb, 1);
            b.buff_level = Col<uint16_t>(sb, 2);
            b.skill_ref_id = Col<uint32_t>(sb, 3);
            b.duration_ms = Col<uint32_t>(sb, 4);
            b.buff_type = Col<uint16_t>(sb, 5);
            b.buff_value = Col<int32_t>(sb, 6);
            b.buff_chance = Col<uint16_t>(sb, 7);
            b.icon_id = Col<uint32_t>(sb, 8);
            buff_skills_[b.buff_id] = b;
            buff_count++;
        }
        sqlite3_finalize(sb);
        spdlog::info("LoadSkillData: {} buffs loaded", buff_count);
    }

    // Skill trees
    auto st = prepare(legacy_db_,
        "SELECT class_id, tree_level, slot_index, skill_id FROM skill_trees");
    if (st) {
        int tree_count = 0;
        while (sqlite3_step(st) == SQLITE_ROW) {
            SkillTreeEntry e{};
            e.entry_id = static_cast<uint32_t>(tree_count + 1);
            e.class_id = Col<uint16_t>(st, 0);
            e.tree_level = Col<uint16_t>(st, 1);
            e.slot_index = Col<uint16_t>(st, 2);
            e.skill_id = Col<uint32_t>(st, 3);
            skill_trees_[e.class_id].push_back(e);
            skill_trees_by_class_[e.class_id].push_back(e);
            tree_count++;
        }
        sqlite3_finalize(st);
        spdlog::info("LoadSkillData: {} tree entries loaded", tree_count);
    }

    spdlog::info("LoadSkillData: {} skills loaded", count);
}

// ═══════════════════════════════════════════════════════════════════════
//  LoadItemTemplates  (Task D6)
// ═══════════════════════════════════════════════════════════════════════

void GameDataDB::LoadItemTemplates() {
    if (!legacy_db_) return;
    items_legacy_.clear();

    auto s = prepare(legacy_db_,
        "SELECT id, name, item_type, item_subtype, level_required, "
        "attack, defense, magic_attack, magic_defense, "
        "price_buy, price_sell, rarity, max_stack FROM item_templates");
    if (!s) { spdlog::error("LoadItemTemplates: prepare failed"); return; }

    int count = 0;
    while (sqlite3_step(s) == SQLITE_ROW) {
        ItemData item{};
        item.item_id = Col<uint32_t>(s, 0);
        item.name = col_str(s, 1);
        item.item_type = Col<uint16_t>(s, 2);
        item.item_subtype = Col<uint16_t>(s, 3);
        item.level_required = Col<uint16_t>(s, 4);
        item.attack = Col<int32_t>(s, 5);
        item.defense = Col<int32_t>(s, 6);
        item.magic_attack = Col<int32_t>(s, 7);
        item.magic_defense = Col<int32_t>(s, 8);
        item.price_buy = Col<int32_t>(s, 9);
        item.price_sell = Col<int32_t>(s, 10);
        item.rarity = Col<uint16_t>(s, 11);
        item.max_stack = Col<uint16_t>(s, 12);
        items_legacy_[item.item_id] = item;
        count++;
    }
    sqlite3_finalize(s);

    spdlog::info("LoadItemTemplates: {} items loaded", count);
}

// ═══════════════════════════════════════════════════════════════════════
//  LoadMapData  (Task D7)
// ═══════════════════════════════════════════════════════════════════════

void GameDataDB::LoadMapData() {
    if (!legacy_db_) return;
    map_data_.clear();
    map_warps_.clear();
    map_boundaries_.clear();

    // Map meta data
    auto s = prepare(legacy_db_,
        "SELECT id, name, file_path, hgt_file, "
        "box_min_x, box_min_y, box_min_z, "
        "box_max_x, box_max_y, box_max_z FROM map_data");
    if (!s) { spdlog::error("LoadMapData: prepare failed"); return; }

    while (sqlite3_step(s) == SQLITE_ROW) {
        MapData md{};
        md.map_id = Col<uint32_t>(s, 0);
        md.name = col_str(s, 1);
        md.file_path = col_str(s, 2);
        md.hgt_file = col_str(s, 3);
        md.box_min_x = Col<float>(s, 4);
        md.box_min_y = Col<float>(s, 5);
        md.box_min_z = Col<float>(s, 6);
        md.box_max_x = Col<float>(s, 7);
        md.box_max_y = Col<float>(s, 8);
        md.box_max_z = Col<float>(s, 9);
        map_data_.push_back(md);
    }
    sqlite3_finalize(s);

    // Warps
    auto sw = prepare(legacy_db_,
        "SELECT id, map_from, map_to, from_x, from_z, to_x, to_z, name, dest_name, fee "
        "FROM map_warps");
    if (sw) {
        while (sqlite3_step(sw) == SQLITE_ROW) {
            MapWarp w{};
            w.warp_id = Col<uint32_t>(sw, 0);
            w.map_from = Col<uint32_t>(sw, 1);
            w.map_to = Col<uint32_t>(sw, 2);
            w.from_x = Col<float>(sw, 3);
            w.from_z = Col<float>(sw, 4);
            w.to_x = Col<float>(sw, 5);
            w.to_z = Col<float>(sw, 6);
            w.name = col_str(sw, 7);
            w.dest_name = col_str(sw, 8);
            w.fee = Col<uint32_t>(sw, 9);
            map_warps_[w.map_from].push_back(w);
        }
        sqlite3_finalize(sw);
    }

    // Boundaries
    auto sb = prepare(legacy_db_,
        "SELECT id, map_a, map_b, boundary_x_a, boundary_z_a, "
        "boundary_x_b, boundary_z_b, name_a, name_b, level_required "
        "FROM map_boundaries");
    if (sb) {
        while (sqlite3_step(sb) == SQLITE_ROW) {
            MapBoundary mb{};
            mb.boundary_id = Col<uint32_t>(sb, 0);
            mb.map_a = Col<uint32_t>(sb, 1);
            mb.map_b = Col<uint32_t>(sb, 2);
            mb.boundary_x_a = Col<float>(sb, 3);
            mb.boundary_z_a = Col<float>(sb, 4);
            mb.boundary_x_b = Col<float>(sb, 5);
            mb.boundary_z_b = Col<float>(sb, 6);
            mb.name_a = col_str(sb, 7);
            mb.name_b = col_str(sb, 8);
            mb.level_required = Col<uint32_t>(sb, 9);
            map_boundaries_.push_back(mb);
        }
        sqlite3_finalize(sb);
    }

    // Monster spawns already loaded in LoadMonsterTemplates

    spdlog::info("LoadMapData: {} maps, {} warps, {} boundaries loaded",
        map_data_.size(), map_warps_.size(), map_boundaries_.size());
}

// ═══════════════════════════════════════════════════════════════════════
//  Legacy Accessors (in-memory)
// ═══════════════════════════════════════════════════════════════════════

const MonsterData* GameDataDB::GetMonsterLegacy(uint32_t monster_id) const {
    auto it = monsters_legacy_.find(monster_id);
    return it != monsters_legacy_.end() ? &it->second : nullptr;
}

const ItemData* GameDataDB::GetItemLegacy(uint32_t item_id) const {
    auto it = items_legacy_.find(item_id);
    return it != items_legacy_.end() ? &it->second : nullptr;
}

const SkillData* GameDataDB::GetSkillLegacy(uint32_t skill_id) const {
    auto it = skills_legacy_.find(skill_id);
    return it != skills_legacy_.end() ? &it->second : nullptr;
}

const QuestData* GameDataDB::GetQuestLegacy(uint32_t quest_id) const {
    auto it = quests_legacy_.find(quest_id);
    return it != quests_legacy_.end() ? &it->second : nullptr;
}

const NPCTemplate* GameDataDB::GetNPCTemplate(uint32_t npc_id) const {
    auto it = npc_templates_.find(npc_id);
    return it != npc_templates_.end() ? &it->second : nullptr;
}

const BuffSkill* GameDataDB::GetBuffSkill(uint32_t buff_id) const {
    auto it = buff_skills_.find(buff_id);
    return it != buff_skills_.end() ? &it->second : nullptr;
}

std::vector<DropEntry> GameDataDB::GetMonsterDrops(uint32_t monster_id) const {
    auto it = monster_drops_.find(monster_id);
    return it != monster_drops_.end() ? it->second : std::vector<DropEntry>{};
}

std::vector<NPCPosition> GameDataDB::GetNPCPositions(uint32_t map_id) const {
    auto it = npc_positions_.find(map_id);
    return it != npc_positions_.end() ? it->second : std::vector<NPCPosition>{};
}

std::vector<ShopEntry> GameDataDB::GetNPCShop(uint32_t npc_id) const {
    auto it = npc_shop_.find(npc_id);
    return it != npc_shop_.end() ? it->second : std::vector<ShopEntry>{};
}

std::vector<QuestCondition> GameDataDB::GetQuestConditions(uint32_t quest_id) const {
    auto it = quest_conditions_.find(quest_id);
    return it != quest_conditions_.end() ? it->second : std::vector<QuestCondition>{};
}

std::vector<MapWarp> GameDataDB::GetMapWarps(uint32_t map_id) const {
    auto it = map_warps_.find(map_id);
    return it != map_warps_.end() ? it->second : std::vector<MapWarp>{};
}

std::vector<MonsterSpawn> GameDataDB::GetMonsterSpawns(uint32_t map_id) const {
    auto it = monster_spawns_.find(map_id);
    return it != monster_spawns_.end() ? it->second : std::vector<MonsterSpawn>{};
}

std::vector<SkillTreeEntry> GameDataDB::GetSkillTree(uint16_t class_id) const {
    auto it = skill_trees_by_class_.find(class_id);
    return it != skill_trees_by_class_.end() ? it->second : std::vector<SkillTreeEntry>{};
}

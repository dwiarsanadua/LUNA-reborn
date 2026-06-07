#include "PersistenceManager.hpp"
#include <ui/GameState.hpp>
#include <spdlog/spdlog.h>

PersistenceManager::~PersistenceManager() { Shutdown(); }

bool PersistenceManager::Init(const std::string& db_path) {
    if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK) {
        spdlog::error("Persistence: failed to open {}", db_path);
        db_ = nullptr;
        return false;
    }
    if (!CreateTables()) {
        spdlog::error("Persistence: failed to create tables");
        return false;
    }
    spdlog::info("PersistenceManager: initialized ({})", db_path);
    return true;
}

void PersistenceManager::Shutdown() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool PersistenceManager::CreateTables() {
    const char* sql_char = R"(
        CREATE TABLE IF NOT EXISTS save_slot (
            slot INTEGER PRIMARY KEY DEFAULT 0,
            name TEXT NOT NULL DEFAULT 'Hero',
            level INTEGER DEFAULT 1,
            class_id INTEGER DEFAULT 0,
            map_id INTEGER DEFAULT 51,
            pos_x REAL DEFAULT 0,
            pos_y REAL DEFAULT 3,
            pos_z REAL DEFAULT 0,
            hp INTEGER DEFAULT 500,
            max_hp INTEGER DEFAULT 500,
            mp INTEGER DEFAULT 100,
            max_mp INTEGER DEFAULT 100,
            exp INTEGER DEFAULT 0,
            exp_next INTEGER DEFAULT 500,
            gold INTEGER DEFAULT 0,
            attack INTEGER DEFAULT 50,
            defense INTEGER DEFAULT 20,
            stat_str INTEGER DEFAULT 10,
            stat_dex INTEGER DEFAULT 10,
            stat_int INTEGER DEFAULT 10,
            stat_con INTEGER DEFAULT 10,
            skill_points INTEGER DEFAULT 5,
            monster_kills INTEGER DEFAULT 0,
            pk_kills INTEGER DEFAULT 0
        )
    )";
    const char* sql_inv = R"(
        CREATE TABLE IF NOT EXISTS inventory (
            slot INTEGER PRIMARY KEY,
            item_id INTEGER DEFAULT 0,
            name TEXT DEFAULT '',
            count INTEGER DEFAULT 1,
            enchant INTEGER DEFAULT 0
        )
    )";
    const char* sql_quest = R"(
        CREATE TABLE IF NOT EXISTS quests (
            name TEXT PRIMARY KEY,
            status TEXT DEFAULT 'active'
        )
    )";
    const char* sql_skill = R"(
        CREATE TABLE IF NOT EXISTS learned_skills (
            skill_id INTEGER PRIMARY KEY
        )
    )";

    auto exec = [this](const char* sql) -> bool {
        char* err = nullptr;
        if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK) {
            spdlog::error("Persistence SQL: {}", err ? err : "unknown");
            sqlite3_free(err);
            return false;
        }
        return true;
    };

    return exec(sql_char) && exec(sql_inv) && exec(sql_quest) && exec(sql_skill);
}

bool PersistenceManager::EnsureCharacterRow(GameState& state) {
    // Check if slot 0 exists
    sqlite3_stmt* stmt;
    const char* check = "SELECT COUNT(*) FROM save_slot WHERE slot = 0";
    if (sqlite3_prepare_v2(db_, check, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (count == 0) {
        const char* insert = "INSERT INTO save_slot (slot, name) VALUES (0, ?)";
        if (sqlite3_prepare_v2(db_, insert, -1, &stmt, nullptr) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, state.name.c_str(), -1, SQLITE_TRANSIENT);
        bool ok = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return ok;
    }
    return true;
}

bool PersistenceManager::SaveGameState(const GameState& state) {
    if (!db_) return false;
    EnsureCharacterRow(const_cast<GameState&>(state));

    const char* sql = R"(
        UPDATE save_slot SET
            name=?, level=?, class_id=?, map_id=?,
            pos_x=?, pos_y=?, pos_z=?,
            hp=?, max_hp=?, mp=?, max_mp=?,
            exp=?, exp_next=?, gold=?,
            attack=?, defense=?,
            stat_str=?, stat_dex=?, stat_int=?, stat_con=?,
            skill_points=?, monster_kills=?, pk_kills=?
        WHERE slot=0
    )";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    auto bind = [&](int idx, auto val) { sqlite3_bind_int(stmt, idx, static_cast<int>(val)); };
    sqlite3_bind_text(stmt, 1, state.name.c_str(), -1, SQLITE_TRANSIENT);
    bind(2, state.level); bind(3, state.class_id); bind(4, state.map_id);
    sqlite3_bind_double(stmt, 5, state.player_x);
    sqlite3_bind_double(stmt, 6, state.player_y);
    sqlite3_bind_double(stmt, 7, state.player_z);
    bind(8, state.hp); bind(9, state.max_hp); bind(10, state.mp); bind(11, state.max_mp);
    bind(12, state.exp); bind(13, state.exp_next); bind(14, state.gold);
    bind(15, state.attack); bind(16, state.defense);
    bind(17, state.stat_str); bind(18, state.stat_dex); bind(19, state.stat_int); bind(20, state.stat_con);
    bind(21, state.skill_points); bind(22, state.monster_kills); bind(23, state.pk_kills);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    if (!ok) return false;

    // Save inventory
    sqlite3_exec(db_, "DELETE FROM inventory", nullptr, nullptr, nullptr);
    for (size_t i = 0; i < state.inventory.size(); i++) {
        auto& item = state.inventory[i];
        const char* ins = "INSERT OR REPLACE INTO inventory (slot, item_id, name, count, enchant) VALUES (?,?,?,?,?)";
        if (sqlite3_prepare_v2(db_, ins, -1, &stmt, nullptr) != SQLITE_OK) continue;
        bind(1, i); bind(2, item.id);
        sqlite3_bind_text(stmt, 3, item.name.c_str(), -1, SQLITE_TRANSIENT);
        bind(4, item.count); bind(5, item.enchant);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // Save quests
    sqlite3_exec(db_, "DELETE FROM quests", nullptr, nullptr, nullptr);
    for (auto& q : state.quest_list) {
        const char* ins = "INSERT INTO quests (name, status) VALUES (?, 'active')";
        if (sqlite3_prepare_v2(db_, ins, -1, &stmt, nullptr) != SQLITE_OK) continue;
        sqlite3_bind_text(stmt, 1, q.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // Save learned skills
    sqlite3_exec(db_, "DELETE FROM learned_skills", nullptr, nullptr, nullptr);
    for (auto& s : state.learned_skills) {
        const char* ins = "INSERT INTO learned_skills (skill_id) VALUES (?)";
        if (sqlite3_prepare_v2(db_, ins, -1, &stmt, nullptr) != SQLITE_OK) continue;
        bind(1, s);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    dirty_ = false;
    return true;
}

bool PersistenceManager::LoadGameState(GameState& state) {
    if (!db_) return false;
    
    const char* sql = "SELECT name, level, class_id, map_id, pos_x, pos_y, pos_z, "
                       "hp, max_hp, mp, max_mp, exp, exp_next, gold, "
                       "attack, defense, stat_str, stat_dex, stat_int, stat_con, "
                       "skill_points, monster_kills, pk_kills "
                       "FROM save_slot WHERE slot=0";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return false;
    }

    auto get = [&](int idx) { return sqlite3_column_int(stmt, idx); };
    auto getS = [&](int idx) { auto s = sqlite3_column_text(stmt, idx); return s ? (const char*)s : ""; };

    state.name = getS(0);
    state.level = get(1); state.class_id = get(2); state.map_id = get(3);
    state.player_x = static_cast<float>(sqlite3_column_double(stmt, 4));
    state.player_y = static_cast<float>(sqlite3_column_double(stmt, 5));
    state.player_z = static_cast<float>(sqlite3_column_double(stmt, 6));
    state.hp = get(7); state.max_hp = get(8);
    state.mp = get(9); state.max_mp = get(10);
    state.exp = get(11); state.exp_next = get(12); state.gold = get(13);
    state.attack = get(14); state.defense = get(15);
    state.stat_str = get(16); state.stat_dex = get(17);
    state.stat_int = get(18); state.stat_con = get(19);
    state.skill_points = get(20); state.monster_kills = get(21); state.pk_kills = get(22);
    sqlite3_finalize(stmt);

    // Load inventory
    state.inventory.clear();
    const char* inv_sql = "SELECT item_id, name, count, enchant FROM inventory ORDER BY slot";
    if (sqlite3_prepare_v2(db_, inv_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            InvItem item;
            item.id = sqlite3_column_int(stmt, 0);
            if (auto s = sqlite3_column_text(stmt, 1)) item.name = (const char*)s;
            item.count = sqlite3_column_int(stmt, 2);
            item.enchant = sqlite3_column_int(stmt, 3);
            state.inventory.push_back(item);
        }
        sqlite3_finalize(stmt);
    }

    // Load quests
    state.quest_list.clear();
    const char* q_sql = "SELECT name FROM quests WHERE status='active'";
    if (sqlite3_prepare_v2(db_, q_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (auto s = sqlite3_column_text(stmt, 0))
                state.quest_list.push_back((const char*)s);
        }
        sqlite3_finalize(stmt);
    }

    // Load learned skills
    state.learned_skills.clear();
    const char* sk_sql = "SELECT skill_id FROM learned_skills";
    if (sqlite3_prepare_v2(db_, sk_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            state.learned_skills.push_back(sqlite3_column_int(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }

    spdlog::info("Persistence: loaded character '{}' (Lv.{})", state.name, state.level);
    return true;
}

bool PersistenceManager::SaveCharacter(GameState& state) { return SaveGameState(state); }
bool PersistenceManager::SaveInventory(GameState& state) { return SaveGameState(state); }
bool PersistenceManager::SaveQuest(GameState& state) { return SaveGameState(state); }

void PersistenceManager::Update(float dt) {
    if (!db_) return;
    auto_save_timer_ += dt;
    if (auto_save_timer_ >= auto_save_interval_) {
        auto_save_timer_ = 0;
        if (dirty_) {
            spdlog::info("Persistence: auto-save triggered");
            dirty_ = false;
        }
    }
}

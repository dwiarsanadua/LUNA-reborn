#include "CharacterDB.hpp"
#include <spdlog/spdlog.h>

CharacterDB::CharacterDB() = default;
CharacterDB::~CharacterDB() { if (db_) sqlite3_close(db_); }

bool CharacterDB::Init(const std::string& path) {
    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        spdlog::error("CharacterDB: failed to open {}", path);
        return false;
    }
    spdlog::info("CharacterDB: opened");
    return true;
}

std::vector<CharInfo> CharacterDB::GetCharacters(const std::string& account_id) {
    std::vector<CharInfo> chars;
    if (!db_) return chars;
    const char* sql = "SELECT character_id, account_id, name, level, class, gender, "
                       "map_id, pos_x, pos_y, pos_z, hp, max_hp, mp, max_mp "
                       "FROM characters WHERE account_id = ? AND is_deleted = 0";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return chars;
    sqlite3_bind_text(stmt, 1, account_id.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        CharInfo c;
        c.id = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
        if (auto s = sqlite3_column_text(stmt, 1)) c.account_id = reinterpret_cast<const char*>(s);
        if (auto s = sqlite3_column_text(stmt, 2)) c.name = reinterpret_cast<const char*>(s);
        c.level = static_cast<uint16_t>(sqlite3_column_int(stmt, 3));
        c.char_class = static_cast<uint16_t>(sqlite3_column_int(stmt, 4));
        c.gender = static_cast<uint8_t>(sqlite3_column_int(stmt, 5));
        c.map_id = static_cast<uint16_t>(sqlite3_column_int(stmt, 6));
        c.pos_x = static_cast<float>(sqlite3_column_double(stmt, 7));
        c.pos_y = static_cast<float>(sqlite3_column_double(stmt, 8));
        c.pos_z = static_cast<float>(sqlite3_column_double(stmt, 9));
        c.hp = sqlite3_column_int(stmt, 10);
        c.max_hp = sqlite3_column_int(stmt, 11);
        c.mp = sqlite3_column_int(stmt, 12);
        c.max_mp = sqlite3_column_int(stmt, 13);
        chars.push_back(c);
    }
    sqlite3_finalize(stmt);
    return chars;
}

CharInfo CharacterDB::GetCharacter(uint32_t char_id) {
    CharInfo c;
    if (!db_) return c;
    const char* sql = "SELECT character_id, account_id, name, level, class, gender, "
                       "map_id, pos_x, pos_y, pos_z, hp, max_hp, mp, max_mp "
                       "FROM characters WHERE character_id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return c;
    sqlite3_bind_int(stmt, 1, static_cast<int>(char_id));
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        c.id = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
        if (auto s = sqlite3_column_text(stmt, 1)) c.account_id = reinterpret_cast<const char*>(s);
        if (auto s = sqlite3_column_text(stmt, 2)) c.name = reinterpret_cast<const char*>(s);
        c.level = static_cast<uint16_t>(sqlite3_column_int(stmt, 3));
        c.char_class = static_cast<uint16_t>(sqlite3_column_int(stmt, 4));
        c.gender = static_cast<uint8_t>(sqlite3_column_int(stmt, 5));
        c.map_id = static_cast<uint16_t>(sqlite3_column_int(stmt, 6));
        c.pos_x = static_cast<float>(sqlite3_column_double(stmt, 7));
        c.pos_y = static_cast<float>(sqlite3_column_double(stmt, 8));
        c.pos_z = static_cast<float>(sqlite3_column_double(stmt, 9));
        c.hp = sqlite3_column_int(stmt, 10);
        c.max_hp = sqlite3_column_int(stmt, 11);
        c.mp = sqlite3_column_int(stmt, 12);
        c.max_mp = sqlite3_column_int(stmt, 13);
    }
    sqlite3_finalize(stmt);
    return c;
}

uint32_t CharacterDB::CreateCharacter(const CharInfo& info) {
    if (!db_) return 0;
    const char* sql = "INSERT INTO characters (account_id, name, slot, class, gender) "
                       "VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return 0;
    sqlite3_bind_text(stmt, 1, info.account_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, info.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, 0);
    sqlite3_bind_int(stmt, 4, info.char_class);
    sqlite3_bind_int(stmt, 5, info.gender);
    if (sqlite3_step(stmt) != SQLITE_DONE) { sqlite3_finalize(stmt); return 0; }
    sqlite3_finalize(stmt);
    return static_cast<uint32_t>(sqlite3_last_insert_rowid(db_));
}

bool CharacterDB::SavePosition(uint32_t char_id, float x, float y, float z) {
    if (!db_) return false;
    const char* sql = "UPDATE characters SET pos_x = ?, pos_y = ?, pos_z = ?, last_login = datetime('now') WHERE character_id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_double(stmt, 1, x);
    sqlite3_bind_double(stmt, 2, y);
    sqlite3_bind_double(stmt, 3, z);
    sqlite3_bind_int(stmt, 4, static_cast<int>(char_id));
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool CharacterDB::SaveItems(uint32_t char_id, const std::vector<CharItem>& items) {
    if (!db_) return false;
    sqlite3_exec(db_, "DELETE FROM character_inventory WHERE character_id = ?", nullptr, nullptr, nullptr);
    const char* sql = "INSERT INTO character_inventory (character_id, slot_index, item_id, count, enchant) VALUES (?,?,?,?,?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    for (auto& item : items) {
        if (item.item_id == 0) continue;
        sqlite3_bind_int(stmt, 1, static_cast<int>(char_id));
        sqlite3_bind_int(stmt, 2, static_cast<int>(item.slot));
        sqlite3_bind_int(stmt, 3, static_cast<int>(item.item_id));
        sqlite3_bind_int(stmt, 4, static_cast<int>(item.count));
        sqlite3_bind_int(stmt, 5, static_cast<int>(item.enchant));
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);
    return true;
}

std::vector<CharItem> CharacterDB::LoadItems(uint32_t char_id) {
    std::vector<CharItem> items;
    if (!db_) return items;
    const char* sql = "SELECT slot_index, item_id, count, enchant FROM character_inventory WHERE character_id = ? ORDER BY slot_index";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return items;
    sqlite3_bind_int(stmt, 1, static_cast<int>(char_id));
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        CharItem item;
        item.slot = static_cast<uint8_t>(sqlite3_column_int(stmt, 0));
        item.item_id = static_cast<uint32_t>(sqlite3_column_int(stmt, 1));
        item.count = static_cast<uint16_t>(sqlite3_column_int(stmt, 2));
        item.enchant = static_cast<uint8_t>(sqlite3_column_int(stmt, 3));
        items.push_back(item);
    }
    sqlite3_finalize(stmt);
    return items;
}

bool CharacterDB::SaveHP(uint32_t char_id, int32_t hp, int32_t mp) {
    if (!db_) return false;
    const char* sql = "UPDATE characters SET hp = ?, mp = ? WHERE character_id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, hp);
    sqlite3_bind_int(stmt, 2, mp);
    sqlite3_bind_int(stmt, 3, static_cast<int>(char_id));
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

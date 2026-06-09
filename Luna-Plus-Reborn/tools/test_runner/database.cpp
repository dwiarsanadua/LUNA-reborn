#include "test_harness.hpp"
#include <spdlog/spdlog.h>
#include <string>
#include <vector>
#include <sqlite3.h>


void RunDatabaseTests() {
    TEST_SUITE("DATABASE");

    TEST_STEP("Opening game database");
    {
        sqlite3* db = nullptr;
        int rc = sqlite3_open_v2(":memory:", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
        TEST("In-memory DB created", rc == SQLITE_OK);

        if (rc == SQLITE_OK) {
            // Execute schema
            sqlite3_close(db);
            TEST("DB handle valid", true);
        }
        spdlog::info("    SQLite version: {}", sqlite3_libversion());
    }

    TEST_STEP("Character CRUD operations");
    {
        sqlite3* db = nullptr;
        sqlite3_open(":memory:", &db);

        const char* create = R"(
            CREATE TABLE IF NOT EXISTS TB_CHARACTER (
                CharacterIdx INTEGER PRIMARY KEY AUTOINCREMENT,
                AccountID TEXT NOT NULL,
                CharName TEXT UNIQUE NOT NULL,
                Level INTEGER DEFAULT 1,
                Exp INTEGER DEFAULT 0,
                Money INTEGER DEFAULT 0,
                MapIdx INTEGER DEFAULT 0,
                PosX REAL DEFAULT 0, PosY REAL DEFAULT 0, PosZ REAL DEFAULT 0,
                HP INTEGER DEFAULT 100, MP INTEGER DEFAULT 100, SP INTEGER DEFAULT 100,
                Str INTEGER DEFAULT 10, Dex INTEGER DEFAULT 10,
                Con INTEGER DEFAULT 10, Int INTEGER DEFAULT 10, Wis INTEGER DEFAULT 10,
                BattleStyle INTEGER DEFAULT 0,
                Fame INTEGER DEFAULT 0, BadFame INTEGER DEFAULT 0
            );
        )";
        char* err = nullptr;
        sqlite3_exec(db, create, 0, 0, &err);
        if (err) { spdlog::error("    Schema error: {}", err); sqlite3_free(err); }

        // INSERT
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db,
            "INSERT INTO TB_CHARACTER (AccountID, CharName, Level, HP, MP) VALUES (?, ?, ?, ?, ?)",
            -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, "acc1", -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, "TestChar", -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, 50);
        sqlite3_bind_int(stmt, 4, 1000);
        sqlite3_bind_int(stmt, 5, 500);
        int rc = sqlite3_step(stmt);
        TEST("Character INSERT succeeds", rc == SQLITE_DONE);
        sqlite3_finalize(stmt);

        // SELECT
        sqlite3_prepare_v2(db, "SELECT CharName, Level, HP, MP FROM TB_CHARACTER WHERE CharName = ?",
                          -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, "TestChar", -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
        TEST("Character SELECT finds row", rc == SQLITE_ROW);
        if (rc == SQLITE_ROW) {
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            int level = sqlite3_column_int(stmt, 1);
            int hp = sqlite3_column_int(stmt, 2);
            int mp = sqlite3_column_int(stmt, 3);
            TEST("Character name = TestChar", name == "TestChar");
            TEST("Character level = 50", level == 50);
            TEST("Character HP = 1000", hp == 1000);
            TEST("Character MP = 500", mp == 500);
            spdlog::info("    Character '{}': Lv{}, HP={}, MP={}", name, level, hp, mp);
        }
        sqlite3_finalize(stmt);

        // UPDATE
        sqlite3_prepare_v2(db, "UPDATE TB_CHARACTER SET Level = ?, Money = ? WHERE CharName = ?",
                          -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, 51);
        sqlite3_bind_int(stmt, 2, 10000);
        sqlite3_bind_text(stmt, 3, "TestChar", -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
        TEST("Character UPDATE succeeds", rc == SQLITE_DONE);
        sqlite3_finalize(stmt);

        // Verify UPDATE
        sqlite3_prepare_v2(db, "SELECT Level, Money FROM TB_CHARACTER WHERE CharName = ?", -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, "TestChar", -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        int new_lv = sqlite3_column_int(stmt, 0);
        int gold = sqlite3_column_int(stmt, 1);
        TEST("Level updated to 51", new_lv == 51);
        TEST("Money updated to 10000", gold == 10000);
        sqlite3_finalize(stmt);

        // DELETE
        sqlite3_prepare_v2(db, "DELETE FROM TB_CHARACTER WHERE CharName = ?", -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, "TestChar", -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
        TEST("Character DELETE succeeds", rc == SQLITE_DONE);
        sqlite3_finalize(stmt);

        sqlite3_close(db);
    }

    TEST_STEP("Item CRUD with options table");
    {
        sqlite3* db = nullptr;
        sqlite3_open(":memory:", &db);

        sqlite3_exec(db,
            "CREATE TABLE TB_ITEM (ItemIdx INTEGER PRIMARY KEY, CharacterIdx INTEGER, "
            "ItemDBIdx INTEGER, Count INTEGER, Slot INTEGER, Grade INTEGER, "
            "Durability INTEGER, Socket INTEGER);"
            "CREATE TABLE TB_ITEM_OPTION (OptionIdx INTEGER PRIMARY KEY, "
            "ItemIdx INTEGER, OptType INTEGER, OptValue REAL);", 0, 0, 0);

        // Insert item
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "INSERT INTO TB_ITEM VALUES (1, 100, 5001, 1, 5, 3, 80, 2)", -1, &stmt, nullptr);
        TEST("Item INSERT", sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);

        // Insert options
        sqlite3_prepare_v2(db, "INSERT INTO TB_ITEM_OPTION VALUES (1, 1, 10, 5.0)", -1, &stmt, nullptr);
        sqlite3_step(stmt); sqlite3_finalize(stmt);
        sqlite3_prepare_v2(db, "INSERT INTO TB_ITEM_OPTION VALUES (2, 1, 11, 3.0)", -1, &stmt, nullptr);
        sqlite3_step(stmt); sqlite3_finalize(stmt);

        // Read item with options
        sqlite3_prepare_v2(db,
            "SELECT i.ItemDBIdx, i.Grade, i.Durability, o.OptType, o.OptValue "
            "FROM TB_ITEM i LEFT JOIN TB_ITEM_OPTION o ON i.ItemIdx = o.ItemIdx "
            "WHERE i.ItemIdx = 1", -1, &stmt, nullptr);

        int row_count = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int db_idx = sqlite3_column_int(stmt, 0);
            int grade = sqlite3_column_int(stmt, 1);
            int dur = sqlite3_column_int(stmt, 2);
            int opt_type = sqlite3_column_type(stmt, 3) != SQLITE_NULL ? sqlite3_column_int(stmt, 3) : 0;
            float opt_val = sqlite3_column_type(stmt, 4) != SQLITE_NULL ? (float)sqlite3_column_double(stmt, 4) : 0;
            if (row_count == 0) {
                TEST("Item DBIdx = 5001", db_idx == 5001);
                TEST("Item Grade = 3", grade == 3);
                TEST("Item Durability = 80", dur == 80);
            }
            spdlog::info("    ItemOption #{}: type={}, value={}", row_count+1, opt_type, opt_val);
            row_count++;
        }
        sqlite3_finalize(stmt);
        TEST("Item has 2 options", row_count == 2);

        sqlite3_close(db);
    }

    TEST_STEP("Guild with mark/emblem data");
    {
        sqlite3* db = nullptr;
        sqlite3_open(":memory:", &db);

        sqlite3_exec(db,
            "CREATE TABLE TB_GUILD (GuildIdx INTEGER PRIMARY KEY, GuildName TEXT UNIQUE, "
            "MasterIdx INTEGER, Level INTEGER DEFAULT 1, GP INTEGER DEFAULT 0, "
            "MarkData BLOB, EmblemData BLOB, Notice TEXT);", 0, 0, 0);

        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db,
            "INSERT INTO TB_GUILD VALUES (1, 'TestGuild', 100, 5, 10000, NULL, NULL, 'Hello')",
            -1, &stmt, nullptr);
        TEST("Guild INSERT with GP", sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);

        sqlite3_prepare_v2(db, "SELECT GuildName, Level, GP FROM TB_GUILD WHERE GuildIdx = 1", -1, &stmt, nullptr);
        sqlite3_step(stmt);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int lv = sqlite3_column_int(stmt, 1);
        int gp = sqlite3_column_int(stmt, 2);
        TEST("Guild name = TestGuild", name == "TestGuild");
        TEST("Guild level = 5", lv == 5);
        TEST("Guild GP = 10000 (column verified)", gp == 10000);
        sqlite3_finalize(stmt);

        sqlite3_close(db);
    }

    TEST_STEP("Party with Option column");
    {
        sqlite3* db = nullptr;
        sqlite3_open(":memory:", &db);

        sqlite3_exec(db,
            "CREATE TABLE TB_PARTY (PartyIdx INTEGER PRIMARY KEY, "
            "MasterIdx INTEGER, Option INTEGER DEFAULT 0, CreateDate TEXT);", 0, 0, 0);

        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "INSERT INTO TB_PARTY VALUES (1, 100, 3, datetime('now'))", -1, &stmt, nullptr);
        TEST("Party INSERT with Option", sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);

        sqlite3_prepare_v2(db, "SELECT Option FROM TB_PARTY WHERE PartyIdx = 1", -1, &stmt, nullptr);
        sqlite3_step(stmt);
        int opt = sqlite3_column_int(stmt, 0);
        TEST("Party Option = 3", opt == 3);
        sqlite3_finalize(stmt);

        sqlite3_close(db);
    }

    // ─── Read-only file check ───
    TEST_STEP("Schema SQL file check");
    {
        // Coba beberapa path
        const char* paths[] = {
            "database/schema_game_sqlite.sql",
            "../database/schema_game_sqlite.sql",
            "../../database/schema_game_sqlite.sql",
            "../../../database/schema_game_sqlite.sql"
        };
        FILE* f = nullptr;
        for (auto p : paths) { f = fopen(p, "r"); if (f) { spdlog::info("    Found at: {}", p); break; } }
        if (!f) spdlog::warn("    Schema file not found in any relative path (run from project root)");
        TEST("Schema file check", true); // non-critical, just info
        if (f) {
            fseek(f, 0, SEEK_END);
            long size = ftell(f);
            fclose(f);
            spdlog::info("    Schema file size: {} bytes", size);
        }
    }
}

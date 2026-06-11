#include "SQLiteDatabase.h"
#include <spdlog/spdlog.h>
#include <cstring>

SQLiteDatabase::SQLiteDatabase() = default;

SQLiteDatabase::~SQLiteDatabase() {
    Disconnect();
}

bool SQLiteDatabase::Connect(const std::string& connection_string) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (db_) {
        spdlog::warn("SQLiteDatabase: Already connected");
        return true;
    }
    int rc = sqlite3_open(connection_string.c_str(), &db_);
    if (rc != SQLITE_OK) {
        spdlog::error("SQLiteDatabase: Failed to open '{}': {}", connection_string, sqlite3_errmsg(db_));
        sqlite3_close(db_);
        db_ = nullptr;
        return false;
    }
    sqlite3_exec(db_, "PRAGMA journal_mode=WAL", nullptr, nullptr, nullptr);
    sqlite3_exec(db_, "PRAGMA foreign_keys=ON", nullptr, nullptr, nullptr);
    spdlog::info("SQLiteDatabase: Connected to '{}'", connection_string);
    return true;
}

void SQLiteDatabase::Disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
        in_transaction_ = false;
        spdlog::info("SQLiteDatabase: Disconnected");
    }
}

bool SQLiteDatabase::IsConnected() const {
    return db_ != nullptr;
}

QueryResult SQLiteDatabase::Execute(const std::string& query) {
    std::lock_guard<std::mutex> lock(mutex_);
    return ExecuteInternal(query);
}

QueryResult SQLiteDatabase::ExecuteParams(const std::string& query,
                                            const std::vector<std::string>& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!db_) {
        QueryResult result;
        result.success = false;
        result.error_message = "Not connected";
        return result;
    }

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        QueryResult result;
        result.success = false;
        result.error_message = sqlite3_errmsg(db_);
        return result;
    }

    for (size_t i = 0; i < params.size(); ++i) {
        sqlite3_bind_text(stmt, (int)(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT);
    }

    QueryResult result;
    result.success = true;
    int col_count = sqlite3_column_count(stmt);
    for (int i = 0; i < col_count; ++i) {
        result.columns.push_back(sqlite3_column_name(stmt, i));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::vector<std::string> row;
        for (int i = 0; i < col_count; ++i) {
            const char* val = (const char*)sqlite3_column_text(stmt, i);
            row.push_back(val ? val : "");
        }
        result.rows.push_back(std::move(row));
    }

    result.affected_rows = sqlite3_changes(db_);
    result.last_insert_id = sqlite3_last_insert_rowid(db_);
    sqlite3_finalize(stmt);
    return result;
}

bool SQLiteDatabase::BeginTransaction() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (in_transaction_) return true;
    auto r = ExecuteInternal("BEGIN TRANSACTION");
    in_transaction_ = r.success;
    return r.success;
}

bool SQLiteDatabase::Commit() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!in_transaction_) return true;
    auto r = ExecuteInternal("COMMIT");
    in_transaction_ = !r.success;
    return r.success;
}

bool SQLiteDatabase::Rollback() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!in_transaction_) return true;
    auto r = ExecuteInternal("ROLLBACK");
    in_transaction_ = !r.success;
    return r.success;
}

QueryResult SQLiteDatabase::ExecuteInternal(const std::string& query) {
    QueryResult result;
    if (!db_) {
        result.success = false;
        result.error_message = "Not connected";
        return result;
    }

    char* err = nullptr;
    auto rc = sqlite3_exec(db_, query.c_str(),
        [](void* data, int cols, char** values, char** names) -> int {
            auto* res = static_cast<QueryResult*>(data);
            if (res->columns.empty()) {
                for (int i = 0; i < cols; ++i)
                    res->columns.push_back(names[i] ? names[i] : "");
            }
            std::vector<std::string> row;
            for (int i = 0; i < cols; ++i)
                row.push_back(values[i] ? values[i] : "");
            res->rows.push_back(std::move(row));
            return 0;
        }, &result, &err);

    result.success = (rc == SQLITE_OK);
    result.affected_rows = sqlite3_changes(db_);
    result.last_insert_id = sqlite3_last_insert_rowid(db_);

    if (err) {
        result.error_message = err;
        sqlite3_free(err);
    }
    return result;
}

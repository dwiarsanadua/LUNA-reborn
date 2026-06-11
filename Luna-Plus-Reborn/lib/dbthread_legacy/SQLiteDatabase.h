#pragma once
#include "IDatabase.h"
#include <sqlite3.h>
#include <mutex>

class SQLiteDatabase : public IDatabase {
public:
    SQLiteDatabase();
    ~SQLiteDatabase() override;

    bool Connect(const std::string& connection_string) override;
    void Disconnect() override;
    bool IsConnected() const override;

    QueryResult Execute(const std::string& query) override;
    QueryResult ExecuteParams(const std::string& query,
                               const std::vector<std::string>& params) override;

    bool BeginTransaction() override;
    bool Commit() override;
    bool Rollback() override;

private:
    QueryResult ExecuteInternal(const std::string& query);

    sqlite3* db_ = nullptr;
    bool in_transaction_ = false;
    mutable std::mutex mutex_;
};

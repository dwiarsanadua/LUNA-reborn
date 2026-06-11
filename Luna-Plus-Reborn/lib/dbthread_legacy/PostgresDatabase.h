#pragma once
#include "IDatabase.h"
#include <mutex>

struct pg_conn;
struct pg_result;

class PostgresDatabase : public IDatabase {
public:
    PostgresDatabase();
    ~PostgresDatabase() override;

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

    pg_conn* conn_ = nullptr;
    bool in_transaction_ = false;
    mutable std::mutex mutex_;
};

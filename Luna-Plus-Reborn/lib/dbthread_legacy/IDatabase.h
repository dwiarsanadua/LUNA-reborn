#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>

struct QueryResult {
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> rows;
    int64_t affected_rows = 0;
    int64_t last_insert_id = 0;
    bool success = false;
    std::string error_message;

    int RowCount() const { return (int)rows.size(); }
    int ColCount() const { return (int)columns.size(); }

    std::string GetString(int row, int col) const {
        if (row < RowCount() && col < ColCount())
            return rows[row][col];
        return {};
    }

    int GetInt(int row, int col) const {
        auto val = GetString(row, col);
        return val.empty() ? 0 : std::stoi(val);
    }

    int64_t GetInt64(int row, int col) const {
        auto val = GetString(row, col);
        return val.empty() ? 0 : std::stoll(val);
    }

    float GetFloat(int row, int col) const {
        auto val = GetString(row, col);
        return val.empty() ? 0.0f : std::stof(val);
    }
};

class IDatabase {
public:
    virtual ~IDatabase() = default;

    virtual bool Connect(const std::string& connection_string) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;

    virtual QueryResult Execute(const std::string& query) = 0;
    virtual QueryResult ExecuteParams(const std::string& query,
                                       const std::vector<std::string>& params) = 0;

    virtual bool BeginTransaction() = 0;
    virtual bool Commit() = 0;
    virtual bool Rollback() = 0;
};

enum class DatabaseType { SQLite, PostgreSQL };

std::unique_ptr<IDatabase> CreateDatabase(DatabaseType type);

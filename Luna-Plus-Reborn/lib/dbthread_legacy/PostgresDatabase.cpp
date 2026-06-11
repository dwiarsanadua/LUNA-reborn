#include "PostgresDatabase.h"
#include <libpq-fe.h>
#include <spdlog/spdlog.h>

PostgresDatabase::PostgresDatabase() = default;

PostgresDatabase::~PostgresDatabase() {
    Disconnect();
}

bool PostgresDatabase::Connect(const std::string& connection_string) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (conn_) {
        spdlog::warn("PostgresDatabase: Already connected");
        return true;
    }
    conn_ = PQconnectdb(connection_string.c_str());
    if (PQstatus(conn_) != CONNECTION_OK) {
        spdlog::error("PostgresDatabase: Connection failed: {}", PQerrorMessage(conn_));
        PQfinish(conn_);
        conn_ = nullptr;
        return false;
    }
    spdlog::info("PostgresDatabase: Connected successfully");
    return true;
}

void PostgresDatabase::Disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (conn_) {
        PQfinish(conn_);
        conn_ = nullptr;
        in_transaction_ = false;
        spdlog::info("PostgresDatabase: Disconnected");
    }
}

bool PostgresDatabase::IsConnected() const {
    return conn_ && PQstatus(conn_) == CONNECTION_OK;
}

static QueryResult ResultFromPGResult(pg_result* res) {
    QueryResult result;
    if (!res) {
        result.success = false;
        return result;
    }

    int nrows = PQntuples(res);
    int ncols = PQnfields(res);

    for (int i = 0; i < ncols; ++i)
        result.columns.push_back(PQfname(res, i));

    for (int i = 0; i < nrows; ++i) {
        std::vector<std::string> row;
        for (int j = 0; j < ncols; ++j) {
            if (PQgetisnull(res, i, j))
                row.push_back("");
            else
                row.push_back(PQgetvalue(res, i, j));
        }
        result.rows.push_back(std::move(row));
    }

    result.success = true;
    result.affected_rows = (const char*)PQcmdTuples(res) ? atol(PQcmdTuples(res)) : 0;
    PQclear(res);
    return result;
}

QueryResult PostgresDatabase::Execute(const std::string& query) {
    std::lock_guard<std::mutex> lock(mutex_);
    return ExecuteInternal(query);
}

QueryResult PostgresDatabase::ExecuteParams(const std::string& query,
                                              const std::vector<std::string>& params) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!conn_ || PQstatus(conn_) != CONNECTION_OK) {
        QueryResult result;
        result.success = false;
        result.error_message = "Not connected";
        return result;
    }

    std::vector<const char*> param_values;
    for (const auto& p : params)
        param_values.push_back(p.c_str());

    auto* res = PQexecParams(conn_, query.c_str(),
        (int)params.size(), nullptr, param_values.data(),
        nullptr, nullptr, 0);

    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
        QueryResult result;
        result.success = false;
        result.error_message = PQerrorMessage(conn_);
        if (res) PQclear(res);
        return result;
    }

    return ResultFromPGResult(res);
}

bool PostgresDatabase::BeginTransaction() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (in_transaction_) return true;
    auto r = ExecuteInternal("BEGIN");
    in_transaction_ = r.success;
    return r.success;
}

bool PostgresDatabase::Commit() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!in_transaction_) return true;
    auto r = ExecuteInternal("COMMIT");
    in_transaction_ = !r.success;
    return r.success;
}

bool PostgresDatabase::Rollback() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!in_transaction_) return true;
    auto r = ExecuteInternal("ROLLBACK");
    in_transaction_ = !r.success;
    return r.success;
}

QueryResult PostgresDatabase::ExecuteInternal(const std::string& query) {
    if (!conn_ || PQstatus(conn_) != CONNECTION_OK) {
        QueryResult result;
        result.success = false;
        result.error_message = "Not connected";
        return result;
    }

    auto* res = PQexec(conn_, query.c_str());
    if (!res) {
        QueryResult result;
        result.success = false;
        result.error_message = PQerrorMessage(conn_);
        return result;
    }

    ExecStatusType status = PQresultStatus(res);
    if (status == PGRES_TUPLES_OK || status == PGRES_COMMAND_OK) {
        return ResultFromPGResult(res);
    }

    QueryResult result;
    result.success = false;
    result.error_message = PQresultErrorMessage(res);
    PQclear(res);
    return result;
}

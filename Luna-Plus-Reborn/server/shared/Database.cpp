#include "Database.h"
#include <sqlite3.h>
#include <spdlog/spdlog.h>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <map>

struct StmtHandle {
    sqlite3_stmt* stmt = nullptr;
    sqlite3* db = nullptr;
    bool finalized = false;
};

class Database::Impl {
public:
    static constexpr int POOL_SIZE = 4;
    sqlite3* db_pool_[POOL_SIZE];
    bool db_used_[POOL_SIZE] = {};
    std::mutex pool_mutex_;
    std::mutex exec_mutex_;
    std::string db_path_;

    // Async write queue
    struct WriteJob {
        std::string sql;
        std::function<void(bool)> callback;
    };
    std::queue<WriteJob> write_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::thread worker_thread_;
    std::atomic<bool> running_{false};

    bool Init(const std::string& path) {
        db_path_ = path;
        for (int i = 0; i < POOL_SIZE; i++) {
            int rc = sqlite3_open_v2(path.c_str(), &db_pool_[i],
                SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
            if (rc != SQLITE_OK) {
                spdlog::error("Database: failed to open connection {} - {}", i, sqlite3_errmsg(db_pool_[i]));
                return false;
            }
            // Enable WAL mode for concurrency
            char* err = nullptr;
            sqlite3_exec(db_pool_[i], "PRAGMA journal_mode=WAL; PRAGMA synchronous=NORMAL;", nullptr, nullptr, &err);
            if (err) {
                spdlog::warn("Database: pragma error - {}", err);
                sqlite3_free(err);
            }
            db_used_[i] = false;
        }

        running_ = true;
        worker_thread_ = std::thread(&Impl::WorkerLoop, this);

        spdlog::info("Database: initialized with {} connections (WAL mode)", POOL_SIZE);
        return true;
    }

    void Shutdown() {
        running_ = false;
        queue_cv_.notify_all();
        if (worker_thread_.joinable()) worker_thread_.join();

        for (int i = 0; i < POOL_SIZE; i++) {
            if (db_pool_[i]) {
                sqlite3_close(db_pool_[i]);
                db_pool_[i] = nullptr;
            }
        }
        spdlog::info("Database: shutdown");
    }

    sqlite3* GetDb() {
        std::lock_guard<std::mutex> lock(pool_mutex_);
        for (int i = 0; i < POOL_SIZE; i++) {
            if (!db_used_[i]) {
                db_used_[i] = true;
                return db_pool_[i];
            }
        }
        // Fallback: use connection 0
        spdlog::warn("Database: all connections busy, using connection 0");
        return db_pool_[0];
    }

    void ReleaseDb(sqlite3* db) {
        std::lock_guard<std::mutex> lock(pool_mutex_);
        for (int i = 0; i < POOL_SIZE; i++) {
            if (db_pool_[i] == db) {
                db_used_[i] = false;
                return;
            }
        }
    }

    bool Exec(const std::string& sql) {
        std::lock_guard<std::mutex> lock(exec_mutex_);
        char* err = nullptr;
        int rc = sqlite3_exec(GetDb(), sql.c_str(), nullptr, nullptr, &err);
        if (rc != SQLITE_OK) {
            spdlog::error("Database: exec error - {}", err ? err : "unknown");
            sqlite3_free(err);
            ReleaseDb(GetDb());
            return false;
        }
        ReleaseDb(GetDb());
        return true;
    }

    std::vector<std::vector<std::string>> Qry(const std::string& sql) {
        std::lock_guard<std::mutex> lock(exec_mutex_);
        std::vector<std::vector<std::string>> results;
        sqlite3* db = GetDb();
        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            spdlog::error("Database: query prepare error - {}", sqlite3_errmsg(db));
            ReleaseDb(db);
            return results;
        }

        int cols = sqlite3_column_count(stmt);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::vector<std::string> row;
            for (int i = 0; i < cols; i++) {
                const char* val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                row.push_back(val ? val : "");
            }
            results.push_back(std::move(row));
        }

        sqlite3_finalize(stmt);
        ReleaseDb(db);
        return results;
    }

    int Prep(const std::string& sql) {
        sqlite3* db = GetDb();
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            spdlog::error("Database: prepare error - {}", sqlite3_errmsg(db));
            ReleaseDb(db);
            return -1;
        }

        auto handle = new StmtHandle{stmt, db, false};
        return static_cast<int>(reinterpret_cast<intptr_t>(handle));
    }

    void Bind(const std::string& sql, int stmt_handle, int index, int value) {
        auto handle = reinterpret_cast<StmtHandle*>(stmt_handle);
        if (!handle || handle->finalized) return;
        sqlite3_bind_int(handle->stmt, index, value);
    }

    void BindText(int stmt_handle, int index, const std::string& value) {
        auto handle = reinterpret_cast<StmtHandle*>(stmt_handle);
        if (!handle || handle->finalized) return;
        sqlite3_bind_text(handle->stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
    }

    bool Step(int stmt_handle) {
        auto handle = reinterpret_cast<StmtHandle*>(stmt_handle);
        if (!handle || handle->finalized) return false;
        int rc = sqlite3_step(handle->stmt);
        if (rc == SQLITE_ROW) return true;
        return false;
    }

    int GetColInt(int stmt_handle, int col) {
        auto handle = reinterpret_cast<StmtHandle*>(stmt_handle);
        if (!handle || handle->finalized) return 0;
        return sqlite3_column_int(handle->stmt, col);
    }

    std::string GetColText(int stmt_handle, int col) {
        auto handle = reinterpret_cast<StmtHandle*>(stmt_handle);
        if (!handle || handle->finalized) return "";
        const char* val = reinterpret_cast<const char*>(sqlite3_column_text(handle->stmt, col));
        return val ? val : "";
    }

    void Final(int stmt_handle) {
        auto handle = reinterpret_cast<StmtHandle*>(stmt_handle);
        if (!handle || handle->finalized) return;
        handle->finalized = true;
        sqlite3_finalize(handle->stmt);
        ReleaseDb(handle->db);
        delete handle;
    }

    void PushWrite(const std::string& sql, std::function<void(bool)> cb) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            write_queue_.push({sql, std::move(cb)});
        }
        queue_cv_.notify_one();
    }

    void WorkerLoop() {
        while (running_) {
            WriteJob job;
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                queue_cv_.wait_for(lock, std::chrono::milliseconds(100), [this]() {
                    return !write_queue_.empty() || !running_;
                });
                if (!running_ && write_queue_.empty()) return;
                if (write_queue_.empty()) continue;
                job = std::move(write_queue_.front());
                write_queue_.pop();
            }

            bool ok = Exec(job.sql);
            if (job.callback) job.callback(ok);
        }
    }
};

Database::Database() : impl_(std::make_unique<Impl>()) {}
Database::~Database() { Shutdown(); }

bool Database::Initialize(const std::string& path) { return impl_->Init(path); }
void Database::Shutdown() { impl_->Shutdown(); }
bool Database::Execute(const std::string& sql) { return impl_->Exec(sql); }
std::vector<std::vector<std::string>> Database::Query(const std::string& sql) { return impl_->Qry(sql); }
int Database::Prepare(const std::string& sql) { return impl_->Prep(sql); }
void Database::BindInt(int stmt, int index, int value) { impl_->Bind("", stmt, index, value); }
void Database::BindText(int stmt, int index, const std::string& value) { impl_->BindText(stmt, index, value); }
bool Database::Step(int stmt) { return impl_->Step(stmt); }
int Database::GetColumnInt(int stmt, int col) { return impl_->GetColInt(stmt, col); }
std::string Database::GetColumnText(int stmt, int col) { return impl_->GetColText(stmt, col); }
void Database::Finalize(int stmt) { impl_->Final(stmt); }

#include "IDatabase.h"
#include "SQLiteDatabase.h"
#include "PostgresDatabase.h"

std::unique_ptr<IDatabase> CreateDatabase(DatabaseType type) {
    switch (type) {
        case DatabaseType::SQLite:
            return std::make_unique<SQLiteDatabase>();
        case DatabaseType::PostgreSQL:
            return std::make_unique<PostgresDatabase>();
        default:
            return nullptr;
    }
}

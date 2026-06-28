#pragma once

#include <QString>

namespace RetailMS {

class DatabaseManager;

class MigrationRunner {
public:
    explicit MigrationRunner(DatabaseManager& db);
    bool runMigrations(const QString& migrationsDir);

private:
    DatabaseManager& m_db;
    void ensureSchemaTable();
    int getAppliedVersion();
    void setAppliedVersion(int version);
};

} // namespace RetailMS

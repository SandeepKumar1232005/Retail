#pragma once

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariantList>
#include <optional>

namespace RetailMS {

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool connect(const QString& dbPath);
    void disconnect();
    bool isConnected() const;

    QSqlQuery execute(const QString& sql);
    QSqlQuery prepare(const QString& sql);
    bool executeNonQuery(const QString& sql, const QVariantList& params = {});
    std::optional<QVariant> executeScalar(const QString& sql, const QVariantList& params = {});

    bool beginTransaction();
    bool commit();
    bool rollback();

    void runMigrations(const QString& migrationsDir);
    int currentSchemaVersion() const;

    QString lastError() const;

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_db;
};

} // namespace RetailMS

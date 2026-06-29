#include "DatabaseManager.h"
#include "exceptions/AppException.h"
#include "utils/Logger.h"
#include "migrations/MigrationRunner.h"
#include <QSqlError>
#include <QDir>
#include <QFileInfo>

namespace RetailMS {

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    }
}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

bool DatabaseManager::connect(const QString& dbPath) {
    if (m_db.isOpen()) {
        if (m_db.databaseName() == dbPath) return true;
        m_db.close();
    }
    
    m_db.setDatabaseName(dbPath);
    if (!m_db.open()) {
        LOG_ERROR(QString("Failed to open database: %1").arg(m_db.lastError().text()));
        return false;
    }
    
    LOG_INFO(QString("Connected to database: %1").arg(dbPath));
    
    // Enable foreign keys
    executeNonQuery("PRAGMA foreign_keys = ON;");
    
    return true;
}

void DatabaseManager::disconnect() {
    if (m_db.isOpen()) {
        m_db.close();
        LOG_INFO("Disconnected from database.");
    }
}

bool DatabaseManager::isConnected() const {
    return m_db.isOpen();
}

QSqlQuery DatabaseManager::execute(const QString& sql) {
    if (!isConnected()) {
        throw DatabaseException("Database is not connected");
    }
    QSqlQuery query(m_db);
    if (!query.exec(sql)) {
        LOG_ERROR(QString("SQL Error: %1 \nQuery: %2").arg(query.lastError().text(), sql));
        throw DatabaseException(query.lastError().text(), "ERR_SQL_EXEC");
    }
    return query;
}

QSqlQuery DatabaseManager::prepare(const QString& sql) {
    if (!isConnected()) {
        throw DatabaseException("Database is not connected");
    }
    QSqlQuery query(m_db);
    if (!query.prepare(sql)) {
        LOG_ERROR(QString("SQL Prepare Error: %1 \nQuery: %2").arg(query.lastError().text(), sql));
        throw DatabaseException(query.lastError().text(), "ERR_SQL_PREPARE");
    }
    return query;
}

bool DatabaseManager::executeNonQuery(const QString& sql, const QVariantList& params) {
    if (!isConnected()) return false;
    
    QSqlQuery query = prepare(sql);
    for (const auto& param : params) {
        query.addBindValue(param);
    }
    
    if (!query.exec()) {
        LOG_ERROR(QString("SQL Error: %1 \nQuery: %2").arg(query.lastError().text(), sql));
        return false;
    }
    return true;
}

std::optional<QVariant> DatabaseManager::executeScalar(const QString& sql, const QVariantList& params) {
    if (!isConnected()) return std::nullopt;
    
    QSqlQuery query = prepare(sql);
    for (const auto& param : params) {
        query.addBindValue(param);
    }
    
    if (query.exec() && query.next()) {
        return query.value(0);
    }
    return std::nullopt;
}

bool DatabaseManager::beginTransaction() {
    if (!isConnected()) return false;
    if (!m_db.transaction()) {
        LOG_ERROR("Failed to begin transaction: " + lastError());
        return false;
    }
    return true;
}

bool DatabaseManager::commit() {
    if (!isConnected()) return false;
    if (!m_db.commit()) {
        LOG_ERROR("Failed to commit transaction: " + lastError());
        return false;
    }
    return true;
}

bool DatabaseManager::rollback() {
    if (!isConnected()) return false;
    if (!m_db.rollback()) {
        LOG_ERROR("Failed to rollback transaction: " + lastError());
        return false;
    }
    return true;
}

void DatabaseManager::runMigrations(const QString& migrationsDir) {
    LOG_INFO("Running migrations from: " + migrationsDir);
    MigrationRunner runner(*this);
    if (!runner.runMigrations(migrationsDir)) {
        LOG_ERROR("Failed to apply migrations!");
    }
}

int DatabaseManager::currentSchemaVersion() const {
    // Retrieve schema version from a hypothetical `schema_info` table or `PRAGMA user_version`
    auto result = const_cast<DatabaseManager*>(this)->executeScalar("PRAGMA user_version;");
    return result ? result->toInt() : 0;
}

QString DatabaseManager::lastError() const {
    return m_db.lastError().text();
}

} // namespace RetailMS

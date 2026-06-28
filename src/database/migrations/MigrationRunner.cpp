#include "MigrationRunner.h"
#include "../DatabaseManager.h"
#include "../Transaction.h"
#include "../../utils/Logger.h"
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QFile>

namespace RetailMS {

MigrationRunner::MigrationRunner(DatabaseManager& db) : m_db(db) {}

void MigrationRunner::ensureSchemaTable() {
    m_db.executeNonQuery("CREATE TABLE IF NOT EXISTS schema_migrations (version INTEGER PRIMARY KEY);");
}

int MigrationRunner::getAppliedVersion() {
    auto result = m_db.executeScalar("SELECT MAX(version) FROM schema_migrations;");
    if (result && !result->isNull()) {
        return result->toInt();
    }
    return 0;
}

void MigrationRunner::setAppliedVersion(int version) {
    m_db.executeNonQuery("INSERT INTO schema_migrations (version) VALUES (?);", {version});
}

bool MigrationRunner::runMigrations(const QString& migrationsDir) {
    ensureSchemaTable();
    int currentVersion = getAppliedVersion();
    
    QDir dir(migrationsDir);
    if (!dir.exists()) {
        LOG_WARN("Migrations directory does not exist: " + migrationsDir);
        return false;
    }

    dir.setNameFilters({"Migration_*.sql"});
    dir.setSorting(QDir::Name);
    QFileInfoList list = dir.entryInfoList();

    QRegularExpression re("Migration_(\\d+)_.*\\.sql");

    for (const QFileInfo& fileInfo : list) {
        QRegularExpressionMatch match = re.match(fileInfo.fileName());
        if (match.hasMatch()) {
            int fileVersion = match.captured(1).toInt();
            if (fileVersion > currentVersion) {
                LOG_INFO("Applying migration: " + fileInfo.fileName());
                
                QFile file(fileInfo.absoluteFilePath());
                if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    LOG_ERROR("Failed to open migration file: " + fileInfo.fileName());
                    return false;
                }
                QString sql = file.readAll();
                file.close();

                QStringList statements = sql.split(';', Qt::SkipEmptyParts);

                Transaction tx(m_db);
                try {
                    for (const QString& stmt : statements) {
                        QString trimmed = stmt.trimmed();
                        if (!trimmed.isEmpty()) {
                            m_db.execute(trimmed);
                        }
                    }
                    setAppliedVersion(fileVersion);
                    tx.commit();
                    LOG_INFO("Successfully applied migration: " + fileInfo.fileName());
                } catch (const std::exception& e) {
                    LOG_ERROR(QString("Migration failed %1: %2").arg(fileInfo.fileName(), e.what()));
                    return false; // Transaction automatically rolls back
                }
            }
        }
    }
    return true;
}

} // namespace RetailMS

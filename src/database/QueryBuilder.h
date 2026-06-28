#pragma once
#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <QSqlQuery>

namespace RetailMS {

class DatabaseManager;

class QueryBuilder {
public:
    explicit QueryBuilder(DatabaseManager& db);

    QueryBuilder& table(const QString& name);
    QueryBuilder& select(const QStringList& columns = {"*"});
    QueryBuilder& where(const QString& field, const QVariant& value, const QString& op = "=");
    QueryBuilder& whereRaw(const QString& condition, const QVariantList& params = {});
    QueryBuilder& orderBy(const QString& field, const QString& direction = "ASC");
    QueryBuilder& limit(int count);
    QueryBuilder& offset(int count);

    QSqlQuery get();
    int insert(const QVariantMap& data);
    bool update(const QVariantMap& data);
    bool remove();

private:
    DatabaseManager& m_db;
    QString m_table;
    QStringList m_selects;
    QStringList m_wheres;
    QVariantList m_whereParams;
    QString m_orderBy;
    int m_limit{-1};
    int m_offset{-1};
};

} // namespace RetailMS

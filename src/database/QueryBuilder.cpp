#include "QueryBuilder.h"
#include "DatabaseManager.h"
#include "../utils/Logger.h"

namespace RetailMS {

QueryBuilder::QueryBuilder(DatabaseManager& db) : m_db(db) {}

QueryBuilder& QueryBuilder::table(const QString& name) {
    m_table = name;
    return *this;
}

QueryBuilder& QueryBuilder::select(const QStringList& columns) {
    m_selects = columns;
    return *this;
}

QueryBuilder& QueryBuilder::where(const QString& field, const QVariant& value, const QString& op) {
    m_wheres.append(QString("%1 %2 ?").arg(field, op));
    m_whereParams.append(value);
    return *this;
}

QueryBuilder& QueryBuilder::whereRaw(const QString& condition, const QVariantList& params) {
    m_wheres.append(condition);
    m_whereParams.append(params);
    return *this;
}

QueryBuilder& QueryBuilder::orderBy(const QString& field, const QString& direction) {
    m_orderBy = QString("ORDER BY %1 %2").arg(field, direction);
    return *this;
}

QueryBuilder& QueryBuilder::limit(int count) {
    m_limit = count;
    return *this;
}

QueryBuilder& QueryBuilder::offset(int count) {
    m_offset = count;
    return *this;
}

QSqlQuery QueryBuilder::get() {
    QString sql = "SELECT ";
    sql += m_selects.isEmpty() ? "*" : m_selects.join(", ");
    sql += " FROM " + m_table;
    
    if (!m_wheres.isEmpty()) {
        sql += " WHERE " + m_wheres.join(" AND ");
    }
    if (!m_orderBy.isEmpty()) {
        sql += " " + m_orderBy;
    }
    if (m_limit >= 0) {
        sql += QString(" LIMIT %1").arg(m_limit);
    }
    if (m_offset >= 0) {
        sql += QString(" OFFSET %1").arg(m_offset);
    }
    
    QSqlQuery query = m_db.prepare(sql);
    for (const auto& param : m_whereParams) {
        query.addBindValue(param);
    }
    
    if (!query.exec()) {
        LOG_ERROR("QueryBuilder GET error: " + sql);
    }
    return query;
}

int QueryBuilder::insert(const QVariantMap& data) {
    if (data.isEmpty()) return -1;
    
    QStringList fields;
    QStringList placeholders;
    QVariantList params;
    
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        fields.append(it.key());
        placeholders.append("?");
        params.append(it.value());
    }
    
    QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)")
                      .arg(m_table, fields.join(", "), placeholders.join(", "));
                      
    QSqlQuery query = m_db.prepare(sql);
    for (const auto& param : params) {
        query.addBindValue(param);
    }
    
    if (query.exec()) {
        return query.lastInsertId().toInt();
    } else {
        LOG_ERROR("QueryBuilder INSERT error: " + sql);
        return -1;
    }
}

bool QueryBuilder::update(const QVariantMap& data) {
    if (data.isEmpty()) return false;
    if (m_wheres.isEmpty()) {
        LOG_WARN("QueryBuilder UPDATE called without WHERE clause. Refusing to execute.");
        return false;
    }
    
    QStringList sets;
    QVariantList params;
    
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        sets.append(it.key() + " = ?");
        params.append(it.value());
    }
    
    // Append where params after set params
    params.append(m_whereParams);
    
    QString sql = QString("UPDATE %1 SET %2 WHERE %3")
                      .arg(m_table, sets.join(", "), m_wheres.join(" AND "));
                      
    QSqlQuery query = m_db.prepare(sql);
    for (const auto& param : params) {
        query.addBindValue(param);
    }
    
    if (query.exec()) {
        return true;
    } else {
        LOG_ERROR("QueryBuilder UPDATE error: " + sql);
        return false;
    }
}

bool QueryBuilder::remove() {
    if (m_wheres.isEmpty()) {
        LOG_WARN("QueryBuilder DELETE called without WHERE clause. Refusing to execute.");
        return false;
    }
    
    QString sql = QString("DELETE FROM %1 WHERE %2").arg(m_table, m_wheres.join(" AND "));
    QSqlQuery query = m_db.prepare(sql);
    for (const auto& param : m_whereParams) {
        query.addBindValue(param);
    }
    
    if (query.exec()) {
        return true;
    } else {
        LOG_ERROR("QueryBuilder DELETE error: " + sql);
        return false;
    }
}

} // namespace RetailMS

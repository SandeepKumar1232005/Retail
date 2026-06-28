#include "ExpenseRepository.h"
#include "../database/DatabaseManager.h"
#include "../database/QueryBuilder.h"
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantMap>

namespace RetailMS {

ExpenseRepository::ExpenseRepository(std::shared_ptr<DatabaseManager> db) : m_db(std::move(db)) {}

Expense ExpenseRepository::mapRow(const QSqlQuery& query) const {
    Expense entity;
    QJsonObject obj;
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        obj[record.fieldName(i)] = QJsonValue::fromVariant(record.value(i));
    }
    entity.fromJson(obj);
    return entity;
}

std::optional<Expense> ExpenseRepository::findById(int id) const {
    auto results = findWhere("id = ?", {id});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

std::vector<Expense> ExpenseRepository::findAll() const {
    return findWhere("");
}

std::vector<Expense> ExpenseRepository::findWhere(const QString& condition, const QVariantList& params) const {
    std::vector<Expense> results;
    QueryBuilder qb(*m_db);
    qb.table("expenses");
    if (!condition.isEmpty()) {
        qb.whereRaw(condition, params);
    }
    auto query = qb.get();
    while (query.next()) {
        results.push_back(mapRow(query));
    }
    return results;
}

int ExpenseRepository::save(const Expense& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    obj.remove("updated_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("expenses").insert(data);
}

bool ExpenseRepository::update(const Expense& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("expenses").where("id", entity.id).update(data);
}

bool ExpenseRepository::remove(int id) {
    QueryBuilder qb(*m_db);
    return qb.table("expenses").where("id", id).remove();
}

int ExpenseRepository::count() const {
    auto res = m_db->executeScalar("SELECT COUNT(*) FROM expenses");
    return res ? res->toInt() : 0;
}

bool ExpenseRepository::exists(int id) const {
    auto res = m_db->executeScalar("SELECT 1 FROM expenses WHERE id = ?", {id});
    return res.has_value();
}

} // namespace RetailMS

#include "CustomerRepository.h"
#include "../database/DatabaseManager.h"
#include "../database/QueryBuilder.h"
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantMap>

namespace RetailMS {

CustomerRepository::CustomerRepository(std::shared_ptr<DatabaseManager> db) : m_db(std::move(db)) {}

Customer CustomerRepository::mapRow(const QSqlQuery& query) const {
    Customer entity;
    QJsonObject obj;
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        obj[record.fieldName(i)] = QJsonValue::fromVariant(record.value(i));
    }
    entity.fromJson(obj);
    return entity;
}

std::optional<Customer> CustomerRepository::findById(int id) const {
    auto results = findWhere("id = ?", {id});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

std::vector<Customer> CustomerRepository::findAll() const {
    return findWhere("");
}

std::vector<Customer> CustomerRepository::findWhere(const QString& condition, const QVariantList& params) const {
    std::vector<Customer> results;
    QueryBuilder qb(*m_db);
    qb.table("customers");
    if (!condition.isEmpty()) {
        qb.whereRaw(condition, params);
    }
    auto query = qb.get();
    while (query.next()) {
        results.push_back(mapRow(query));
    }
    return results;
}

int CustomerRepository::save(const Customer& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    obj.remove("updated_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("customers").insert(data);
}

bool CustomerRepository::update(const Customer& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("customers").where("id", entity.id).update(data);
}

bool CustomerRepository::remove(int id) {
    QueryBuilder qb(*m_db);
    return qb.table("customers").where("id", id).remove();
}

int CustomerRepository::count() const {
    auto res = m_db->executeScalar("SELECT COUNT(*) FROM customers");
    return res ? res->toInt() : 0;
}

bool CustomerRepository::exists(int id) const {
    auto res = m_db->executeScalar("SELECT 1 FROM customers WHERE id = ?", {id});
    return res.has_value();
}

} // namespace RetailMS

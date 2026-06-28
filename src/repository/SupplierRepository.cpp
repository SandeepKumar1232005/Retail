#include "SupplierRepository.h"
#include "../database/DatabaseManager.h"
#include "../database/QueryBuilder.h"
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantMap>

namespace RetailMS {

SupplierRepository::SupplierRepository(std::shared_ptr<DatabaseManager> db) : m_db(std::move(db)) {}

Supplier SupplierRepository::mapRow(const QSqlQuery& query) const {
    Supplier entity;
    QJsonObject obj;
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        obj[record.fieldName(i)] = QJsonValue::fromVariant(record.value(i));
    }
    entity.fromJson(obj);
    return entity;
}

std::optional<Supplier> SupplierRepository::findById(int id) const {
    auto results = findWhere("id = ?", {id});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

std::vector<Supplier> SupplierRepository::findAll() const {
    return findWhere("");
}

std::vector<Supplier> SupplierRepository::findWhere(const QString& condition, const QVariantList& params) const {
    std::vector<Supplier> results;
    QueryBuilder qb(*m_db);
    qb.table("suppliers");
    if (!condition.isEmpty()) {
        qb.whereRaw(condition, params);
    }
    auto query = qb.get();
    while (query.next()) {
        results.push_back(mapRow(query));
    }
    return results;
}

int SupplierRepository::save(const Supplier& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    obj.remove("updated_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("suppliers").insert(data);
}

bool SupplierRepository::update(const Supplier& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("suppliers").where("id", entity.id).update(data);
}

bool SupplierRepository::remove(int id) {
    QueryBuilder qb(*m_db);
    return qb.table("suppliers").where("id", id).remove();
}

int SupplierRepository::count() const {
    auto res = m_db->executeScalar("SELECT COUNT(*) FROM suppliers");
    return res ? res->toInt() : 0;
}

bool SupplierRepository::exists(int id) const {
    auto res = m_db->executeScalar("SELECT 1 FROM suppliers WHERE id = ?", {id});
    return res.has_value();
}

} // namespace RetailMS

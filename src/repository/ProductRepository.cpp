#include "ProductRepository.h"
#include "../database/DatabaseManager.h"
#include "../database/QueryBuilder.h"
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantMap>

namespace RetailMS {

ProductRepository::ProductRepository(std::shared_ptr<DatabaseManager> db) : m_db(std::move(db)) {}

Product ProductRepository::mapRow(const QSqlQuery& query) const {
    Product entity;
    QJsonObject obj;
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        obj[record.fieldName(i)] = QJsonValue::fromVariant(record.value(i));
    }
    entity.fromJson(obj);
    return entity;
}

std::optional<Product> ProductRepository::findById(int id) const {
    auto results = findWhere("id = ?", {id});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

std::vector<Product> ProductRepository::findAll() const {
    return findWhere("");
}

std::vector<Product> ProductRepository::findWhere(const QString& condition, const QVariantList& params) const {
    std::vector<Product> results;
    QueryBuilder qb(*m_db);
    qb.table("products");
    if (!condition.isEmpty()) {
        qb.whereRaw(condition, params);
    }
    auto query = qb.get();
    while (query.next()) {
        results.push_back(mapRow(query));
    }
    return results;
}

int ProductRepository::save(const Product& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    obj.remove("updated_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("products").insert(data);
}

bool ProductRepository::update(const Product& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("products").where("id", entity.id).update(data);
}

bool ProductRepository::remove(int id) {
    QueryBuilder qb(*m_db);
    return qb.table("products").where("id", id).remove();
}

int ProductRepository::count() const {
    auto res = m_db->executeScalar("SELECT COUNT(*) FROM products");
    return res ? res->toInt() : 0;
}

bool ProductRepository::exists(int id) const {
    auto res = m_db->executeScalar("SELECT 1 FROM products WHERE id = ?", {id});
    return res.has_value();
}

} // namespace RetailMS

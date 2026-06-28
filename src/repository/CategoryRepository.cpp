#include "CategoryRepository.h"
#include "../database/DatabaseManager.h"
#include "../database/QueryBuilder.h"
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantMap>

namespace RetailMS {

CategoryRepository::CategoryRepository(std::shared_ptr<DatabaseManager> db) : m_db(std::move(db)) {}

Category CategoryRepository::mapRow(const QSqlQuery& query) const {
    Category entity;
    QJsonObject obj;
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        obj[record.fieldName(i)] = QJsonValue::fromVariant(record.value(i));
    }
    entity.fromJson(obj);
    return entity;
}

std::optional<Category> CategoryRepository::findById(int id) const {
    auto results = findWhere("id = ?", {id});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

std::vector<Category> CategoryRepository::findAll() const {
    return findWhere("");
}

std::vector<Category> CategoryRepository::findWhere(const QString& condition, const QVariantList& params) const {
    std::vector<Category> results;
    QueryBuilder qb(*m_db);
    qb.table("categories");
    if (!condition.isEmpty()) {
        qb.whereRaw(condition, params);
    }
    auto query = qb.get();
    while (query.next()) {
        results.push_back(mapRow(query));
    }
    return results;
}

int CategoryRepository::save(const Category& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    obj.remove("updated_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("categories").insert(data);
}

bool CategoryRepository::update(const Category& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("categories").where("id", entity.id).update(data);
}

bool CategoryRepository::remove(int id) {
    QueryBuilder qb(*m_db);
    return qb.table("categories").where("id", id).remove();
}

int CategoryRepository::count() const {
    auto res = m_db->executeScalar("SELECT COUNT(*) FROM categories");
    return res ? res->toInt() : 0;
}

bool CategoryRepository::exists(int id) const {
    auto res = m_db->executeScalar("SELECT 1 FROM categories WHERE id = ?", {id});
    return res.has_value();
}

} // namespace RetailMS

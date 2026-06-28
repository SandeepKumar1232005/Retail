#include "InventoryLogRepository.h"
#include "../database/DatabaseManager.h"
#include "../database/QueryBuilder.h"
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantMap>

namespace RetailMS {

InventoryLogRepository::InventoryLogRepository(std::shared_ptr<DatabaseManager> db) : m_db(std::move(db)) {}

InventoryLog InventoryLogRepository::mapRow(const QSqlQuery& query) const {
    InventoryLog entity;
    QJsonObject obj;
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        obj[record.fieldName(i)] = QJsonValue::fromVariant(record.value(i));
    }
    entity.fromJson(obj);
    return entity;
}

std::optional<InventoryLog> InventoryLogRepository::findById(int id) const {
    auto results = findWhere("id = ?", {id});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

std::vector<InventoryLog> InventoryLogRepository::findAll() const {
    return findWhere("");
}

std::vector<InventoryLog> InventoryLogRepository::findWhere(const QString& condition, const QVariantList& params) const {
    std::vector<InventoryLog> results;
    QueryBuilder qb(*m_db);
    qb.table("inventory_logs");
    if (!condition.isEmpty()) {
        qb.whereRaw(condition, params);
    }
    auto query = qb.get();
    while (query.next()) {
        results.push_back(mapRow(query));
    }
    return results;
}

int InventoryLogRepository::save(const InventoryLog& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    obj.remove("updated_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("inventory_logs").insert(data);
}

bool InventoryLogRepository::update(const InventoryLog& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("inventory_logs").where("id", entity.id).update(data);
}

bool InventoryLogRepository::remove(int id) {
    QueryBuilder qb(*m_db);
    return qb.table("inventory_logs").where("id", id).remove();
}

int InventoryLogRepository::count() const {
    auto res = m_db->executeScalar("SELECT COUNT(*) FROM inventory_logs");
    return res ? res->toInt() : 0;
}

bool InventoryLogRepository::exists(int id) const {
    auto res = m_db->executeScalar("SELECT 1 FROM inventory_logs WHERE id = ?", {id});
    return res.has_value();
}

} // namespace RetailMS

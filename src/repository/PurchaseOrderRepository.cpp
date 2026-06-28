#include "PurchaseOrderRepository.h"
#include "../database/DatabaseManager.h"
#include "../database/QueryBuilder.h"
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantMap>

namespace RetailMS {

PurchaseOrderRepository::PurchaseOrderRepository(std::shared_ptr<DatabaseManager> db) : m_db(std::move(db)) {}

PurchaseOrder PurchaseOrderRepository::mapRow(const QSqlQuery& query) const {
    PurchaseOrder entity;
    QJsonObject obj;
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        obj[record.fieldName(i)] = QJsonValue::fromVariant(record.value(i));
    }
    entity.fromJson(obj);
    return entity;
}

std::optional<PurchaseOrder> PurchaseOrderRepository::findById(int id) const {
    auto results = findWhere("id = ?", {id});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

std::vector<PurchaseOrder> PurchaseOrderRepository::findAll() const {
    return findWhere("");
}

std::vector<PurchaseOrder> PurchaseOrderRepository::findWhere(const QString& condition, const QVariantList& params) const {
    std::vector<PurchaseOrder> results;
    QueryBuilder qb(*m_db);
    qb.table("purchase_orders");
    if (!condition.isEmpty()) {
        qb.whereRaw(condition, params);
    }
    auto query = qb.get();
    while (query.next()) {
        results.push_back(mapRow(query));
    }
    return results;
}

int PurchaseOrderRepository::save(const PurchaseOrder& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    obj.remove("updated_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("purchase_orders").insert(data);
}

bool PurchaseOrderRepository::update(const PurchaseOrder& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("purchase_orders").where("id", entity.id).update(data);
}

bool PurchaseOrderRepository::remove(int id) {
    QueryBuilder qb(*m_db);
    return qb.table("purchase_orders").where("id", id).remove();
}

int PurchaseOrderRepository::count() const {
    auto res = m_db->executeScalar("SELECT COUNT(*) FROM purchase_orders");
    return res ? res->toInt() : 0;
}

bool PurchaseOrderRepository::exists(int id) const {
    auto res = m_db->executeScalar("SELECT 1 FROM purchase_orders WHERE id = ?", {id});
    return res.has_value();
}

} // namespace RetailMS

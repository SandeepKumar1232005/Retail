#include "NotificationRepository.h"
#include "../database/DatabaseManager.h"
#include "../database/QueryBuilder.h"
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantMap>

namespace RetailMS {

NotificationRepository::NotificationRepository(std::shared_ptr<DatabaseManager> db) : m_db(std::move(db)) {}

Notification NotificationRepository::mapRow(const QSqlQuery& query) const {
    Notification entity;
    QJsonObject obj;
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        obj[record.fieldName(i)] = QJsonValue::fromVariant(record.value(i));
    }
    entity.fromJson(obj);
    return entity;
}

std::optional<Notification> NotificationRepository::findById(int id) const {
    auto results = findWhere("id = ?", {id});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

std::vector<Notification> NotificationRepository::findAll() const {
    return findWhere("");
}

std::vector<Notification> NotificationRepository::findWhere(const QString& condition, const QVariantList& params) const {
    std::vector<Notification> results;
    QueryBuilder qb(*m_db);
    qb.table("notifications");
    if (!condition.isEmpty()) {
        qb.whereRaw(condition, params);
    }
    auto query = qb.get();
    while (query.next()) {
        results.push_back(mapRow(query));
    }
    return results;
}

int NotificationRepository::save(const Notification& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    obj.remove("updated_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("notifications").insert(data);
}

bool NotificationRepository::update(const Notification& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("notifications").where("id", entity.id).update(data);
}

bool NotificationRepository::remove(int id) {
    QueryBuilder qb(*m_db);
    return qb.table("notifications").where("id", id).remove();
}

int NotificationRepository::count() const {
    auto res = m_db->executeScalar("SELECT COUNT(*) FROM notifications");
    return res ? res->toInt() : 0;
}

bool NotificationRepository::exists(int id) const {
    auto res = m_db->executeScalar("SELECT 1 FROM notifications WHERE id = ?", {id});
    return res.has_value();
}

} // namespace RetailMS

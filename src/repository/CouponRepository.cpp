#include "CouponRepository.h"
#include "../database/DatabaseManager.h"
#include "../database/QueryBuilder.h"
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantMap>

namespace RetailMS {

CouponRepository::CouponRepository(std::shared_ptr<DatabaseManager> db) : m_db(std::move(db)) {}

Coupon CouponRepository::mapRow(const QSqlQuery& query) const {
    Coupon entity;
    QJsonObject obj;
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        obj[record.fieldName(i)] = QJsonValue::fromVariant(record.value(i));
    }
    entity.fromJson(obj);
    return entity;
}

std::optional<Coupon> CouponRepository::findById(int id) const {
    auto results = findWhere("id = ?", {id});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

std::vector<Coupon> CouponRepository::findAll() const {
    return findWhere("");
}

std::vector<Coupon> CouponRepository::findWhere(const QString& condition, const QVariantList& params) const {
    std::vector<Coupon> results;
    QueryBuilder qb(*m_db);
    qb.table("coupons");
    if (!condition.isEmpty()) {
        qb.whereRaw(condition, params);
    }
    auto query = qb.get();
    while (query.next()) {
        results.push_back(mapRow(query));
    }
    return results;
}

int CouponRepository::save(const Coupon& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    obj.remove("updated_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("coupons").insert(data);
}

bool CouponRepository::update(const Coupon& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("coupons").where("id", entity.id).update(data);
}

bool CouponRepository::remove(int id) {
    QueryBuilder qb(*m_db);
    return qb.table("coupons").where("id", id).remove();
}

int CouponRepository::count() const {
    auto res = m_db->executeScalar("SELECT COUNT(*) FROM coupons");
    return res ? res->toInt() : 0;
}

bool CouponRepository::exists(int id) const {
    auto res = m_db->executeScalar("SELECT 1 FROM coupons WHERE id = ?", {id});
    return res.has_value();
}

} // namespace RetailMS

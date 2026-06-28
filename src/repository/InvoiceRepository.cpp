#include "InvoiceRepository.h"
#include "../database/DatabaseManager.h"
#include "../database/QueryBuilder.h"
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantMap>

namespace RetailMS {

InvoiceRepository::InvoiceRepository(std::shared_ptr<DatabaseManager> db) : m_db(std::move(db)) {}

Invoice InvoiceRepository::mapRow(const QSqlQuery& query) const {
    Invoice entity;
    QJsonObject obj;
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        obj[record.fieldName(i)] = QJsonValue::fromVariant(record.value(i));
    }
    entity.fromJson(obj);
    return entity;
}

std::optional<Invoice> InvoiceRepository::findById(int id) const {
    auto results = findWhere("id = ?", {id});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

std::vector<Invoice> InvoiceRepository::findAll() const {
    return findWhere("");
}

std::vector<Invoice> InvoiceRepository::findWhere(const QString& condition, const QVariantList& params) const {
    std::vector<Invoice> results;
    QueryBuilder qb(*m_db);
    qb.table("invoices");
    if (!condition.isEmpty()) {
        qb.whereRaw(condition, params);
    }
    auto query = qb.get();
    while (query.next()) {
        results.push_back(mapRow(query));
    }
    return results;
}

int InvoiceRepository::save(const Invoice& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    obj.remove("updated_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("invoices").insert(data);
}

bool InvoiceRepository::update(const Invoice& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("invoices").where("id", entity.id).update(data);
}

bool InvoiceRepository::remove(int id) {
    QueryBuilder qb(*m_db);
    return qb.table("invoices").where("id", id).remove();
}

int InvoiceRepository::count() const {
    auto res = m_db->executeScalar("SELECT COUNT(*) FROM invoices");
    return res ? res->toInt() : 0;
}

bool InvoiceRepository::exists(int id) const {
    auto res = m_db->executeScalar("SELECT 1 FROM invoices WHERE id = ?", {id});
    return res.has_value();
}

} // namespace RetailMS

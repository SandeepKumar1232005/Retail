#include "InvoiceRepository.h"
#include "../database/DatabaseManager.h"
#include "../database/QueryBuilder.h"
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonArray>
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
    QJsonArray itemsArr;
    int invoiceId = obj["id"].toInt(-1);
    if (invoiceId > 0) {
        QueryBuilder qbItem(*m_db);
        auto itemQuery = qbItem.table("invoice_items").where("invoice_id", invoiceId).get();
        while (itemQuery.next()) {
            QJsonObject itemObj;
            QSqlRecord itemRecord = itemQuery.record();
            for (int i = 0; i < itemRecord.count(); ++i) {
                itemObj[itemRecord.fieldName(i)] = QJsonValue::fromVariant(itemRecord.value(i));
            }
            itemsArr.append(itemObj);
        }
    }
    obj["items"] = itemsArr;
    
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
    QJsonArray itemsArr = obj.take("items").toArray(); // Remove and store items
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    int invoiceId = qb.table("invoices").insert(data);
    
    if (invoiceId > 0) {
        for (int i = 0; i < itemsArr.size(); ++i) {
            QJsonObject itemObj = itemsArr[i].toObject();
            itemObj.remove("id");
            itemObj.insert("invoice_id", invoiceId);
            QueryBuilder qbItem(*m_db);
            qbItem.table("invoice_items").insert(itemObj.toVariantMap());
        }
    }
    return invoiceId;
}

bool InvoiceRepository::update(const Invoice& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    QJsonArray itemsArr = obj.take("items").toArray(); // Remove and store items
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    bool ok = qb.table("invoices").where("id", entity.id).update(data);
    
    if (ok) {
        QueryBuilder qbDel(*m_db);
        qbDel.table("invoice_items").where("invoice_id", entity.id).remove();
        
        for (int i = 0; i < itemsArr.size(); ++i) {
            QJsonObject itemObj = itemsArr[i].toObject();
            itemObj.remove("id");
            itemObj.insert("invoice_id", entity.id);
            QueryBuilder qbItem(*m_db);
            qbItem.table("invoice_items").insert(itemObj.toVariantMap());
        }
    }
    return ok;
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

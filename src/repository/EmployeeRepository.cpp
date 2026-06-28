#include "EmployeeRepository.h"
#include "../database/DatabaseManager.h"
#include "../database/QueryBuilder.h"
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantMap>

namespace RetailMS {

EmployeeRepository::EmployeeRepository(std::shared_ptr<DatabaseManager> db) : m_db(std::move(db)) {}

Employee EmployeeRepository::mapRow(const QSqlQuery& query) const {
    Employee entity;
    QJsonObject obj;
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        obj[record.fieldName(i)] = QJsonValue::fromVariant(record.value(i));
    }
    entity.fromJson(obj);
    return entity;
}

std::optional<Employee> EmployeeRepository::findById(int id) const {
    auto results = findWhere("id = ?", {id});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

std::vector<Employee> EmployeeRepository::findAll() const {
    return findWhere("");
}

std::vector<Employee> EmployeeRepository::findWhere(const QString& condition, const QVariantList& params) const {
    std::vector<Employee> results;
    QueryBuilder qb(*m_db);
    qb.table("employees");
    if (!condition.isEmpty()) {
        qb.whereRaw(condition, params);
    }
    auto query = qb.get();
    while (query.next()) {
        results.push_back(mapRow(query));
    }
    return results;
}

int EmployeeRepository::save(const Employee& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    obj.remove("updated_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("employees").insert(data);
}

bool EmployeeRepository::update(const Employee& entity) {
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("employees").where("id", entity.id).update(data);
}

bool EmployeeRepository::remove(int id) {
    QueryBuilder qb(*m_db);
    return qb.table("employees").where("id", id).remove();
}

int EmployeeRepository::count() const {
    auto res = m_db->executeScalar("SELECT COUNT(*) FROM employees");
    return res ? res->toInt() : 0;
}

bool EmployeeRepository::exists(int id) const {
    auto res = m_db->executeScalar("SELECT 1 FROM employees WHERE id = ?", {id});
    return res.has_value();
}

} // namespace RetailMS

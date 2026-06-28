import os

models = [
    ("User", "users"),
    ("Product", "products"),
    ("Category", "categories"),
    ("Customer", "customers"),
    ("Supplier", "suppliers"),
    ("Employee", "employees"),
    ("Invoice", "invoices"),
    ("InventoryLog", "inventory_logs"),
    ("PurchaseOrder", "purchase_orders"),
    ("Expense", "expenses"),
    ("Coupon", "coupons"),
    ("Notification", "notifications")
]

repo_dir = "f:/PROJECT/RETAIL/src/repository"

header_template = """#pragma once
#include "IRepository.h"
#include "../models/{model_name}.h"
#include <memory>

namespace RetailMS {{

class DatabaseManager;

class {model_name}Repository : public IRepository<{model_name}> {{
public:
    explicit {model_name}Repository(std::shared_ptr<DatabaseManager> db);

    std::optional<{model_name}> findById(int id) const override;
    std::vector<{model_name}> findAll() const override;
    std::vector<{model_name}> findWhere(const QString& condition, const QVariantList& params = {{}}) const override;
    
    int save(const {model_name}& entity) override;
    bool update(const {model_name}& entity) override;
    bool remove(int id) override;
    
    int count() const override;
    bool exists(int id) const override;

private:
    std::shared_ptr<DatabaseManager> m_db;
    {model_name} mapRow(const class QSqlQuery& query) const;
}};

}} // namespace RetailMS
"""

cpp_template = """#include "{model_name}Repository.h"
#include "../database/DatabaseManager.h"
#include "../database/QueryBuilder.h"
#include <QSqlRecord>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariantMap>

namespace RetailMS {{

{model_name}Repository::{model_name}Repository(std::shared_ptr<DatabaseManager> db) : m_db(std::move(db)) {{}}

{model_name} {model_name}Repository::mapRow(const QSqlQuery& query) const {{
    {model_name} entity;
    QJsonObject obj;
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {{
        obj[record.fieldName(i)] = QJsonValue::fromVariant(record.value(i));
    }}
    entity.fromJson(obj);
    return entity;
}}

std::optional<{model_name}> {model_name}Repository::findById(int id) const {{
    auto results = findWhere("id = ?", {{id}});
    if (!results.empty()) return results.front();
    return std::nullopt;
}}

std::vector<{model_name}> {model_name}Repository::findAll() const {{
    return findWhere("");
}}

std::vector<{model_name}> {model_name}Repository::findWhere(const QString& condition, const QVariantList& params) const {{
    std::vector<{model_name}> results;
    QueryBuilder qb(*m_db);
    qb.table("{table_name}");
    if (!condition.isEmpty()) {{
        qb.whereRaw(condition, params);
    }}
    auto query = qb.get();
    while (query.next()) {{
        results.push_back(mapRow(query));
    }}
    return results;
}}

int {model_name}Repository::save(const {model_name}& entity) {{
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    obj.remove("updated_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("{table_name}").insert(data);
}}

bool {model_name}Repository::update(const {model_name}& entity) {{
    QJsonObject obj = entity.toJson();
    obj.remove("id");
    obj.remove("created_at");
    
    QVariantMap data = obj.toVariantMap();
    QueryBuilder qb(*m_db);
    return qb.table("{table_name}").where("id", entity.id).update(data);
}}

bool {model_name}Repository::remove(int id) {{
    QueryBuilder qb(*m_db);
    return qb.table("{table_name}").where("id", id).remove();
}}

int {model_name}Repository::count() const {{
    auto res = m_db->executeScalar("SELECT COUNT(*) FROM {table_name}");
    return res ? res->toInt() : 0;
}}

bool {model_name}Repository::exists(int id) const {{
    auto res = m_db->executeScalar("SELECT 1 FROM {table_name} WHERE id = ?", {{id}});
    return res.has_value();
}}

}} // namespace RetailMS
"""

for model_name, table_name in models:
    h_path = os.path.join(repo_dir, f"{model_name}Repository.h")
    cpp_path = os.path.join(repo_dir, f"{model_name}Repository.cpp")
    
    with open(h_path, 'w') as f:
        f.write(header_template.format(model_name=model_name))
        
    with open(cpp_path, 'w') as f:
        f.write(cpp_template.format(model_name=model_name, table_name=table_name))

print("Repositories generated successfully.")

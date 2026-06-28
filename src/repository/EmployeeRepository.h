#pragma once
#include "IRepository.h"
#include "../models/Employee.h"
#include <memory>

namespace RetailMS {

class DatabaseManager;

class EmployeeRepository : public IRepository<Employee> {
public:
    explicit EmployeeRepository(std::shared_ptr<DatabaseManager> db);

    std::optional<Employee> findById(int id) const override;
    std::vector<Employee> findAll() const override;
    std::vector<Employee> findWhere(const QString& condition, const QVariantList& params = {}) const override;
    
    int save(const Employee& entity) override;
    bool update(const Employee& entity) override;
    bool remove(int id) override;
    
    int count() const override;
    bool exists(int id) const override;

private:
    std::shared_ptr<DatabaseManager> m_db;
    Employee mapRow(const class QSqlQuery& query) const;
};

} // namespace RetailMS

#pragma once
#include "IRepository.h"
#include "../models/Customer.h"
#include <memory>

namespace RetailMS {

class DatabaseManager;

class CustomerRepository : public IRepository<Customer> {
public:
    explicit CustomerRepository(std::shared_ptr<DatabaseManager> db);

    std::optional<Customer> findById(int id) const override;
    std::vector<Customer> findAll() const override;
    std::vector<Customer> findWhere(const QString& condition, const QVariantList& params = {}) const override;
    
    int save(const Customer& entity) override;
    bool update(const Customer& entity) override;
    bool remove(int id) override;
    
    int count() const override;
    bool exists(int id) const override;

private:
    std::shared_ptr<DatabaseManager> m_db;
    Customer mapRow(const class QSqlQuery& query) const;
};

} // namespace RetailMS

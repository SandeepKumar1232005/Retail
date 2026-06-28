#pragma once
#include "IRepository.h"
#include "../models/Supplier.h"
#include <memory>

namespace RetailMS {

class DatabaseManager;

class SupplierRepository : public IRepository<Supplier> {
public:
    explicit SupplierRepository(std::shared_ptr<DatabaseManager> db);

    std::optional<Supplier> findById(int id) const override;
    std::vector<Supplier> findAll() const override;
    std::vector<Supplier> findWhere(const QString& condition, const QVariantList& params = {}) const override;
    
    int save(const Supplier& entity) override;
    bool update(const Supplier& entity) override;
    bool remove(int id) override;
    
    int count() const override;
    bool exists(int id) const override;

private:
    std::shared_ptr<DatabaseManager> m_db;
    Supplier mapRow(const class QSqlQuery& query) const;
};

} // namespace RetailMS

#pragma once
#include "IRepository.h"
#include "../models/PurchaseOrder.h"
#include <memory>

namespace RetailMS {

class DatabaseManager;

class PurchaseOrderRepository : public IRepository<PurchaseOrder> {
public:
    explicit PurchaseOrderRepository(std::shared_ptr<DatabaseManager> db);

    std::optional<PurchaseOrder> findById(int id) const override;
    std::vector<PurchaseOrder> findAll() const override;
    std::vector<PurchaseOrder> findWhere(const QString& condition, const QVariantList& params = {}) const override;
    
    int save(const PurchaseOrder& entity) override;
    bool update(const PurchaseOrder& entity) override;
    bool remove(int id) override;
    
    int count() const override;
    bool exists(int id) const override;

private:
    std::shared_ptr<DatabaseManager> m_db;
    PurchaseOrder mapRow(const class QSqlQuery& query) const;
};

} // namespace RetailMS

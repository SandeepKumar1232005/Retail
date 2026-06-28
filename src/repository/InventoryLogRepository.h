#pragma once
#include "IRepository.h"
#include "../models/InventoryLog.h"
#include <memory>

namespace RetailMS {

class DatabaseManager;

class InventoryLogRepository : public IRepository<InventoryLog> {
public:
    explicit InventoryLogRepository(std::shared_ptr<DatabaseManager> db);

    std::optional<InventoryLog> findById(int id) const override;
    std::vector<InventoryLog> findAll() const override;
    std::vector<InventoryLog> findWhere(const QString& condition, const QVariantList& params = {}) const override;
    
    int save(const InventoryLog& entity) override;
    bool update(const InventoryLog& entity) override;
    bool remove(int id) override;
    
    int count() const override;
    bool exists(int id) const override;

private:
    std::shared_ptr<DatabaseManager> m_db;
    InventoryLog mapRow(const class QSqlQuery& query) const;
};

} // namespace RetailMS

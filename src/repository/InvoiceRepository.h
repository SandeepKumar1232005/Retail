#pragma once
class QSqlQuery;
#include "IRepository.h"
#include "../models/Invoice.h"
#include <memory>

namespace RetailMS {

class DatabaseManager;

class InvoiceRepository : public IRepository<Invoice> {
public:
    explicit InvoiceRepository(std::shared_ptr<DatabaseManager> db);

    std::optional<Invoice> findById(int id) const override;
    std::vector<Invoice> findAll() const override;
    std::vector<Invoice> findWhere(const QString& condition, const QVariantList& params = {}) const override;
    
    int save(const Invoice& entity) override;
    bool update(const Invoice& entity) override;
    bool remove(int id) override;
    
    int count() const override;
    bool exists(int id) const override;

private:
    std::shared_ptr<DatabaseManager> m_db;
    Invoice mapRow(const QSqlQuery& query) const;
};

} // namespace RetailMS

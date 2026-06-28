#pragma once
#include "IRepository.h"
#include "../models/Expense.h"
#include <memory>

namespace RetailMS {

class DatabaseManager;

class ExpenseRepository : public IRepository<Expense> {
public:
    explicit ExpenseRepository(std::shared_ptr<DatabaseManager> db);

    std::optional<Expense> findById(int id) const override;
    std::vector<Expense> findAll() const override;
    std::vector<Expense> findWhere(const QString& condition, const QVariantList& params = {}) const override;
    
    int save(const Expense& entity) override;
    bool update(const Expense& entity) override;
    bool remove(int id) override;
    
    int count() const override;
    bool exists(int id) const override;

private:
    std::shared_ptr<DatabaseManager> m_db;
    Expense mapRow(const class QSqlQuery& query) const;
};

} // namespace RetailMS

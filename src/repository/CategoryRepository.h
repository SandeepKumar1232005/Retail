#pragma once
#include "IRepository.h"
#include "../models/Category.h"
#include <memory>

namespace RetailMS {

class DatabaseManager;

class CategoryRepository : public IRepository<Category> {
public:
    explicit CategoryRepository(std::shared_ptr<DatabaseManager> db);

    std::optional<Category> findById(int id) const override;
    std::vector<Category> findAll() const override;
    std::vector<Category> findWhere(const QString& condition, const QVariantList& params = {}) const override;
    
    int save(const Category& entity) override;
    bool update(const Category& entity) override;
    bool remove(int id) override;
    
    int count() const override;
    bool exists(int id) const override;

private:
    std::shared_ptr<DatabaseManager> m_db;
    Category mapRow(const class QSqlQuery& query) const;
};

} // namespace RetailMS

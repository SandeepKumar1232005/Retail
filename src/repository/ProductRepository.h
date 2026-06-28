#pragma once
#include "IRepository.h"
#include "../models/Product.h"
#include <memory>

namespace RetailMS {

class DatabaseManager;

class ProductRepository : public IRepository<Product> {
public:
    explicit ProductRepository(std::shared_ptr<DatabaseManager> db);

    std::optional<Product> findById(int id) const override;
    std::vector<Product> findAll() const override;
    std::vector<Product> findWhere(const QString& condition, const QVariantList& params = {}) const override;
    
    int save(const Product& entity) override;
    bool update(const Product& entity) override;
    bool remove(int id) override;
    
    int count() const override;
    bool exists(int id) const override;

private:
    std::shared_ptr<DatabaseManager> m_db;
    Product mapRow(const class QSqlQuery& query) const;
};

} // namespace RetailMS

#pragma once
#include "IRepository.h"
#include "../models/Coupon.h"
#include <memory>

namespace RetailMS {

class DatabaseManager;

class CouponRepository : public IRepository<Coupon> {
public:
    explicit CouponRepository(std::shared_ptr<DatabaseManager> db);

    std::optional<Coupon> findById(int id) const override;
    std::vector<Coupon> findAll() const override;
    std::vector<Coupon> findWhere(const QString& condition, const QVariantList& params = {}) const override;
    
    int save(const Coupon& entity) override;
    bool update(const Coupon& entity) override;
    bool remove(int id) override;
    
    int count() const override;
    bool exists(int id) const override;

private:
    std::shared_ptr<DatabaseManager> m_db;
    Coupon mapRow(const class QSqlQuery& query) const;
};

} // namespace RetailMS

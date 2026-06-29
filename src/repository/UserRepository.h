#pragma once
class QSqlQuery;
#include "IRepository.h"
#include "../models/User.h"
#include <memory>

namespace RetailMS {

class DatabaseManager;

class UserRepository : public IRepository<User> {
public:
    explicit UserRepository(std::shared_ptr<DatabaseManager> db);

    std::optional<User> findById(int id) const override;
    std::vector<User> findAll() const override;
    std::vector<User> findWhere(const QString& condition, const QVariantList& params = {}) const override;
    
    int save(const User& entity) override;
    bool update(const User& entity) override;
    bool remove(int id) override;
    
    int count() const override;
    bool exists(int id) const override;

private:
    std::shared_ptr<DatabaseManager> m_db;
    User mapRow(const QSqlQuery& query) const;
};

} // namespace RetailMS

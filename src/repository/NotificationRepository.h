#pragma once
#include "IRepository.h"
#include "../models/Notification.h"
#include <memory>

namespace RetailMS {

class DatabaseManager;

class NotificationRepository : public IRepository<Notification> {
public:
    explicit NotificationRepository(std::shared_ptr<DatabaseManager> db);

    std::optional<Notification> findById(int id) const override;
    std::vector<Notification> findAll() const override;
    std::vector<Notification> findWhere(const QString& condition, const QVariantList& params = {}) const override;
    
    int save(const Notification& entity) override;
    bool update(const Notification& entity) override;
    bool remove(int id) override;
    
    int count() const override;
    bool exists(int id) const override;

private:
    std::shared_ptr<DatabaseManager> m_db;
    Notification mapRow(const class QSqlQuery& query) const;
};

} // namespace RetailMS

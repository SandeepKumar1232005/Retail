#pragma once
#include <QObject>
#include <memory>
#include "../database/DatabaseManager.h"

namespace RetailMS {

class ExpenseService : public QObject {
    Q_OBJECT
public:
    explicit ExpenseService(std::shared_ptr<DatabaseManager> db, QObject* parent = nullptr) 
        : QObject(parent), m_db(db) {}

private:
    std::shared_ptr<DatabaseManager> m_db;
};

} // namespace RetailMS

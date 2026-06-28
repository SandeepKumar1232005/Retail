#pragma once
#include <QObject>
#include <memory>
#include "../database/DatabaseManager.h"

namespace RetailMS {

class EmployeeService : public QObject {
    Q_OBJECT
public:
    explicit EmployeeService(std::shared_ptr<DatabaseManager> db, QObject* parent = nullptr) 
        : QObject(parent), m_db(db) {}

private:
    std::shared_ptr<DatabaseManager> m_db;
};

} // namespace RetailMS

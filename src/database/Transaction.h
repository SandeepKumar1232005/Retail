#pragma once

namespace RetailMS {

class DatabaseManager;

class Transaction {
public:
    explicit Transaction(DatabaseManager& db);
    ~Transaction();           // auto-rollback if not committed

    void commit();
    void rollback();

private:
    DatabaseManager& m_db;
    bool m_committed{false};
};

} // namespace RetailMS

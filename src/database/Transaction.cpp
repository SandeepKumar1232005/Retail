#include "Transaction.h"
#include "DatabaseManager.h"

namespace RetailMS {

Transaction::Transaction(DatabaseManager& db) : m_db(db) {
    m_db.beginTransaction();
}

Transaction::~Transaction() {
    if (!m_committed) {
        m_db.rollback();
    }
}

void Transaction::commit() {
    if (!m_committed) {
        if (m_db.commit()) {
            m_committed = true;
        }
    }
}

void Transaction::rollback() {
    if (!m_committed) {
        m_db.rollback();
        m_committed = true; // Mark as resolved so destructor doesn't try again
    }
}

} // namespace RetailMS

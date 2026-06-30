#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QDateEdit>
#include <QPushButton>
#include <memory>
#include <vector>

namespace RetailMS {

class BillingController;
class Invoice;

class MyBillsPage : public QWidget {
    Q_OBJECT
public:
    explicit MyBillsPage(std::shared_ptr<BillingController> controller, QWidget* parent = nullptr);

public slots:
    void refreshData();

private slots:
    void onViewInvoiceClicked();

private:
    void setupUi();
    void setupConnections();
    void populateTable(const std::vector<Invoice>& invoices);

    std::shared_ptr<BillingController> m_controller;

    QDateEdit* m_fromDateEdit;
    QDateEdit* m_toDateEdit;
    QPushButton* m_refreshButton;
    QTableWidget* m_billsTable;
};

} // namespace RetailMS

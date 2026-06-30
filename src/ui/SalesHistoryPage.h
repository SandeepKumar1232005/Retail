#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QDateEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <memory>

namespace RetailMS {

class BillingController;

class SalesHistoryPage : public QWidget {
    Q_OBJECT
public:
    explicit SalesHistoryPage(std::shared_ptr<BillingController> controller, QWidget* parent = nullptr);

public slots:
    void refreshData();

private slots:
    void onInvoiceSelected();

private:
    void setupUi();
    void setupConnections();

    std::shared_ptr<BillingController> m_controller;

    QDateEdit* m_fromDateEdit;
    QDateEdit* m_toDateEdit;
    QLineEdit* m_searchField;
    QPushButton* m_refreshButton;
    
    QTableWidget* m_invoicesTable;
    
    // Details panel widgets
    QLabel* m_detailInvoiceNum;
    QLabel* m_detailDateTime;
    QLabel* m_detailCashier;
    QLabel* m_detailCustomer;
    QLabel* m_detailCustomerPhone;
    QLabel* m_detailPaymentMode;
    
    QTableWidget* m_itemsTable;
    
    QLabel* m_detailSubtotal;
    QLabel* m_detailDiscount;
    QLabel* m_detailTax;
    QLabel* m_detailGrandTotal;
};

} // namespace RetailMS

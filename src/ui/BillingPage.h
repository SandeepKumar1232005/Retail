#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <memory>

namespace RetailMS {

class BillingController;

class BillingPage : public QWidget {
    Q_OBJECT
public:
    explicit BillingPage(std::shared_ptr<BillingController> controller, QWidget* parent = nullptr);

private:
    void setupUi();
    void setupConnections();
    void refreshTable();
    void refreshTotals();

    std::shared_ptr<BillingController> m_controller;

    QLineEdit* m_barcodeInput;
    QTableWidget* m_cartTable;
    QLabel* m_subtotalLabel;
    QLabel* m_discountLabel;
    QLabel* m_taxLabel;
    QLabel* m_totalLabel;
};

} // namespace RetailMS

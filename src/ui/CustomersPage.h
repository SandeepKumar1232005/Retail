#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <memory>

namespace RetailMS {

class CustomerService;
class BillingController;

class CustomersPage : public QWidget {
    Q_OBJECT
public:
    explicit CustomersPage(std::shared_ptr<CustomerService> customerService,
                           std::shared_ptr<BillingController> billingController,
                           QWidget* parent = nullptr);

public slots:
    void refreshData();

private slots:
    void onCustomerSelected();
    void onEditCustomerClicked();
    void onAdjustPointsClicked();
    void onLoyaltyRulesClicked();

private:
    void setupUi();
    void setupConnections();
    void loadCustomerDetails(int customerId);

    std::shared_ptr<CustomerService> m_customerService;
    std::shared_ptr<BillingController> m_billingController;

    QLineEdit* m_searchField;
    QPushButton* m_refreshButton;
    QPushButton* m_rulesButton;
    QTableWidget* m_customersTable;

    // Detail Panel Widgets
    QLabel* m_detailName;
    QLabel* m_detailPhone;
    QLabel* m_detailEmail;
    QLabel* m_detailTier;
    QLabel* m_detailPoints;
    QLabel* m_detailTotalSpent;
    QLabel* m_detailTotalOrders;
    QLabel* m_detailAvgOrderVal;
    QLabel* m_detailLastVisit;

    QTableWidget* m_historyTable;
    QTableWidget* m_favoritesTable;

    QPushButton* m_editBtn;
    QPushButton* m_adjustPointsBtn;

    int m_selectedCustomerId{-1};
};

} // namespace RetailMS

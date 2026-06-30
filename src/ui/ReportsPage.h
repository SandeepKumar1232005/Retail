#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <memory>

namespace RetailMS {

class BillingController;

class ReportsPage : public QWidget {
    Q_OBJECT
public:
    explicit ReportsPage(std::shared_ptr<BillingController> controller, QWidget* parent = nullptr);

public slots:
    void refreshData();

private:
    void setupUi();
    void setupConnections();
    
    void loadSalesSummary();
    void loadProductAnalytics();
    void loadCategoryAnalytics();

    std::shared_ptr<BillingController> m_controller;

    QTabWidget* m_tabWidget;
    
    // Sales summary table
    QTableWidget* m_summaryTable;
    
    // Product analytics table
    QTableWidget* m_productTable;
    
    // Category analytics table
    QTableWidget* m_categoryTable;
    
    QPushButton* m_refreshButton;
};

} // namespace RetailMS

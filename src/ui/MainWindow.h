#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <memory>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

namespace RetailMS {

class SidebarWidget;
class BillingController;
class ProductController;
class DashboardPage;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(std::shared_ptr<BillingController> billingController,
                        std::shared_ptr<ProductController> productController,
                        QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void setupUi();
    void setupTopNavBar(QWidget* parent, QVBoxLayout* contentLayout);
    void setupConnections();

    SidebarWidget* m_sidebar;
    QStackedWidget* m_contentArea;
    DashboardPage* m_dashboardPage;
    class BillingPage* m_billingPage;
    class MyBillsPage* m_myBillsPage;
    class InventoryPage* m_inventoryPage;
    class SalesHistoryPage* m_salesHistoryPage;
    class ReportsPage* m_reportsPage;
    class CustomersPage* m_customersPage;
    QLabel* m_breadcrumbLabel;
    QLabel* m_dateTimeLabel;
    QLineEdit* m_searchBox;
    
    std::shared_ptr<BillingController> m_billingController;
    std::shared_ptr<ProductController> m_productController;
};

} // namespace RetailMS

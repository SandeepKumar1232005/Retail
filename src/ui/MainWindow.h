#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <memory>

namespace RetailMS {

class SidebarWidget;
class BillingController;
class ProductController;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(std::shared_ptr<BillingController> billingController,
                        std::shared_ptr<ProductController> productController,
                        QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void setupUi();
    void setupConnections();

    SidebarWidget* m_sidebar;
    QStackedWidget* m_contentArea;
    
    std::shared_ptr<BillingController> m_billingController;
    std::shared_ptr<ProductController> m_productController;
};

} // namespace RetailMS

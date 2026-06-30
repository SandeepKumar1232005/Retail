#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <memory>

namespace RetailMS {

class BillingController;
class ProductController;

class InventoryPage : public QWidget {
    Q_OBJECT
public:
    explicit InventoryPage(std::shared_ptr<BillingController> billingController, 
                           std::shared_ptr<ProductController> productController, 
                           QWidget* parent = nullptr);

public slots:
    void refreshData();

private:
    void setupUi();
    void setupConnections();

    std::shared_ptr<BillingController> m_billingController;
    std::shared_ptr<ProductController> m_productController;

    QLineEdit* m_searchField;
    QComboBox* m_statusFilter;
    QPushButton* m_refreshButton;
    QTableWidget* m_inventoryTable;
};

} // namespace RetailMS

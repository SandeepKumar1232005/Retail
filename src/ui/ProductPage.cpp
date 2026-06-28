#include "ProductPage.h"
#include "../controllers/ProductController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

namespace RetailMS {

ProductPage::ProductPage(std::shared_ptr<ProductController> controller, QWidget* parent)
    : QWidget(parent), m_controller(std::move(controller)) {
    setupUi();
    setupConnections();
    refreshTable();
}

void ProductPage::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Top Bar
    QHBoxLayout* topLayout = new QHBoxLayout();
    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Search products...");
    m_searchInput->setMinimumHeight(40);
    
    QPushButton* addBtn = new QPushButton("Add New Product", this);
    addBtn->setMinimumHeight(40);
    addBtn->setStyleSheet("background-color: #2196F3; color: white;");
    
    topLayout->addWidget(m_searchInput);
    topLayout->addWidget(addBtn);
    mainLayout->addLayout(topLayout);
    
    // Product Table
    m_productTable = new QTableWidget(this);
    m_productTable->setColumnCount(6);
    m_productTable->setHorizontalHeaderLabels({"ID", "Barcode", "Name", "Stock", "Price", "Actions"});
    m_productTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    mainLayout->addWidget(m_productTable, 1);
}

void ProductPage::setupConnections() {
    connect(m_searchInput, &QLineEdit::textChanged, this, &ProductPage::refreshTable);
    
    connect(m_controller.get(), &ProductController::productListChanged, this, &ProductPage::refreshTable);
    
    connect(m_controller.get(), &ProductController::errorOccurred, this, [this](const QString& msg) {
        QMessageBox::critical(this, "Error", msg);
    });
    
    connect(m_controller.get(), &ProductController::successMessage, this, [this](const QString& msg) {
        QMessageBox::information(this, "Success", msg);
    });
}

void ProductPage::refreshTable() {
    QString query = m_searchInput->text();
    auto products = m_controller->searchProducts(query);
    
    m_productTable->setRowCount(products.size());
    for (size_t i = 0; i < products.size(); ++i) {
        const auto& p = products[i];
        m_productTable->setItem(i, 0, new QTableWidgetItem(QString::number(p.id)));
        m_productTable->setItem(i, 1, new QTableWidgetItem(p.barcode));
        m_productTable->setItem(i, 2, new QTableWidgetItem(p.name));
        m_productTable->setItem(i, 3, new QTableWidgetItem(QString::number(p.stockQuantity)));
        m_productTable->setItem(i, 4, new QTableWidgetItem(QString::number(p.sellingPrice, 'f', 2)));
        
        QPushButton* delBtn = new QPushButton("Delete", this);
        delBtn->setStyleSheet("background-color: #F44336; color: white;");
        connect(delBtn, &QPushButton::clicked, this, [this, p]() {
            if (QMessageBox::question(this, "Confirm", "Delete product?") == QMessageBox::Yes) {
                m_controller->deleteProduct(p.id);
            }
        });
        m_productTable->setCellWidget(i, 5, delBtn);
    }
}

}

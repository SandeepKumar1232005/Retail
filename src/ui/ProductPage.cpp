#include "ProductPage.h"
#include "ProductDialog.h"
#include "../controllers/ProductController.h"
#include "../database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

namespace RetailMS {

ProductPage::ProductPage(std::shared_ptr<ProductController> controller, QWidget* parent)
    : QWidget(parent), m_controller(std::move(controller)) {
    setupUi();
    setupConnections();
    refreshTable();
}

void ProductPage::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    
    // Header Section
    QLabel* pageTitle = new QLabel("Product Management", this);
    pageTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #FFFFFF;");
    mainLayout->addWidget(pageTitle);
    
    // Top Action Bar
    QWidget* topBar = new QWidget(this);
    topBar->setObjectName("statCard"); // Use card background
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    
    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Search by barcode or name...");
    m_searchInput->setFixedWidth(300);
    
    m_categoryFilter = new QComboBox(this);
    m_categoryFilter->addItems({"All Categories", "Groceries", "Beverages", "Electronics", "Clothing"});
    m_categoryFilter->setFixedWidth(200);
    
    m_addProductButton = new QPushButton("Add Product", this);
    m_addProductButton->setObjectName("primaryButton");
    m_addProductButton->setCursor(Qt::PointingHandCursor);
    
    m_importButton = new QPushButton("Import CSV", this);
    m_importButton->setCursor(Qt::PointingHandCursor);
    m_exportButton = new QPushButton("Export CSV", this);
    m_exportButton->setCursor(Qt::PointingHandCursor);
    
    topLayout->addWidget(m_searchInput);
    topLayout->addWidget(m_categoryFilter);
    topLayout->addStretch();
    topLayout->addWidget(m_importButton);
    topLayout->addWidget(m_exportButton);
    topLayout->addWidget(m_addProductButton);
    
    mainLayout->addWidget(topBar);
    
    // Product Table
    m_productTable = new QTableWidget(this);
    m_productTable->setColumnCount(7);
    m_productTable->setHorizontalHeaderLabels({"Image", "Barcode", "Name", "Category", "Stock", "Price", "Actions"});
    m_productTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_productTable->setColumnWidth(6, 250);
    m_productTable->verticalHeader()->setDefaultSectionSize(60); // Taller rows for images
    mainLayout->addWidget(m_productTable, 1);
}

void ProductPage::setupConnections() {
    connect(m_searchInput, &QLineEdit::textChanged, this, &ProductPage::refreshTable);
    connect(m_categoryFilter, &QComboBox::currentTextChanged, this, &ProductPage::refreshTable);
    
    connect(m_controller.get(), &ProductController::productListChanged, this, &ProductPage::refreshTable);
    
    connect(m_addProductButton, &QPushButton::clicked, this, [this]() {
        ProductDialog dialog(nullptr, this);
        if (dialog.exec() == QDialog::Accepted) {
            Product p = dialog.getProduct();
            m_controller->saveProduct(p);
        }
    });
    
    connect(m_importButton, &QPushButton::clicked, this, &ProductPage::onImportCsv);
    connect(m_exportButton, &QPushButton::clicked, this, &ProductPage::onExportCsv);
    
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
    QString catFilter = m_categoryFilter->currentText();
    
    // Fetch and filter products based on the selected category in memory
    std::vector<std::pair<Product, QString>> filtered;
    for (const auto& p : products) {
        QString catName = "General";
        if (p.categoryId > 0) {
            auto catRes = DatabaseManager::instance().executeScalar("SELECT name FROM categories WHERE id = ?;", { p.categoryId });
            if (catRes && !catRes->isNull()) {
                catName = catRes->toString();
            }
        }
        
        if (catFilter == "All Categories" || catFilter == catName) {
            filtered.push_back({p, catName});
        }
    }
    
    m_productTable->setRowCount(filtered.size());
    for (size_t i = 0; i < filtered.size(); ++i) {
        const auto& p = filtered[i].first;
        const auto& catName = filtered[i].second;
        
        QLabel* imgLabel = new QLabel(this);
        imgLabel->setAlignment(Qt::AlignCenter);
        imgLabel->setText("📦"); // Placeholder for actual image
        m_productTable->setCellWidget(i, 0, imgLabel);
        
        m_productTable->setItem(i, 1, new QTableWidgetItem(p.barcode));
        m_productTable->setItem(i, 2, new QTableWidgetItem(p.name));
        m_productTable->setItem(i, 3, new QTableWidgetItem(catName));
        
        QTableWidgetItem* stockItem = new QTableWidgetItem(QString::number(p.stockQuantity));
        if (p.stockQuantity < 10) {
            stockItem->setForeground(QBrush(QColor("#EF4444"))); // Low stock warning
        }
        m_productTable->setItem(i, 4, stockItem);
        
        m_productTable->setItem(i, 5, new QTableWidgetItem(QString("₹%1").arg(p.sellingPrice, 0, 'f', 2)));
        
        QWidget* actionsWidget = new QWidget(this);
        QHBoxLayout* actionLayout = new QHBoxLayout(actionsWidget);
        actionLayout->setContentsMargins(10, 5, 10, 5);
        actionLayout->setSpacing(10);

        QPushButton* editBtn = new QPushButton("  Edit  ", this);
        editBtn->setMinimumWidth(80);
        editBtn->setMinimumHeight(32);
        editBtn->setStyleSheet("background-color: #4CAF50; color: white; padding: 6px 20px; border-radius: 4px; font-weight: bold; font-size: 13px;");
        editBtn->setCursor(Qt::PointingHandCursor);
        connect(editBtn, &QPushButton::clicked, this, [this, p]() {
            ProductDialog dialog(&p, this);
            if (dialog.exec() == QDialog::Accepted) {
                Product updated = dialog.getProduct();
                m_controller->saveProduct(updated);
            }
        });

        QPushButton* delBtn = new QPushButton("  Delete  ", this);
        delBtn->setMinimumWidth(80);
        delBtn->setMinimumHeight(32);
        delBtn->setStyleSheet("background-color: #EF4444; color: white; padding: 6px 20px; border-radius: 4px; font-weight: bold; font-size: 13px;");
        delBtn->setCursor(Qt::PointingHandCursor);
        connect(delBtn, &QPushButton::clicked, this, [this, p]() {
            if (QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this product?") == QMessageBox::Yes) {
                m_controller->deleteProduct(p.id);
            }
        });

        actionLayout->addWidget(editBtn);
        actionLayout->addWidget(delBtn);
        m_productTable->setCellWidget(i, 6, actionsWidget);
    }
}

void ProductPage::onImportCsv() {
    QString filePath = QFileDialog::getOpenFileName(this, "Import Products", "", "CSV Files (*.csv)");
    if (filePath.isEmpty()) return;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file for reading.");
        return;
    }
    
    QTextStream in(&file);
    int importedCount = 0;
    
    // Check if categories are seeded. We will map category strings to IDs.
    auto& db = DatabaseManager::instance();
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) continue;
        
        QStringList fields = line.split(',');
        if (fields.size() < 4) continue; // Barcode, Name, CostPrice, SellingPrice, Stock, Category
        
        Product p;
        p.barcode = fields[0].trimmed();
        p.sku = "SKU_" + p.barcode;
        p.name = fields[1].trimmed();
        p.costPrice = fields[2].toDouble();
        p.sellingPrice = fields[3].toDouble();
        p.stockQuantity = (fields.size() > 4) ? fields[4].toDouble() : 100.0;
        
        // Handle Category Name mapping to ID
        QString categoryName = (fields.size() > 5) ? fields[5].trimmed() : "Groceries";
        int categoryId = 2; // Default to Groceries
        auto catRes = db.executeScalar("SELECT id FROM categories WHERE name = ? LIMIT 1;", { categoryName });
        if (catRes && !catRes->isNull()) {
            categoryId = catRes->toInt();
        } else {
            // Insert category dynamically
            db.executeNonQuery("INSERT INTO categories (name, description) VALUES (?, ?);", { categoryName, "Dynamically created category" });
            auto newCatRes = db.executeScalar("SELECT id FROM categories WHERE name = ? LIMIT 1;", { categoryName });
            if (newCatRes && !newCatRes->isNull()) {
                categoryId = newCatRes->toInt();
            }
        }
        p.categoryId = categoryId;
        
        m_controller->saveProduct(p);
        importedCount++;
    }
    
    QMessageBox::information(this, "Success", QString("Successfully imported %1 products from CSV.").arg(importedCount));
}

void ProductPage::onExportCsv() {
    QString filePath = QFileDialog::getSaveFileName(this, "Export Products", "products.csv", "CSV Files (*.csv)");
    if (filePath.isEmpty()) return;
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file for writing.");
        return;
    }
    
    QTextStream out(&file);
    auto products = m_controller->searchProducts("");
    
    // Header
    out << "Barcode,Name,CostPrice,SellingPrice,StockQuantity,Category\n";
    
    for (const auto& p : products) {
        QString catName = "General";
        if (p.categoryId > 0) {
            auto catRes = DatabaseManager::instance().executeScalar("SELECT name FROM categories WHERE id = ?;", { p.categoryId });
            if (catRes && !catRes->isNull()) {
                catName = catRes->toString();
            }
        }
        out << p.barcode << "," 
            << p.name << "," 
            << p.costPrice << "," 
            << p.sellingPrice << "," 
            << p.stockQuantity << ","
            << catName << "\n";
    }
    
    QMessageBox::information(this, "Success", "Products exported to CSV successfully.");
}

} // namespace RetailMS

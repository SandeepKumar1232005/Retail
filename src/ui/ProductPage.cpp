#include "ProductPage.h"
#include "ProductDialog.h"
#include "../controllers/ProductController.h"
#include "../database/DatabaseManager.h"
#include "../services/SessionManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMenu>

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
    m_categoryFilter->addItem("All Categories", -1);
    
    // Load categories dynamically from database
    auto catQuery = DatabaseManager::instance().execute("SELECT id, name FROM categories ORDER BY name");
    while (catQuery.next()) {
        m_categoryFilter->addItem(catQuery.value("name").toString(), catQuery.value("id").toInt());
    }
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
    
    if (SessionManager::instance().isLoggedIn() && !SessionManager::instance().currentUser().isAdmin()) {
        m_importButton->hide();
        m_exportButton->hide();
        m_addProductButton->hide();
    }
    
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
    
    // Refresh categories in case new ones were added
    m_categoryFilter->blockSignals(true);
    m_categoryFilter->clear();
    m_categoryFilter->addItem("All Categories", -1);
    auto catQuery = DatabaseManager::instance().execute("SELECT id, name FROM categories ORDER BY name");
    while (catQuery.next()) {
        m_categoryFilter->addItem(catQuery.value("name").toString(), catQuery.value("id").toInt());
    }
    m_categoryFilter->setCurrentText(catFilter);
    m_categoryFilter->blockSignals(false);
    
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
        
        if (SessionManager::instance().isLoggedIn() && SessionManager::instance().currentUser().isAdmin()) {
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

            QPushButton* deleteBtn = new QPushButton("  Delete  ", this);
            deleteBtn->setMinimumWidth(80);
            deleteBtn->setMinimumHeight(32);
            deleteBtn->setStyleSheet("background-color: #EF4444; color: white; padding: 6px 20px; border-radius: 4px; font-weight: bold; font-size: 13px;");
            deleteBtn->setCursor(Qt::PointingHandCursor);
            connect(deleteBtn, &QPushButton::clicked, this, [this, p]() {
                if (QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this product?") == QMessageBox::Yes) {
                    m_controller->deleteProduct(p.id);
                }
            });
            
            actionLayout->addWidget(editBtn);
            actionLayout->addWidget(deleteBtn);
        } else {
            QLabel* lbl = new QLabel("Read-Only", this);
            lbl->setAlignment(Qt::AlignCenter);
            actionLayout->addWidget(lbl);
        }
        
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
    int addedCount = 0;
    int updatedCount = 0;
    
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
        
        // If the CSV provides a category explicitly, set it so the backend can resolve it
        if (fields.size() > 5 && !fields[5].trimmed().isEmpty()) {
            p.categoryName = fields[5].trimmed();
        }
        

        auto existingOpt = m_controller->getProductByBarcode(p.barcode);
        if (existingOpt.has_value()) {
            Product existing = existingOpt.value();
            existing.stockQuantity += p.stockQuantity;
            m_controller->saveProduct(existing, false);
            updatedCount++;
        } else {
            m_controller->saveProduct(p, false);
            addedCount++;
        }
    }
    
    m_controller->notifyProductsUpdated();
    QMessageBox::information(this, "Success", QString("Import complete: %1 added, %2 updated.")
                                              .arg(addedCount).arg(updatedCount));
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

#include "InventoryPage.h"
#include "../controllers/BillingController.h"
#include "../controllers/ProductController.h"
#include "../database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSqlQuery>
#include <QLabel>

namespace RetailMS {

InventoryPage::InventoryPage(std::shared_ptr<BillingController> billingController,
                             std::shared_ptr<ProductController> productController,
                             QWidget* parent)
    : QWidget(parent),
      m_billingController(std::move(billingController)),
      m_productController(std::move(productController)) {
    setupUi();
    setupConnections();
    refreshData();
}

void InventoryPage::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Title
    QLabel* titleLabel = new QLabel("Inventory Stock Levels", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #FFFFFF;");
    mainLayout->addWidget(titleLabel);

    // Filter controls
    QHBoxLayout* filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(15);

    QLabel* searchLabel = new QLabel("Search:", this);
    m_searchField = new QLineEdit(this);
    m_searchField->setPlaceholderText("Search by barcode or product name...");
    m_searchField->setFixedWidth(300);

    QLabel* statusLabel = new QLabel("Status:", this);
    m_statusFilter = new QComboBox(this);
    m_statusFilter->addItem("All Items");
    m_statusFilter->addItem("Healthy Stock");
    m_statusFilter->addItem("Low Stock");
    m_statusFilter->addItem("Out of Stock");
    m_statusFilter->setFixedWidth(150);

    m_refreshButton = new QPushButton("Refresh", this);
    m_refreshButton->setObjectName("primaryButton");
    m_refreshButton->setFixedWidth(100);

    filterLayout->addWidget(searchLabel);
    filterLayout->addWidget(m_searchField);
    filterLayout->addWidget(statusLabel);
    filterLayout->addWidget(m_statusFilter);
    filterLayout->addWidget(m_refreshButton);
    filterLayout->addStretch();

    mainLayout->addLayout(filterLayout);

    // Table
    m_inventoryTable = new QTableWidget(this);
    m_inventoryTable->setColumnCount(7);
    m_inventoryTable->setHorizontalHeaderLabels({"Barcode", "Product Name", "Category", "Current Stock", "Min Threshold", "Value (Cost)", "Status"});
    m_inventoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_inventoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_inventoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_inventoryTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_inventoryTable->verticalHeader()->setVisible(false);
    m_inventoryTable->setStyleSheet(
        "QTableWidget { background-color: #1E2025; border: 1px solid #2A2D35; border-radius: 8px; }"
    );

    mainLayout->addWidget(m_inventoryTable, 1);
}

void InventoryPage::setupConnections() {
    connect(m_refreshButton, &QPushButton::clicked, this, &InventoryPage::refreshData);
    connect(m_searchField, &QLineEdit::textChanged, this, &InventoryPage::refreshData);
    connect(m_statusFilter, &QComboBox::currentIndexChanged, this, &InventoryPage::refreshData);

    // Refresh immediately when bill completes
    connect(m_billingController.get(), &BillingController::checkoutComplete, this, &InventoryPage::refreshData);
    // Refresh immediately when product changes occur (e.g. addition, adjustment)
    connect(m_productController.get(), &ProductController::productListChanged, this, &InventoryPage::refreshData);
}

void InventoryPage::refreshData() {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    QString sql = "SELECT p.barcode, p.name, c.name AS category_name, p.stock_quantity, p.min_stock, p.cost_price "
                  "FROM products p "
                  "LEFT JOIN categories c ON p.category_id = c.id "
                  "WHERE p.is_active = 1";
    
    QVariantList params;
    QString search = m_searchField->text().trimmed();
    if (!search.isEmpty()) {
        sql += " AND (p.barcode LIKE ? OR p.name LIKE ?)";
        QString wildcard = "%" + search + "%";
        params << wildcard << wildcard;
    }

    int filterIdx = m_statusFilter->currentIndex();
    if (filterIdx == 1) { // Healthy Stock
        sql += " AND p.stock_quantity > p.min_stock";
    } else if (filterIdx == 2) { // Low Stock
        sql += " AND p.stock_quantity <= p.min_stock AND p.stock_quantity > 0";
    } else if (filterIdx == 3) { // Out of Stock
        sql += " AND p.stock_quantity <= 0";
    }

    sql += " ORDER BY p.stock_quantity ASC, p.name ASC;";

    m_inventoryTable->setRowCount(0);

    QSqlQuery query = db.prepare(sql);
    for (const auto& p : params) {
        query.addBindValue(p);
    }

    if (query.exec()) {
        while (query.next()) {
            int row = m_inventoryTable->rowCount();
            m_inventoryTable->insertRow(row);

            double stock = query.value("stock_quantity").toDouble();
            double min = query.value("min_stock").toDouble();
            double cost = query.value("cost_price").toDouble();
            double totalVal = stock * cost;

            m_inventoryTable->setItem(row, 0, new QTableWidgetItem(query.value("barcode").toString()));
            m_inventoryTable->setItem(row, 1, new QTableWidgetItem(query.value("name").toString()));
            m_inventoryTable->setItem(row, 2, new QTableWidgetItem(query.value("category_name").toString()));
            m_inventoryTable->setItem(row, 3, new QTableWidgetItem(QString::number(stock)));
            m_inventoryTable->setItem(row, 4, new QTableWidgetItem(QString::number(min)));
            m_inventoryTable->setItem(row, 5, new QTableWidgetItem(QString("₹%1").arg(totalVal, 0, 'f', 2)));

            QTableWidgetItem* statusItem = new QTableWidgetItem();
            if (stock <= 0) {
                statusItem->setText("OUT OF STOCK");
                statusItem->setForeground(QBrush(QColor("#EF4444")));
            } else if (stock <= min) {
                statusItem->setText("LOW STOCK");
                statusItem->setForeground(QBrush(QColor("#F59E0B")));
            } else {
                statusItem->setText("HEALTHY");
                statusItem->setForeground(QBrush(QColor("#22C55E")));
            }
            m_inventoryTable->setItem(row, 6, statusItem);
        }
    }
}

} // namespace RetailMS

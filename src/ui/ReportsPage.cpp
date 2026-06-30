#include "ReportsPage.h"
#include "../controllers/BillingController.h"
#include "../database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSqlQuery>

namespace RetailMS {

ReportsPage::ReportsPage(std::shared_ptr<BillingController> controller, QWidget* parent)
    : QWidget(parent), m_controller(std::move(controller)) {
    setupUi();
    setupConnections();
    refreshData();
}

void ReportsPage::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Header layout
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel("Sales & Revenue Reports", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #FFFFFF;");
    headerLayout->addWidget(titleLabel);
    
    headerLayout->addStretch();
    m_refreshButton = new QPushButton("Refresh Reports", this);
    m_refreshButton->setObjectName("primaryButton");
    m_refreshButton->setFixedWidth(140);
    headerLayout->addWidget(m_refreshButton);
    mainLayout->addLayout(headerLayout);

    // Tab Widget
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #2A2D35; background-color: #16181B; border-radius: 8px; }"
        "QTabBar::tab { background-color: #1E2025; color: #A9B1BC; padding: 12px 20px; font-weight: bold; border-top-left-radius: 6px; border-top-right-radius: 6px; }"
        "QTabBar::tab:selected { background-color: #16181B; color: #4CAF50; border-bottom: 2px solid #4CAF50; }"
    );

    // Tab 1: Sales Summary
    QWidget* summaryTab = new QWidget(m_tabWidget);
    QVBoxLayout* summaryLayout = new QVBoxLayout(summaryTab);
    m_summaryTable = new QTableWidget(summaryTab);
    m_summaryTable->setColumnCount(4);
    m_summaryTable->setHorizontalHeaderLabels({"Period", "Total Revenue", "Total Orders", "Average Order Value"});
    m_summaryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_summaryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_summaryTable->verticalHeader()->setVisible(false);
    m_summaryTable->setStyleSheet("QTableWidget { background-color: #1E2025; border: none; }");
    summaryLayout->addWidget(m_summaryTable);
    m_tabWidget->addTab(summaryTab, "Sales & Revenue Summary");

    // Tab 2: Product Analytics
    QWidget* productTab = new QWidget(m_tabWidget);
    QVBoxLayout* productLayout = new QVBoxLayout(productTab);
    m_productTable = new QTableWidget(productTab);
    m_productTable->setColumnCount(4);
    m_productTable->setHorizontalHeaderLabels({"Barcode", "Product Name", "Quantity Sold", "Total Revenue"});
    m_productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_productTable->verticalHeader()->setVisible(false);
    m_productTable->setStyleSheet("QTableWidget { background-color: #1E2025; border: none; }");
    productLayout->addWidget(m_productTable);
    m_tabWidget->addTab(productTab, "Product Sales Analytics");

    // Tab 3: Category Analytics
    QWidget* categoryTab = new QWidget(m_tabWidget);
    QVBoxLayout* categoryLayout = new QVBoxLayout(categoryTab);
    m_categoryTable = new QTableWidget(categoryTab);
    m_categoryTable->setColumnCount(3);
    m_categoryTable->setHorizontalHeaderLabels({"Category Name", "Quantity Sold", "Total Revenue"});
    m_categoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_categoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_categoryTable->verticalHeader()->setVisible(false);
    m_categoryTable->setStyleSheet("QTableWidget { background-color: #1E2025; border: none; }");
    categoryLayout->addWidget(m_categoryTable);
    m_tabWidget->addTab(categoryTab, "Category Sales Analytics");

    mainLayout->addWidget(m_tabWidget, 1);
}

void ReportsPage::setupConnections() {
    connect(m_refreshButton, &QPushButton::clicked, this, &ReportsPage::refreshData);
    
    // Auto-refresh reports when checkout completes successfully
    connect(m_controller.get(), &BillingController::checkoutComplete, this, &ReportsPage::refreshData);
}

void ReportsPage::refreshData() {
    loadSalesSummary();
    loadProductAnalytics();
    loadCategoryAnalytics();
}

void ReportsPage::loadSalesSummary() {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    m_summaryTable->setRowCount(0);

    auto addSummaryRow = [&](const QString& period, const QString& dateCondition) {
        QString sql = QString("SELECT SUM(grand_total), COUNT(*), AVG(grand_total) FROM invoices WHERE status = 'paid' AND %1;").arg(dateCondition);
        QSqlQuery query = db.execute(sql);
        if (query.next()) {
            int row = m_summaryTable->rowCount();
            m_summaryTable->insertRow(row);
            
            double totalRev = query.value(0).toDouble();
            int orderCount = query.value(1).toInt();
            double avgVal = query.value(2).toDouble();

            m_summaryTable->setItem(row, 0, new QTableWidgetItem(period));
            m_summaryTable->setItem(row, 1, new QTableWidgetItem(QString("₹%1").arg(totalRev, 0, 'f', 2)));
            m_summaryTable->setItem(row, 2, new QTableWidgetItem(QString::number(orderCount)));
            m_summaryTable->setItem(row, 3, new QTableWidgetItem(QString("₹%1").arg(avgVal, 0, 'f', 2)));
        }
    };

    addSummaryRow("Daily (Today)", "DATE(invoice_date) = DATE('now')");
    addSummaryRow("Weekly (Last 7 Days)", "DATE(invoice_date) >= DATE('now', '-7 days')");
    addSummaryRow("Monthly (Last 30 Days)", "DATE(invoice_date) >= DATE('now', '-30 days')");
    addSummaryRow("Yearly (Last 365 Days)", "DATE(invoice_date) >= DATE('now', '-365 days')");
}

void ReportsPage::loadProductAnalytics() {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    m_productTable->setRowCount(0);

    QString sql = "SELECT barcode, product_name, SUM(quantity) as qty_sold, SUM(total) as revenue "
                  "FROM invoice_items ii "
                  "JOIN invoices i ON ii.invoice_id = i.id "
                  "WHERE i.status = 'paid' "
                  "GROUP BY product_id, product_name, barcode "
                  "ORDER BY qty_sold DESC;";

    QSqlQuery query = db.execute(sql);
    while (query.next()) {
        int row = m_productTable->rowCount();
        m_productTable->insertRow(row);
        m_productTable->setItem(row, 0, new QTableWidgetItem(query.value("barcode").toString()));
        m_productTable->setItem(row, 1, new QTableWidgetItem(query.value("product_name").toString()));
        m_productTable->setItem(row, 2, new QTableWidgetItem(QString::number(query.value("qty_sold").toDouble())));
        m_productTable->setItem(row, 3, new QTableWidgetItem(QString("₹%1").arg(query.value("revenue").toDouble(), 0, 'f', 2)));
    }
}

void ReportsPage::loadCategoryAnalytics() {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    m_categoryTable->setRowCount(0);

    QString sql = "SELECT c.name as category_name, SUM(ii.quantity) as qty_sold, SUM(ii.total) as revenue "
                  "FROM invoice_items ii "
                  "JOIN products p ON ii.product_id = p.id "
                  "JOIN categories c ON p.category_id = c.id "
                  "JOIN invoices i ON ii.invoice_id = i.id "
                  "WHERE i.status = 'paid' "
                  "GROUP BY c.id, c.name "
                  "ORDER BY revenue DESC;";

    QSqlQuery query = db.execute(sql);
    while (query.next()) {
        int row = m_categoryTable->rowCount();
        m_categoryTable->insertRow(row);
        m_categoryTable->setItem(row, 0, new QTableWidgetItem(query.value("category_name").toString()));
        m_categoryTable->setItem(row, 1, new QTableWidgetItem(QString::number(query.value("qty_sold").toDouble())));
        m_categoryTable->setItem(row, 2, new QTableWidgetItem(QString("₹%1").arg(query.value("revenue").toDouble(), 0, 'f', 2)));
    }
}

} // namespace RetailMS

#include "StaffManagementPage.h"
#include "../../database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDate>
#include <QDateTime>
#include <QScrollArea>
#include <QGraphicsLayout>
#include <QSplitter>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <algorithm>

namespace RetailMS {

// Custom Invoice Detail Dialog
class InvoiceDetailDialog : public QDialog {
public:
    InvoiceDetailDialog(const QString& invoiceNumber, QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Invoice Details - " + invoiceNumber);
        resize(850, 600);
        setStyleSheet("QDialog { background-color: #111315; }");
        
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        mainLayout->setSpacing(15);
        
        // Title
        auto* titleLabel = new QLabel("Invoice Details", this);
        titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #FFFFFF;");
        mainLayout->addWidget(titleLabel);
        
        // Grid details
        auto* detailsWidget = new QWidget(this);
        detailsWidget->setObjectName("statCard");
        detailsWidget->setStyleSheet("QWidget#statCard { background-color: #16181B; border: 1px solid #2A2D35; border-radius: 8px; }");
        auto* gridLayout = new QGridLayout(detailsWidget);
        gridLayout->setSpacing(10);
        
        auto addRow = [&](const QString& label, const QString& val, int r, int c) {
            auto* lbl = new QLabel(label, detailsWidget);
            lbl->setStyleSheet("color: #A9B1BC; font-weight: 500; font-size: 13px;");
            auto* v = new QLabel(val, detailsWidget);
            v->setStyleSheet("color: #FFFFFF; font-weight: bold; font-size: 13px;");
            gridLayout->addWidget(lbl, r, c * 2);
            gridLayout->addWidget(v, r, c * 2 + 1);
        };
        
        auto& db = DatabaseManager::instance();
        int invoiceId = -1;
        if (db.isConnected()) {
            QString sql = "SELECT i.id, i.invoice_number, i.customer_id, i.customer_name, i.customer_phone, "
                          "u.full_name AS cashier_name, i.payment_mode, i.invoice_date, i.subtotal, i.discount_amt, "
                          "i.coupon_discount, (i.cgst_amt + i.sgst_amt) AS tax, i.grand_total, i.status "
                          "FROM invoices i "
                          "LEFT JOIN users u ON i.user_id = u.id "
                          "WHERE i.invoice_number = ?;";
            QSqlQuery query = db.prepare(sql);
            query.addBindValue(invoiceNumber);
            if (query.exec() && query.next()) {
                invoiceId = query.value("id").toInt();
                QString custIdStr = query.value("customer_id").toInt() > 0 ? 
                                    QString::number(query.value("customer_id").toInt()) : "-";
                QString custName = query.value("customer_name").toString().isEmpty() ? "Walk-in" : query.value("customer_name").toString();
                QString custPhone = query.value("customer_phone").toString().isEmpty() ? "-" : query.value("customer_phone").toString();
                
                addRow("Invoice Number:", query.value("invoice_number").toString(), 0, 0);
                addRow("Customer ID:", custIdStr, 0, 1);
                addRow("Customer Name:", custName, 1, 0);
                addRow("Mobile Number:", custPhone, 1, 1);
                addRow("Cashier Name:", query.value("cashier_name").toString(), 2, 0);
                addRow("Billing Time:", query.value("invoice_date").toDateTime().toString("yyyy-MM-dd HH:mm:ss"), 2, 1);
                addRow("Payment Method:", query.value("payment_mode").toString().toUpper(), 3, 0);
                addRow("Grand Total:", QString("₹%1").arg(query.value("grand_total").toDouble(), 0, 'f', 2), 3, 1);
            }
        }
        mainLayout->addWidget(detailsWidget);
        
        // Table label
        auto* itemsLabel = new QLabel("Product List", this);
        itemsLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #4CAF50;");
        mainLayout->addWidget(itemsLabel);
        
        // Table of items
        auto* table = new QTableWidget(this);
        table->setColumnCount(6);
        table->setHorizontalHeaderLabels({"Product Name", "Qty", "Price", "GST", "Discount", "Total"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->verticalHeader()->setVisible(false);
        table->setStyleSheet("QTableWidget { background-color: #1E2025; border: 1px solid #2A2D35; border-radius: 8px; }");
        
        if (invoiceId > 0 && db.isConnected()) {
            QString itemSql = "SELECT product_name, quantity, selling_price, (cgst_rate + sgst_rate) AS gst_rate, discount_amt, total FROM invoice_items WHERE invoice_id = ?;";
            QSqlQuery itemQuery = db.prepare(itemSql);
            itemQuery.addBindValue(invoiceId);
            if (itemQuery.exec()) {
                while (itemQuery.next()) {
                    int r = table->rowCount();
                    table->insertRow(r);
                    table->setItem(r, 0, new QTableWidgetItem(itemQuery.value("product_name").toString()));
                    table->setItem(r, 1, new QTableWidgetItem(QString::number(itemQuery.value("quantity").toDouble())));
                    table->setItem(r, 2, new QTableWidgetItem(QString("₹%1").arg(itemQuery.value("selling_price").toDouble(), 0, 'f', 2)));
                    table->setItem(r, 3, new QTableWidgetItem(QString("%1%").arg(itemQuery.value("gst_rate").toDouble())));
                    table->setItem(r, 4, new QTableWidgetItem(QString("₹%1").arg(itemQuery.value("discount_amt").toDouble(), 0, 'f', 2)));
                    table->setItem(r, 5, new QTableWidgetItem(QString("₹%1").arg(itemQuery.value("total").toDouble(), 0, 'f', 2)));
                }
            }
        }
        mainLayout->addWidget(table, 1);
        
        // Close button box
        auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::accept);
        mainLayout->addWidget(buttonBox);
    }
};

StaffManagementPage::StaffManagementPage(std::shared_ptr<BillingController> billingController, QWidget* parent)
    : QWidget(parent), m_billingController(std::move(billingController)) {
    
    // Ensure index on invoices(user_id) exists for optimal database queries
    auto& db = DatabaseManager::instance();
    if (db.isConnected()) {
        db.execute("CREATE INDEX IF NOT EXISTS idx_invoices_user ON invoices(user_id);");
    }

    setupUi();
    setupConnections();
    refreshData();
}

void StaffManagementPage::setupUi() {
    auto* pageLayout = new QVBoxLayout(this);
    pageLayout->setContentsMargins(20, 20, 20, 20);
    pageLayout->setSpacing(20);

    // --- SUMMARY CARDS ---
    auto* cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(15);

    m_cardTotalStaff = new StatCardWidget("Total Staff", "0", "Registered accounts", this);
    m_cardActiveStaff = new StatCardWidget("Active Staff", "0", "Currently active", this);
    m_cardInactiveStaff = new StatCardWidget("Inactive Staff", "0", "Deactivated", this);
    m_cardBillsToday = new StatCardWidget("Bills Today", "0", "Generated today", this);
    m_cardRevenueToday = new StatCardWidget("Today's Revenue", "₹0.00", "Total revenue today", this);

    cardsLayout->addWidget(m_cardTotalStaff);
    cardsLayout->addWidget(m_cardActiveStaff);
    cardsLayout->addWidget(m_cardInactiveStaff);
    cardsLayout->addWidget(m_cardBillsToday);
    cardsLayout->addWidget(m_cardRevenueToday);
    pageLayout->addLayout(cardsLayout);

    // --- CONTENT: SPLIT VIEW (LEFT TABLE & RIGHT DETAILS) ---
    auto* contentWidget = new QWidget(this);
    auto* contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(20);

    // --- LEFT SIDE: SEARCH, FILTERS & STAFF LIST ---
    auto* leftWidget = new QWidget(this);
    auto* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(15);

    // Filters row
    auto* filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(10);

    m_searchField = new QLineEdit(this);
    m_searchField->setPlaceholderText("Search Staff (ID, Name, Username, Phone)...");
    m_searchField->setFixedWidth(260);

    m_statusFilter = new QComboBox(this);
    m_statusFilter->addItems({"All Statuses", "Active", "Inactive"});
    m_statusFilter->setFixedWidth(120);

    m_roleFilter = new QComboBox(this);
    m_roleFilter->addItems({"All Roles", "Admin", "Manager", "Cashier"});
    m_roleFilter->setFixedWidth(110);

    m_sortOrder = new QComboBox(this);
    m_sortOrder->addItems({"Sort By: Newest", "Sort By: Oldest", "Sort By: Highest Revenue", "Sort By: Most Bills", "Sort By: Today's Sales"});
    m_sortOrder->setFixedWidth(180);

    m_refreshButton = new QPushButton("Refresh", this);
    m_refreshButton->setObjectName("primaryButton");
    m_refreshButton->setFixedWidth(80);

    filterLayout->addWidget(m_searchField);
    filterLayout->addWidget(m_statusFilter);
    filterLayout->addWidget(m_roleFilter);
    filterLayout->addWidget(m_sortOrder);
    filterLayout->addWidget(m_refreshButton);
    filterLayout->addStretch();
    leftLayout->addLayout(filterLayout);

    // Staff Table
    m_staffTable = new QTableWidget(this);
    m_staffTable->setColumnCount(13);
    m_staffTable->setHorizontalHeaderLabels({
        "Staff ID", "Name", "Username", "Email", "Phone", "Role", "Status",
        "Joined", "Bills", "Total Sales", "Today's Sales", "Last Login", "Last Bill"
    });
    m_staffTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_staffTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // Name stretches
    m_staffTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_staffTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_staffTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_staffTable->verticalHeader()->setVisible(false);
    leftLayout->addWidget(m_staffTable, 1);

    contentLayout->addWidget(leftWidget, 6); // 60% width

    // --- RIGHT SIDE: DETAILED VIEW / OVERVIEW PANEL ---
    m_detailStack = new QStackedWidget(this);
    m_detailStack->setObjectName("statCard");
    m_detailStack->setStyleSheet("QStackedWidget#statCard { background-color: #16181B; border: 1px solid #2A2D35; border-radius: 12px; }");

    // Page 0: Performance Overview
    m_overviewWidget = new QWidget(m_detailStack);
    auto* overviewLayout = new QVBoxLayout(m_overviewWidget);
    overviewLayout->setContentsMargins(20, 20, 20, 20);
    overviewLayout->setSpacing(15);

    auto* overviewTitle = new QLabel("Staff Performance Overview", m_overviewWidget);
    overviewTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF;");
    overviewLayout->addWidget(overviewTitle);

    auto* overviewScroll = new QScrollArea(m_overviewWidget);
    overviewScroll->setWidgetResizable(true);
    overviewScroll->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");
    
    auto* overviewScrollContent = new QWidget(overviewScroll);
    auto* scrollLayout = new QVBoxLayout(overviewScrollContent);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setSpacing(20);

    m_overviewBillsChart = new QChartView(overviewScrollContent);
    m_overviewSalesChart = new QChartView(overviewScrollContent);
    m_overviewPaymentChart = new QChartView(overviewScrollContent);
    m_overviewCustomersChart = new QChartView(overviewScrollContent);

    m_overviewBillsChart->setMinimumHeight(240);
    m_overviewSalesChart->setMinimumHeight(240);
    m_overviewPaymentChart->setMinimumHeight(240);
    m_overviewCustomersChart->setMinimumHeight(240);

    scrollLayout->addWidget(new QLabel("Bills per Day", overviewScrollContent));
    scrollLayout->addWidget(m_overviewBillsChart);
    scrollLayout->addWidget(new QLabel("Sales per Day", overviewScrollContent));
    scrollLayout->addWidget(m_overviewSalesChart);
    scrollLayout->addWidget(new QLabel("Payment Method Distribution", overviewScrollContent));
    scrollLayout->addWidget(m_overviewPaymentChart);
    scrollLayout->addWidget(new QLabel("Top Customers Served", overviewScrollContent));
    scrollLayout->addWidget(m_overviewCustomersChart);

    overviewScroll->setWidget(overviewScrollContent);
    overviewLayout->addWidget(overviewScroll);
    m_detailStack->addWidget(m_overviewWidget);

    // Page 1: Staff Profile Details & History
    m_profileWidget = new QWidget(m_detailStack);
    auto* profileLayout = new QVBoxLayout(m_profileWidget);
    profileLayout->setContentsMargins(20, 20, 20, 20);
    profileLayout->setSpacing(15);

    // Header with profile title & Back/Close button
    auto* profileHeaderLayout = new QHBoxLayout();
    auto* profileTitle = new QLabel("Staff Profile Details", m_profileWidget);
    profileTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF;");
    auto* closeProfileBtn = new QPushButton("Close Profile", m_profileWidget);
    closeProfileBtn->setFixedWidth(100);
    profileHeaderLayout->addWidget(profileTitle);
    profileHeaderLayout->addStretch();
    profileHeaderLayout->addWidget(closeProfileBtn);
    profileLayout->addLayout(profileHeaderLayout);

    // Profile details grid
    auto* detailsGrid = new QWidget(m_profileWidget);
    detailsGrid->setObjectName("statCard");
    detailsGrid->setStyleSheet("QWidget#statCard { background-color: #1E2025; border: 1px solid #2A2D35; border-radius: 8px; }");
    auto* grid = new QGridLayout(detailsGrid);
    grid->setSpacing(10);

    auto addLabelVal = [&](const QString& label, QLabel*& valueLabel, int r, int c) {
        auto* l = new QLabel(label, detailsGrid);
        l->setStyleSheet("color: #A9B1BC; font-weight: 500; font-size: 13px;");
        valueLabel = new QLabel("-", detailsGrid);
        valueLabel->setStyleSheet("color: #FFFFFF; font-weight: bold; font-size: 13px;");
        grid->addWidget(l, r, c * 2);
        grid->addWidget(valueLabel, r, c * 2 + 1);
    };

    addLabelVal("Staff ID:", m_detailId, 0, 0);
    addLabelVal("Name:", m_detailName, 1, 0);
    addLabelVal("Username:", m_detailUsername, 2, 0);
    addLabelVal("Email:", m_detailEmail, 3, 0);
    addLabelVal("Phone:", m_detailPhone, 4, 0);

    addLabelVal("Role:", m_detailRole, 0, 1);
    addLabelVal("Status:", m_detailStatus, 1, 1);
    addLabelVal("Joined:", m_detailJoinDate, 2, 1);
    addLabelVal("Last Login:", m_detailLastLogin, 3, 1);

    addLabelVal("Total Bills:", m_detailTotalBills, 4, 1);
    addLabelVal("Total Revenue:", m_detailTotalRevenue, 5, 0);
    addLabelVal("Today's Revenue:", m_detailTodayRevenue, 5, 1);
    addLabelVal("Avg Bill Value:", m_detailAvgBill, 6, 0);

    profileLayout->addWidget(detailsGrid);

    // Tab Widget for history / charts
    auto* detailTabs = new QTabWidget(m_profileWidget);
    detailTabs->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #2A2D35; background-color: #1E2025; border-radius: 8px; }"
        "QTabBar::tab { background-color: #16181B; color: #A9B1BC; padding: 8px 16px; font-weight: bold; border-top-left-radius: 4px; border-top-right-radius: 4px; }"
        "QTabBar::tab:selected { background-color: #1E2025; color: #4CAF50; }"
    );

    // Tab A: Billing History
    auto* tabHistory = new QWidget(detailTabs);
    auto* tabHistoryLayout = new QVBoxLayout(tabHistory);
    m_billingHistoryTable = new QTableWidget(tabHistory);
    m_billingHistoryTable->setColumnCount(10);
    m_billingHistoryTable->setHorizontalHeaderLabels({
        "Invoice #", "Cust ID", "Cust Name", "Cust Mobile", "Date", "Time", "Payment", "Items", "Amount", "Status"
    });
    m_billingHistoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_billingHistoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_billingHistoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_billingHistoryTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_billingHistoryTable->verticalHeader()->setVisible(false);
    m_billingHistoryTable->setStyleSheet("QTableWidget { background-color: #1E2025; border: none; }");
    tabHistoryLayout->addWidget(m_billingHistoryTable);
    detailTabs->addTab(tabHistory, "Billing History");

    // Tab B: Performance Analytics (Charts specific to this staff)
    auto* tabCharts = new QWidget(detailTabs);
    auto* tabChartsLayout = new QVBoxLayout(tabCharts);
    
    auto* staffScroll = new QScrollArea(tabCharts);
    staffScroll->setWidgetResizable(true);
    staffScroll->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");
    
    auto* staffScrollContent = new QWidget(staffScroll);
    auto* staffScrollLayout = new QVBoxLayout(staffScrollContent);
    staffScrollLayout->setContentsMargins(0, 0, 0, 0);
    staffScrollLayout->setSpacing(20);

    m_staffBillsChart = new QChartView(staffScrollContent);
    m_staffSalesChart = new QChartView(staffScrollContent);
    m_staffPaymentChart = new QChartView(staffScrollContent);
    m_staffCustomersChart = new QChartView(staffScrollContent);

    m_staffBillsChart->setMinimumHeight(220);
    m_staffSalesChart->setMinimumHeight(220);
    m_staffPaymentChart->setMinimumHeight(220);
    m_staffCustomersChart->setMinimumHeight(220);

    staffScrollLayout->addWidget(new QLabel("Bills per Day", staffScrollContent));
    staffScrollLayout->addWidget(m_staffBillsChart);
    staffScrollLayout->addWidget(new QLabel("Sales per Day", staffScrollContent));
    staffScrollLayout->addWidget(m_staffSalesChart);
    staffScrollLayout->addWidget(new QLabel("Payment Method Distribution", staffScrollContent));
    staffScrollLayout->addWidget(m_staffPaymentChart);
    staffScrollLayout->addWidget(new QLabel("Top Customers Served", staffScrollContent));
    staffScrollLayout->addWidget(m_staffCustomersChart);

    staffScroll->setWidget(staffScrollContent);
    tabChartsLayout->addWidget(staffScroll);
    detailTabs->addTab(tabCharts, "Performance Analytics");

    profileLayout->addWidget(detailTabs, 1);

    m_detailStack->addWidget(m_profileWidget);
    contentLayout->addWidget(m_detailStack, 4); // 40% width

    pageLayout->addWidget(contentWidget, 1);

    connect(closeProfileBtn, &QPushButton::clicked, this, [this]() {
        m_staffTable->clearSelection();
    });
}

void StaffManagementPage::setupConnections() {
    connect(m_refreshButton, &QPushButton::clicked, this, &StaffManagementPage::refreshData);
    connect(m_searchField, &QLineEdit::textChanged, this, &StaffManagementPage::refreshData);
    connect(m_statusFilter, &QComboBox::currentIndexChanged, this, &StaffManagementPage::refreshData);
    connect(m_roleFilter, &QComboBox::currentIndexChanged, this, &StaffManagementPage::refreshData);
    connect(m_sortOrder, &QComboBox::currentIndexChanged, this, &StaffManagementPage::refreshData);

    connect(m_staffTable, &QTableWidget::itemSelectionChanged, this, &StaffManagementPage::onStaffSelected);
    connect(m_billingHistoryTable, &QTableWidget::cellDoubleClicked, this, &StaffManagementPage::onInvoiceDoubleClicked);
    
    // Auto-refresh when a checkout happens
    connect(m_billingController.get(), &BillingController::checkoutComplete, this, &StaffManagementPage::refreshData);
}

void StaffManagementPage::refreshData() {
    updateSummaryCards();
    loadStaffList();
    
    if (m_selectedUserId > 0) {
        loadStaffDetails(m_selectedUserId);
        loadStaffCharts(m_selectedUserId);
    } else {
        m_detailStack->setCurrentIndex(0); // Show Overview page
        loadOverviewCharts();
    }
}

void StaffManagementPage::updateSummaryCards() {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    // Total staff count
    auto totalCount = db.executeScalar("SELECT COUNT(*) FROM users;");
    m_cardTotalStaff->setValue(totalCount ? totalCount->toString() : "0");

    // Active staff count
    auto activeCount = db.executeScalar("SELECT COUNT(*) FROM users WHERE is_active = 1;");
    m_cardActiveStaff->setValue(activeCount ? activeCount->toString() : "0");

    // Inactive staff count
    auto inactiveCount = db.executeScalar("SELECT COUNT(*) FROM users WHERE is_active = 0;");
    m_cardInactiveStaff->setValue(inactiveCount ? inactiveCount->toString() : "0");

    // Bills generated today
    auto billsToday = db.executeScalar("SELECT COUNT(*) FROM invoices WHERE DATE(invoice_date) = DATE('now') AND status = 'paid';");
    m_cardBillsToday->setValue(billsToday ? billsToday->toString() : "0");

    // Revenue generated today
    auto revenueToday = db.executeScalar("SELECT SUM(grand_total) FROM invoices WHERE DATE(invoice_date) = DATE('now') AND status = 'paid';");
    double rev = (revenueToday && !revenueToday->isNull()) ? revenueToday->toDouble() : 0.0;
    m_cardRevenueToday->setValue(QString("₹%1").arg(rev, 0, 'f', 2));
}

void StaffManagementPage::loadStaffList() {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    m_staffTable->setRowCount(0);

    // Build query with search & filter conditions
    QString sql = "SELECT u.id, u.username, u.full_name, u.email, u.phone, u.role, u.is_active, u.created_at, u.last_login, "
                  "(SELECT COUNT(*) FROM invoices WHERE user_id = u.id AND status = 'paid') AS total_bills, "
                  "(SELECT COALESCE(SUM(grand_total), 0) FROM invoices WHERE user_id = u.id AND status = 'paid') AS total_sales, "
                  "(SELECT COALESCE(SUM(grand_total), 0) FROM invoices WHERE user_id = u.id AND status = 'paid' AND DATE(invoice_date) = DATE('now')) AS todays_sales, "
                  "(SELECT MAX(invoice_date) FROM invoices WHERE user_id = u.id AND status = 'paid') AS last_bill_date "
                  "FROM users u "
                  "WHERE 1=1";

    QVariantList params;

    // Search condition
    QString search = m_searchField->text().trimmed();
    if (!search.isEmpty()) {
        sql += " AND (u.username LIKE ? OR u.full_name LIKE ? OR u.phone LIKE ? OR u.email LIKE ? OR CAST(u.id AS TEXT) LIKE ? OR printf('STF-%04d', u.id) LIKE ?)";
        QString wildcard = "%" + search + "%";
        params << wildcard << wildcard << wildcard << wildcard << wildcard << wildcard;
    }

    // Status filter
    int statusIdx = m_statusFilter->currentIndex();
    if (statusIdx == 1) { // Active
        sql += " AND u.is_active = 1";
    } else if (statusIdx == 2) { // Inactive
        sql += " AND u.is_active = 0";
    }

    // Role filter
    int roleIdx = m_roleFilter->currentIndex();
    if (roleIdx == 1) { // Admin
        sql += " AND u.role = 'admin'";
    } else if (roleIdx == 2) { // Manager
        sql += " AND u.role = 'manager'";
    } else if (roleIdx == 3) { // Cashier
        sql += " AND u.role = 'cashier'";
    }

    // Sorting
    int sortIdx = m_sortOrder->currentIndex();
    if (sortIdx == 0) { // Newest
        sql += " ORDER BY u.created_at DESC";
    } else if (sortIdx == 1) { // Oldest
        sql += " ORDER BY u.created_at ASC";
    } else if (sortIdx == 2) { // Highest Revenue
        sql += " ORDER BY total_sales DESC";
    } else if (sortIdx == 3) { // Most Bills
        sql += " ORDER BY total_bills DESC";
    } else if (sortIdx == 4) { // Today's Sales
        sql += " ORDER BY todays_sales DESC";
    }

    QSqlQuery query = db.prepare(sql);
    for (const auto& p : params) {
        query.addBindValue(p);
    }

    if (query.exec()) {
        while (query.next()) {
            int row = m_staffTable->rowCount();
            m_staffTable->insertRow(row);

            int id = query.value("id").toInt();
            QString staffIdFormatted = QString("STF-%1").arg(id, 4, 10, QChar('0'));
            
            auto* idItem = new QTableWidgetItem(staffIdFormatted);
            idItem->setData(Qt::UserRole, id); // Store real ID in first column item

            m_staffTable->setItem(row, 0, idItem);
            m_staffTable->setItem(row, 1, new QTableWidgetItem(query.value("full_name").toString()));
            m_staffTable->setItem(row, 2, new QTableWidgetItem(query.value("username").toString()));
            m_staffTable->setItem(row, 3, new QTableWidgetItem(query.value("email").toString().isEmpty() ? "-" : query.value("email").toString()));
            m_staffTable->setItem(row, 4, new QTableWidgetItem(query.value("phone").toString().isEmpty() ? "-" : query.value("phone").toString()));
            m_staffTable->setItem(row, 5, new QTableWidgetItem(query.value("role").toString().toUpper()));

            bool active = query.value("is_active").toInt() == 1;
            m_staffTable->setItem(row, 6, new QTableWidgetItem(active ? "ACTIVE" : "INACTIVE"));

            // Joining Date
            QString joinStr = query.value("created_at").toString();
            QDateTime joinDt = QDateTime::fromString(joinStr, Qt::ISODate);
            QString joinDisplay = joinDt.isValid() ? joinDt.toString("yyyy-MM-dd") : joinStr;
            m_staffTable->setItem(row, 7, new QTableWidgetItem(joinDisplay));

            m_staffTable->setItem(row, 8, new QTableWidgetItem(query.value("total_bills").toString()));
            m_staffTable->setItem(row, 9, new QTableWidgetItem(QString("₹%1").arg(query.value("total_sales").toDouble(), 0, 'f', 2)));
            m_staffTable->setItem(row, 10, new QTableWidgetItem(QString("₹%1").arg(query.value("todays_sales").toDouble(), 0, 'f', 2)));

            // Last Login
            QString loginStr = query.value("last_login").toString();
            QDateTime loginDt = QDateTime::fromString(loginStr, Qt::ISODate);
            QString loginDisplay = loginDt.isValid() ? loginDt.toString("yyyy-MM-dd HH:mm") : "-";
            m_staffTable->setItem(row, 11, new QTableWidgetItem(loginDisplay));

            // Last Bill Generated
            QString billStr = query.value("last_bill_date").toString();
            QDateTime billDt = QDateTime::fromString(billStr, "yyyy-MM-dd HH:mm:ss");
            if (!billDt.isValid()) billDt = QDateTime::fromString(billStr, Qt::ISODate);
            QString billDisplay = billDt.isValid() ? billDt.toString("yyyy-MM-dd HH:mm") : "-";
            m_staffTable->setItem(row, 12, new QTableWidgetItem(billDisplay));
        }
    }
}

void StaffManagementPage::onStaffSelected() {
    int row = m_staffTable->currentRow();
    if (row < 0) {
        m_selectedUserId = -1;
        m_detailStack->setCurrentIndex(0); // Switch back to Overview charts
        loadOverviewCharts();
        return;
    }

    auto* idItem = m_staffTable->item(row, 0);
    if (!idItem) return;

    m_selectedUserId = idItem->data(Qt::UserRole).toInt();
    m_detailStack->setCurrentIndex(1); // Show Profile detail panel
    loadStaffDetails(m_selectedUserId);
    loadStaffCharts(m_selectedUserId);
}

void StaffManagementPage::loadStaffDetails(int userId) {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    // 1. Fetch Profile info
    QString sql = "SELECT u.id, u.username, u.full_name, u.email, u.phone, u.role, u.is_active, u.created_at, u.last_login, "
                  "(SELECT COUNT(*) FROM invoices WHERE user_id = u.id AND status = 'paid') AS total_bills, "
                  "(SELECT COALESCE(SUM(grand_total), 0) FROM invoices WHERE user_id = u.id AND status = 'paid') AS total_sales, "
                  "(SELECT COALESCE(SUM(grand_total), 0) FROM invoices WHERE user_id = u.id AND status = 'paid' AND DATE(invoice_date) = DATE('now')) AS todays_sales "
                  "FROM users u "
                  "WHERE u.id = ?;";

    QSqlQuery query = db.prepare(sql);
    query.addBindValue(userId);

    if (query.exec() && query.next()) {
        m_detailId->setText(QString("STF-%1").arg(userId, 4, 10, QChar('0')));
        m_detailName->setText(query.value("full_name").toString());
        m_detailUsername->setText(query.value("username").toString());
        m_detailEmail->setText(query.value("email").toString().isEmpty() ? "-" : query.value("email").toString());
        m_detailPhone->setText(query.value("phone").toString().isEmpty() ? "-" : query.value("phone").toString());
        m_detailRole->setText(query.value("role").toString().toUpper());
        m_detailStatus->setText(query.value("is_active").toInt() == 1 ? "ACTIVE" : "INACTIVE");

        QString joinStr = query.value("created_at").toString();
        QDateTime joinDt = QDateTime::fromString(joinStr, Qt::ISODate);
        m_detailJoinDate->setText(joinDt.isValid() ? joinDt.toString("yyyy-MM-dd") : joinStr);

        QString loginStr = query.value("last_login").toString();
        QDateTime loginDt = QDateTime::fromString(loginStr, Qt::ISODate);
        m_detailLastLogin->setText(loginDt.isValid() ? loginDt.toString("yyyy-MM-dd HH:mm") : "-");

        int bills = query.value("total_bills").toInt();
        double sales = query.value("total_sales").toDouble();
        double todaySales = query.value("todays_sales").toDouble();
        double avgBill = bills > 0 ? (sales / bills) : 0.0;

        m_detailTotalBills->setText(QString::number(bills));
        m_detailTotalRevenue->setText(QString("₹%1").arg(sales, 0, 'f', 2));
        m_detailTodayRevenue->setText(QString("₹%1").arg(todaySales, 0, 'f', 2));
        m_detailAvgBill->setText(QString("₹%1").arg(avgBill, 0, 'f', 2));
    }

    // 2. Fetch Billing History list
    m_billingHistoryTable->setRowCount(0);
    QString histSql = "SELECT i.invoice_number, i.customer_id, i.customer_name, i.customer_phone, i.invoice_date, i.payment_mode, "
                      "i.grand_total, i.status, (SELECT SUM(quantity) FROM invoice_items WHERE invoice_id = i.id) as items_count "
                      "FROM invoices i "
                      "WHERE i.user_id = ? "
                      "ORDER BY i.invoice_date DESC;";
    
    QSqlQuery histQuery = db.prepare(histSql);
    histQuery.addBindValue(userId);
    if (histQuery.exec()) {
        while (histQuery.next()) {
            int row = m_billingHistoryTable->rowCount();
            m_billingHistoryTable->insertRow(row);

            m_billingHistoryTable->setItem(row, 0, new QTableWidgetItem(histQuery.value("invoice_number").toString()));
            
            int custId = histQuery.value("customer_id").toInt();
            m_billingHistoryTable->setItem(row, 1, new QTableWidgetItem(custId > 0 ? QString::number(custId) : "-"));
            
            QString custName = histQuery.value("customer_name").toString();
            m_billingHistoryTable->setItem(row, 2, new QTableWidgetItem(custName.isEmpty() ? "Walk-in" : custName));
            
            QString phone = histQuery.value("customer_phone").toString();
            m_billingHistoryTable->setItem(row, 3, new QTableWidgetItem(phone.isEmpty() ? "-" : phone));

            // Date & Time Split
            QDateTime dt = histQuery.value("invoice_date").toDateTime();
            m_billingHistoryTable->setItem(row, 4, new QTableWidgetItem(dt.isValid() ? dt.toString("yyyy-MM-dd") : "-"));
            m_billingHistoryTable->setItem(row, 5, new QTableWidgetItem(dt.isValid() ? dt.toString("HH:mm") : "-"));
            
            m_billingHistoryTable->setItem(row, 6, new QTableWidgetItem(histQuery.value("payment_mode").toString().toUpper()));
            m_billingHistoryTable->setItem(row, 7, new QTableWidgetItem(QString::number(histQuery.value("items_count").toDouble())));
            m_billingHistoryTable->setItem(row, 8, new QTableWidgetItem(QString("₹%1").arg(histQuery.value("grand_total").toDouble(), 0, 'f', 2)));
            m_billingHistoryTable->setItem(row, 9, new QTableWidgetItem(histQuery.value("status").toString().toUpper()));
        }
    }
}

void StaffManagementPage::onInvoiceDoubleClicked(int row, int column) {
    Q_UNUSED(column);
    auto* item = m_billingHistoryTable->item(row, 0);
    if (item) {
        showInvoiceDetailDialog(item->text());
    }
}

void StaffManagementPage::showInvoiceDetailDialog(const QString& invoiceNumber) {
    InvoiceDetailDialog dlg(invoiceNumber, this);
    dlg.exec();
}

void StaffManagementPage::loadOverviewCharts() {
    createBillsPerDayChart(m_overviewBillsChart);
    createSalesPerDayChart(m_overviewSalesChart);
    createPaymentMethodChart(m_overviewPaymentChart);
    createTopCustomersChart(m_overviewCustomersChart);
}

void StaffManagementPage::loadStaffCharts(int userId) {
    createBillsPerDayChart(m_staffBillsChart, userId);
    createSalesPerDayChart(m_staffSalesChart, userId);
    createPaymentMethodChart(m_staffPaymentChart, userId);
    createTopCustomersChart(m_staffCustomersChart, userId);
}

// Chart Creators

void StaffManagementPage::createBillsPerDayChart(QChartView* chartView, int userId) {
    auto* chart = new QChart();
    chart->setBackgroundBrush(QBrush(QColor("#1B1D20")));
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    chart->legend()->hide();
    
    auto* series = new QBarSeries();
    auto* barSet = new QBarSet("Bills");
    barSet->setColor(QColor("#4CAF50")); // Green bar
    
    QStringList categories;
    QVariantList params;
    QString filterSql = "";
    if (userId > 0) {
        filterSql = " AND user_id = ?";
        params << userId;
    }
    
    QString sql = QString("SELECT DATE(invoice_date) as d, COUNT(*) as cnt "
                          "FROM invoices "
                          "WHERE status = 'paid'%1 "
                          "GROUP BY d "
                          "ORDER BY d DESC LIMIT 7;").arg(filterSql);
                          
    auto& db = DatabaseManager::instance();
    std::vector<std::pair<QString, int>> data;
    if (db.isConnected()) {
        QSqlQuery query = db.prepare(sql);
        for (const auto& p : params) query.addBindValue(p);
        if (query.exec()) {
            while (query.next()) {
                data.push_back({query.value("d").toString(), query.value("cnt").toInt()});
            }
        }
    }
    
    std::reverse(data.begin(), data.end());
    
    int maxVal = 5;
    for (const auto& item : data) {
        categories << QDate::fromString(item.first, "yyyy-MM-dd").toString("dd MMM");
        *barSet << item.second;
        if (item.second > maxVal) maxVal = item.second;
    }
    
    // Fill with empty details if no bills
    if (categories.isEmpty()) {
        categories << QDate::currentDate().toString("dd MMM");
        *barSet << 0;
    }
    
    series->append(barSet);
    chart->addSeries(series);
    
    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(QColor("#A9B1BC"));
    axisX->setGridLineColor(QColor("#2A2D35"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    auto* axisY = new QValueAxis();
    axisY->setRange(0, maxVal + 1);
    axisY->setLabelFormat("%d");
    axisY->setLabelsColor(QColor("#A9B1BC"));
    axisY->setGridLineColor(QColor("#2A2D35"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    QChart* oldChart = chartView->chart();
    chartView->setChart(chart);
    if (oldChart) oldChart->deleteLater();
}

void StaffManagementPage::createSalesPerDayChart(QChartView* chartView, int userId) {
    auto* chart = new QChart();
    chart->setBackgroundBrush(QBrush(QColor("#1B1D20")));
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    chart->legend()->hide();
    
    auto* series = new QBarSeries();
    auto* barSet = new QBarSet("Revenue");
    barSet->setColor(QColor("#2196F3")); // Blue bar for revenue
    
    QStringList categories;
    QVariantList params;
    QString filterSql = "";
    if (userId > 0) {
        filterSql = " AND user_id = ?";
        params << userId;
    }
    
    QString sql = QString("SELECT DATE(invoice_date) as d, SUM(grand_total) as total "
                          "FROM invoices "
                          "WHERE status = 'paid'%1 "
                          "GROUP BY d "
                          "ORDER BY d DESC LIMIT 7;").arg(filterSql);
                          
    auto& db = DatabaseManager::instance();
    std::vector<std::pair<QString, double>> data;
    if (db.isConnected()) {
        QSqlQuery query = db.prepare(sql);
        for (const auto& p : params) query.addBindValue(p);
        if (query.exec()) {
            while (query.next()) {
                data.push_back({query.value("d").toString(), query.value("total").toDouble()});
            }
        }
    }
    
    std::reverse(data.begin(), data.end());
    
    double maxVal = 100.0;
    for (const auto& item : data) {
        categories << QDate::fromString(item.first, "yyyy-MM-dd").toString("dd MMM");
        *barSet << item.second;
        if (item.second > maxVal) maxVal = item.second;
    }
    
    if (categories.isEmpty()) {
        categories << QDate::currentDate().toString("dd MMM");
        *barSet << 0;
    }
    
    series->append(barSet);
    chart->addSeries(series);
    
    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(QColor("#A9B1BC"));
    axisX->setGridLineColor(QColor("#2A2D35"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    auto* axisY = new QValueAxis();
    axisY->setRange(0, maxVal * 1.15);
    axisY->setLabelFormat("₹%d");
    axisY->setLabelsColor(QColor("#A9B1BC"));
    axisY->setGridLineColor(QColor("#2A2D35"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    QChart* oldChart = chartView->chart();
    chartView->setChart(chart);
    if (oldChart) oldChart->deleteLater();
}

void StaffManagementPage::createPaymentMethodChart(QChartView* chartView, int userId) {
    auto* chart = new QChart();
    chart->setBackgroundBrush(QBrush(QColor("#1B1D20")));
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setLabelColor(QColor("#A9B1BC"));
    
    auto* series = new QPieSeries();
    
    QVariantList params;
    QString filterSql = "";
    if (userId > 0) {
        filterSql = " AND user_id = ?";
        params << userId;
    }
    
    QString sql = QString("SELECT payment_mode, SUM(grand_total) as total "
                          "FROM invoices "
                          "WHERE status = 'paid'%1 "
                          "GROUP BY payment_mode;").arg(filterSql);
                          
    auto& db = DatabaseManager::instance();
    QList<QColor> colors = {QColor("#4CAF50"), QColor("#2196F3"), QColor("#FF9800"), QColor("#E91E63")};
    int colorIdx = 0;
    
    if (db.isConnected()) {
        QSqlQuery query = db.prepare(sql);
        for (const auto& p : params) query.addBindValue(p);
        if (query.exec()) {
            while (query.next()) {
                QString mode = query.value("payment_mode").toString().toUpper();
                double total = query.value("total").toDouble();
                auto* slice = series->append(mode + QString(" (₹%1)").arg(total, 0, 'f', 0), total);
                if (slice) {
                    slice->setBrush(colors[colorIdx % colors.size()]);
                    slice->setLabelColor(QColor("#FFFFFF"));
                    colorIdx++;
                }
            }
        }
    }
    
    if (series->isEmpty()) {
        series->append("No Sales", 1);
    }
    
    chart->addSeries(series);
    
    QChart* oldChart = chartView->chart();
    chartView->setChart(chart);
    if (oldChart) oldChart->deleteLater();
}

void StaffManagementPage::createTopCustomersChart(QChartView* chartView, int userId) {
    auto* chart = new QChart();
    chart->setBackgroundBrush(QBrush(QColor("#1B1D20")));
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    chart->legend()->hide();
    
    auto* series = new QBarSeries();
    auto* barSet = new QBarSet("Revenue");
    barSet->setColor(QColor("#FF9800")); // Orange bar for top customers
    
    QStringList categories;
    QVariantList params;
    QString filterSql = "";
    if (userId > 0) {
        filterSql = " AND user_id = ?";
        params << userId;
    }
    
    QString sql = QString("SELECT customer_name, SUM(grand_total) as total "
                          "FROM invoices "
                          "WHERE status = 'paid' AND customer_name != '' AND customer_name != 'Walk-in'%1 "
                          "GROUP BY customer_name "
                          "ORDER BY total DESC LIMIT 5;").arg(filterSql);
                          
    auto& db = DatabaseManager::instance();
    double maxVal = 100.0;
    if (db.isConnected()) {
        QSqlQuery query = db.prepare(sql);
        for (const auto& p : params) query.addBindValue(p);
        if (query.exec()) {
            while (query.next()) {
                categories << query.value("customer_name").toString();
                double total = query.value("total").toDouble();
                *barSet << total;
                if (total > maxVal) maxVal = total;
            }
        }
    }
    
    if (categories.isEmpty()) {
        categories << "No Customers";
        *barSet << 0;
    }
    
    series->append(barSet);
    chart->addSeries(series);
    
    auto* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(QColor("#A9B1BC"));
    axisX->setGridLineColor(QColor("#2A2D35"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    auto* axisY = new QValueAxis();
    axisY->setRange(0, maxVal * 1.15);
    axisY->setLabelFormat("₹%d");
    axisY->setLabelsColor(QColor("#A9B1BC"));
    axisY->setGridLineColor(QColor("#2A2D35"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    QChart* oldChart = chartView->chart();
    chartView->setChart(chart);
    if (oldChart) oldChart->deleteLater();
}

} // namespace RetailMS

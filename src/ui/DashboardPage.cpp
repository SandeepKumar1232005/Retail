#include "DashboardPage.h"
#include "../database/DatabaseManager.h"
#include <QLabel>
#include <QGraphicsLayout>
#include <QDate>
#include <QSqlQuery>
#include <QSqlRecord>
#include "../services/SessionManager.h"

namespace RetailMS {

DashboardPage::DashboardPage(QWidget* parent) : QWidget(parent) {
    setupUi();
    refreshData();
}

void DashboardPage::setupUi() {
    if (SessionManager::instance().isLoggedIn() && !SessionManager::instance().currentUser().isAdmin()) {
        setupStaffUi();
    } else {
        setupAdminUi();
    }
}

void DashboardPage::setupAdminUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background-color: transparent; }");
    
    QWidget* scrollContent = new QWidget(scrollArea);
    scrollContent->setStyleSheet("background-color: transparent;");
    QVBoxLayout* contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(40, 40, 40, 40);
    contentLayout->setSpacing(24);
    
    // Top Section: Stat Cards
    QHBoxLayout* statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(20);
    
    m_todayRevenueCard = new StatCardWidget("Today's Revenue", "₹0.00", "", this);
    m_totalOrdersCard = new StatCardWidget("Today's Orders", "0", "", this);
    m_customersCard = new StatCardWidget("Total Customers", "0", "", this);
    m_inventoryValueCard = new StatCardWidget("Inventory Value", "₹0.00", "", this);
    
    statsLayout->addWidget(m_todayRevenueCard);
    statsLayout->addWidget(m_totalOrdersCard);
    statsLayout->addWidget(m_customersCard);
    statsLayout->addWidget(m_inventoryValueCard);
    
    contentLayout->addLayout(statsLayout);
    
    // Middle Section: Charts and AI Panel
    QHBoxLayout* middleLayout = new QHBoxLayout();
    middleLayout->setSpacing(20);
    
    middleLayout->addWidget(createChartsSection(), 2);
    middleLayout->addWidget(createAIPanel(), 1);
    
    contentLayout->addLayout(middleLayout);
    
    // Bottom Section: Recent Transactions & Low Stock
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(20);
    
    bottomLayout->addWidget(createRecentTransactionsPanel(), 2);
    
    QWidget* lowStockPanel = new QWidget(this);
    lowStockPanel->setObjectName("statCard");
    QVBoxLayout* lsLayout = new QVBoxLayout(lowStockPanel);
    lsLayout->setContentsMargins(20, 20, 20, 20);
    QLabel* lsTitle = new QLabel("Low Stock Alerts", lowStockPanel);
    lsTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFFFFF;");
    lsLayout->addWidget(lsTitle);
    
    // Low stock list placeholder label
    QLabel* lsListLabel = new QLabel("No low stock items.", lowStockPanel);
    lsListLabel->setObjectName("lowStockListLabel");
    lsListLabel->setStyleSheet("color: #A9B1BC; font-size: 14px;");
    lsLayout->addWidget(lsListLabel);
    
    lsLayout->addStretch();
    bottomLayout->addWidget(lowStockPanel, 1);
    
    contentLayout->addLayout(bottomLayout);
    
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);
}

void DashboardPage::setupStaffUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(24);
    
    QString name = SessionManager::instance().isLoggedIn() ? SessionManager::instance().currentUser().fullName : "User";
    QLabel* welcome = new QLabel(QString("Welcome, %1").arg(name), this);
    welcome->setStyleSheet("font-size: 24px; font-weight: bold; color: #FFFFFF;");
    mainLayout->addWidget(welcome);

    QHBoxLayout* statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(20);
    
    m_staffPersonalSalesCard = new StatCardWidget("My Sales Today", "₹0.00", "", this);
    m_staffBillsGeneratedCard = new StatCardWidget("My Bills Today", "0", "", this);
    m_staffShiftStatusCard = new StatCardWidget("Shift Status", "Active", "", this);
    
    statsLayout->addWidget(m_staffPersonalSalesCard);
    statsLayout->addWidget(m_staffBillsGeneratedCard);
    statsLayout->addWidget(m_staffShiftStatusCard);
    statsLayout->addStretch();
    
    mainLayout->addLayout(statsLayout);
    mainLayout->addStretch();
}

QWidget* DashboardPage::createChartsSection() {
    QWidget* chartsWidget = new QWidget(this);
    chartsWidget->setObjectName("statCard");
    QVBoxLayout* layout = new QVBoxLayout(chartsWidget);
    layout->setContentsMargins(20, 20, 20, 20);
    
    QLabel* title = new QLabel("Revenue Trend (Last 7 Days)", chartsWidget);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFFFFF;");
    layout->addWidget(title);
    
    m_chartSeries = new QLineSeries();
    
    m_chart = new QChart();
    m_chart->addSeries(m_chartSeries);
    m_chart->legend()->hide();
    m_chart->setBackgroundBrush(QBrush(QColor("#1B1D20")));
    m_chart->layout()->setContentsMargins(0, 0, 0, 0);
    m_chart->setBackgroundRoundness(0);
    
    QPen pen(QColor("#4CAF50"));
    pen.setWidth(3);
    m_chartSeries->setPen(pen);
    
    m_axisX = new QValueAxis();
    m_axisX->setLabelsColor(QColor("#A9B1BC"));
    m_axisX->setGridLineColor(QColor("#2A2D35"));
    m_axisX->setLabelFormat("%d");
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chartSeries->attachAxis(m_axisX);
    
    m_axisY = new QValueAxis();
    m_axisY->setLabelsColor(QColor("#A9B1BC"));
    m_axisY->setGridLineColor(QColor("#2A2D35"));
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_chartSeries->attachAxis(m_axisY);
    
    QChartView* chartView = new QChartView(m_chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background: transparent;");
    chartView->setMinimumHeight(250);
    
    layout->addWidget(chartView);
    return chartsWidget;
}

QWidget* DashboardPage::createAIPanel() {
    QWidget* aiWidget = new QWidget(this);
    aiWidget->setObjectName("statCard");
    QVBoxLayout* layout = new QVBoxLayout(aiWidget);
    layout->setContentsMargins(20, 20, 20, 20);
    
    QLabel* title = new QLabel("RetailGPT", aiWidget);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFFFFF;");
    layout->addWidget(title);
    
    m_chatArea = new QTextEdit(aiWidget);
    m_chatArea->setReadOnly(true);
    m_chatArea->setStyleSheet("background-color: transparent; border: none; color: #A9B1BC;");
    m_chatArea->append("<b>RetailGPT:</b> Hello! Ask me any business questions, e.g., 'How much profit did we make today?' or 'Show low stock items'.");
    layout->addWidget(m_chatArea);
    
    QHBoxLayout* inputLayout = new QHBoxLayout();
    m_chatInput = new QLineEdit(aiWidget);
    m_chatInput->setPlaceholderText("Ask AI to query database...");
    inputLayout->addWidget(m_chatInput);
    
    m_chatSendBtn = new QPushButton("Send", aiWidget);
    m_chatSendBtn->setObjectName("primaryButton");
    inputLayout->addWidget(m_chatSendBtn);
    
    layout->addLayout(inputLayout);
    
    connect(m_chatSendBtn, &QPushButton::clicked, this, &DashboardPage::handleAIQuery);
    connect(m_chatInput, &QLineEdit::returnPressed, this, &DashboardPage::handleAIQuery);
    
    return aiWidget;
}

QWidget* DashboardPage::createRecentTransactionsPanel() {
    QWidget* panel = new QWidget(this);
    panel->setObjectName("statCard");
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(20, 20, 20, 20);
    
    QLabel* title = new QLabel("Recent Transactions (Last 5)", panel);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFFFFF;");
    layout->addWidget(title);
    
    QTextEdit* txsArea = new QTextEdit(panel);
    txsArea->setObjectName("recentTxsArea");
    txsArea->setReadOnly(true);
    txsArea->setStyleSheet("background-color: transparent; border: none; color: #A9B1BC; font-size: 14px;");
    layout->addWidget(txsArea);
    
    return panel;
}

void DashboardPage::refreshData() {
    if (SessionManager::instance().isLoggedIn() && !SessionManager::instance().currentUser().isAdmin()) {
        refreshStaffData();
    } else {
        refreshAdminData();
    }
}

void DashboardPage::refreshStaffData() {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;
    
    if (!SessionManager::instance().isLoggedIn()) return;
    int userId = SessionManager::instance().currentUser().id;

    auto revRes = db.executeScalar(
        "SELECT SUM(grand_total) FROM invoices WHERE DATE(invoice_date) = DATE('now') AND cashier_id = ?",
        {userId}
    );
    double revenue = (revRes && !revRes->isNull()) ? revRes->toDouble() : 0.0;
    if (m_staffPersonalSalesCard) m_staffPersonalSalesCard->setValue(QString("₹%1").arg(revenue, 0, 'f', 2));

    auto ordRes = db.executeScalar(
        "SELECT COUNT(*) FROM invoices WHERE DATE(invoice_date) = DATE('now') AND cashier_id = ?",
        {userId}
    );
    int orders = (ordRes && !ordRes->isNull()) ? ordRes->toInt() : 0;
    if (m_staffBillsGeneratedCard) m_staffBillsGeneratedCard->setValue(QString::number(orders));
}

void DashboardPage::refreshAdminData() {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    // 1. Today's Revenue
    auto revRes = db.executeScalar("SELECT SUM(grand_total) FROM invoices WHERE DATE(invoice_date) = DATE('now');");
    double revenue = (revRes && !revRes->isNull()) ? revRes->toDouble() : 0.0;
    m_todayRevenueCard->setValue(QString("₹%1").arg(revenue, 0, 'f', 2));

    // 2. Today's Orders
    auto ordRes = db.executeScalar("SELECT COUNT(*) FROM invoices WHERE DATE(invoice_date) = DATE('now');");
    int orders = (ordRes && !ordRes->isNull()) ? ordRes->toInt() : 0;
    m_totalOrdersCard->setValue(QString::number(orders));

    // 3. Total Customers
    auto custRes = db.executeScalar("SELECT COUNT(*) FROM customers;");
    int customers = (custRes && !custRes->isNull()) ? custRes->toInt() : 0;
    m_customersCard->setValue(QString::number(customers));

    // 4. Inventory Value
    auto invRes = db.executeScalar("SELECT SUM(stock_quantity * cost_price) FROM products;");
    double invValue = (invRes && !invRes->isNull()) ? invRes->toDouble() : 0.0;
    m_inventoryValueCard->setValue(QString("₹%1").arg(invValue, 0, 'f', 2));

    // 5. Low Stock Alerts Panel & Low Stock Card calculation
    auto lowStockRes = db.executeScalar("SELECT COUNT(*) FROM products WHERE stock_quantity <= min_stock;");
    int lowStockCount = (lowStockRes && !lowStockRes->isNull()) ? lowStockRes->toInt() : 0;
    
    QLabel* lsListLabel = findChild<QLabel*>("lowStockListLabel");
    if (lsListLabel) {
        if (lowStockCount == 0) {
            lsListLabel->setText("No low stock items. All inventory healthy!");
            lsListLabel->setStyleSheet("color: #22C55E; font-size: 14px;");
        } else {
            // Get up to 3 low stock product names
            QSqlQuery query = db.execute("SELECT name, stock_quantity FROM products WHERE stock_quantity <= min_stock LIMIT 3;");
            QStringList lowStockItems;
            while (query.next()) {
                lowStockItems << QString("- %1 (Qty: %2)").arg(query.value(0).toString()).arg(query.value(1).toDouble());
            }
            lsListLabel->setText(QString("Low Stock Items (%1 total):\n%2").arg(lowStockCount).arg(lowStockItems.join("\n")));
            lsListLabel->setStyleSheet("color: #EF4444; font-size: 14px;");
        }
    }

    // 6. Recent Transactions Panel
    QTextEdit* txsArea = findChild<QTextEdit*>("recentTxsArea");
    if (txsArea) {
        txsArea->clear();
        QSqlQuery query = db.execute("SELECT invoice_number, grand_total, payment_mode, invoice_date FROM invoices ORDER BY invoice_date DESC LIMIT 5;");
        bool hasData = false;
        while (query.next()) {
            hasData = true;
            QString invNum = query.value(0).toString();
            double total = query.value(1).toDouble();
            QString mode = query.value(2).toString().toUpper();
            QString date = query.value(3).toString();
            txsArea->append(QString("<b>%1</b> | ₹%2 | %3 | %4").arg(invNum).arg(total, 0, 'f', 2).arg(mode).arg(date));
        }
        if (!hasData) {
            txsArea->setText("No sales transactions recorded yet.");
        }
    }

    // 7. Update Charts (Revenue Trend)
    m_chartSeries->clear();
    QSqlQuery chartQuery = db.execute("SELECT DATE(invoice_date) as d, SUM(grand_total) FROM invoices GROUP BY d ORDER BY d DESC LIMIT 7;");
    
    QList<QPointF> points;
    double maxVal = 1000.0; // minimum default Y range
    int idx = 0;
    
    // We get them in desc order, store them and reverse them for correct timeline sequence
    struct ChartData {
        QString date;
        double value;
    };
    QList<ChartData> rawData;
    while (chartQuery.next()) {
        rawData.prepend({chartQuery.value(0).toString(), chartQuery.value(1).toDouble()});
    }

    for (const auto& item : rawData) {
        points.append(QPointF(idx, item.value));
        if (item.value > maxVal) {
            maxVal = item.value;
        }
        idx++;
    }
    
    if (points.isEmpty()) {
        // Flat line when empty
        m_chartSeries->append(0, 0);
        m_chartSeries->append(6, 0);
        m_axisX->setRange(0, 6);
        m_axisY->setRange(0, 100);
    } else {
        for (const auto& p : points) {
            m_chartSeries->append(p);
        }
        m_axisX->setRange(0, qMax(1, idx - 1));
        m_axisY->setRange(0, maxVal * 1.15); // Add 15% padding
    }
}

void DashboardPage::handleAIQuery() {
    QString question = m_chatInput->text().trimmed();
    if (question.isEmpty()) return;

    m_chatArea->append(QString("<br/><b>You:</b> %1").arg(question));
    m_chatInput->clear();

    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) {
        m_chatArea->append("<b>RetailGPT:</b> Database is not connected.");
        return;
    }

    QString response;
    QString lowerQuestion = question.toLower();

    if (lowerQuestion.contains("profit") && (lowerQuestion.contains("today") || lowerQuestion.contains("now"))) {
        auto res = db.executeScalar("SELECT SUM((ii.selling_price - ii.cost_price) * ii.quantity - ii.discount_amt) FROM invoice_items ii JOIN invoices i ON ii.invoice_id = i.id WHERE DATE(i.invoice_date) = DATE('now') AND i.status = 'paid';");
        double profit = (res && !res->isNull()) ? res->toDouble() : 0.0;
        if (profit == 0.0) {
            response = "No sales have been recorded today. Profit is ₹0.00.";
        } else {
            response = QString("Today's actual profit is <b>₹%1</b> (calculated as Selling Price - Cost Price - Discounts).").arg(profit, 0, 'f', 2);
        }
    } 
    else if (lowerQuestion.contains("revenue") && (lowerQuestion.contains("today") || lowerQuestion.contains("now"))) {
        auto res = db.executeScalar("SELECT SUM(grand_total) FROM invoices WHERE DATE(invoice_date) = DATE('now');");
        double revenue = (res && !res->isNull()) ? res->toDouble() : 0.0;
        if (revenue == 0.0) {
            response = "No sales have been recorded today. Revenue is ₹0.00.";
        } else {
            response = QString("Today's total revenue is <b>₹%1</b> across all generated invoices.").arg(revenue, 0, 'f', 2);
        }
    }
    else if (lowerQuestion.contains("low stock") || lowerQuestion.contains("minimum stock")) {
        QSqlQuery query = db.execute("SELECT name, stock_quantity, min_stock FROM products WHERE stock_quantity <= min_stock;");
        QStringList items;
        while (query.next()) {
            items << QString("- <b>%1</b> (Current Stock: %2, Threshold: %3)").arg(query.value(0).toString()).arg(query.value(1).toDouble()).arg(query.value(2).toDouble());
        }
        if (items.isEmpty()) {
            response = "No low stock alerts. All inventory levels are above thresholds!";
        } else {
            response = QString("Found low stock items:<br/>%1").arg(items.join("<br/>"));
        }
    }
    else if (lowerQuestion.contains("customer")) {
        auto res = db.executeScalar("SELECT COUNT(*) FROM customers;");
        int count = (res && !res->isNull()) ? res->toInt() : 0;
        response = QString("We currently have <b>%1</b> customers registered in the database.").arg(count);
    }
    else {
        response = "I can only answer queries about live database metrics. Try asking:<br/>"
                   "- <i>'How much profit did we make today?'</i><br/>"
                   "- <i>'What is today's revenue?'</i><br/>"
                   "- <i>'Show low stock items'</i><br/>"
                   "- <i>'How many customers do we have?'</i>";
    }

    m_chatArea->append(QString("<b>RetailGPT:</b> %1").arg(response));
}

} // namespace RetailMS

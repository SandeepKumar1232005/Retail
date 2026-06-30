#include "SalesHistoryPage.h"
#include "../controllers/BillingController.h"
#include "../database/DatabaseManager.h"
#include "../models/Invoice.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDate>
#include <QVariantList>

namespace RetailMS {

SalesHistoryPage::SalesHistoryPage(std::shared_ptr<BillingController> controller, QWidget* parent)
    : QWidget(parent), m_controller(std::move(controller)) {
    setupUi();
    setupConnections();
    
    // Auto-refresh for last 30 days initially
    m_fromDateEdit->setDate(QDate::currentDate().addDays(-30));
    m_toDateEdit->setDate(QDate::currentDate());
    refreshData();
}

void SalesHistoryPage::setupUi() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // --- LEFT SIDE: LIST & FILTERS ---
    QWidget* leftWidget = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(15);

    QLabel* titleLabel = new QLabel("Sales History (Admin)", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #FFFFFF;");
    leftLayout->addWidget(titleLabel);

    // Filters row
    QHBoxLayout* filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(10);

    QLabel* searchLabel = new QLabel("Search:", this);
    m_searchField = new QLineEdit(this);
    m_searchField->setPlaceholderText("Invoice #, Customer, Cashier...");
    m_searchField->setFixedWidth(200);

    QLabel* fromLabel = new QLabel("From:", this);
    m_fromDateEdit = new QDateEdit(this);
    m_fromDateEdit->setCalendarPopup(true);
    m_fromDateEdit->setFixedWidth(110);

    QLabel* toLabel = new QLabel("To:", this);
    m_toDateEdit = new QDateEdit(this);
    m_toDateEdit->setCalendarPopup(true);
    m_toDateEdit->setFixedWidth(110);

    m_refreshButton = new QPushButton("Filter", this);
    m_refreshButton->setObjectName("primaryButton");
    m_refreshButton->setFixedWidth(80);

    filterLayout->addWidget(searchLabel);
    filterLayout->addWidget(m_searchField);
    filterLayout->addWidget(fromLabel);
    filterLayout->addWidget(m_fromDateEdit);
    filterLayout->addWidget(toLabel);
    filterLayout->addWidget(m_toDateEdit);
    filterLayout->addWidget(m_refreshButton);
    filterLayout->addStretch();

    leftLayout->addLayout(filterLayout);

    // Invoices Table
    m_invoicesTable = new QTableWidget(this);
    m_invoicesTable->setColumnCount(8);
    m_invoicesTable->setHorizontalHeaderLabels({"Invoice #", "Date & Time", "Cashier", "Customer", "Phone", "Payment", "Total", "Status"});
    m_invoicesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_invoicesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_invoicesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_invoicesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_invoicesTable->verticalHeader()->setVisible(false);
    m_invoicesTable->setStyleSheet(
        "QTableWidget { background-color: #1E2025; border: 1px solid #2A2D35; border-radius: 8px; }"
    );
    leftLayout->addWidget(m_invoicesTable, 1);

    mainLayout->addWidget(leftWidget, 7); // 70% width

    // --- RIGHT SIDE: DETAILED VIEW PANEL ---
    QWidget* rightWidget = new QWidget(this);
    rightWidget->setObjectName("statCard");
    rightWidget->setStyleSheet("QWidget#statCard { background-color: #16181B; border: 1px solid #2A2D35; border-radius: 12px; }");
    
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(20, 20, 20, 20);
    rightLayout->setSpacing(15);

    QLabel* detailTitle = new QLabel("Invoice Details", rightWidget);
    detailTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF;");
    rightLayout->addWidget(detailTitle);

    // Metadata layout
    QGridLayout* metaLayout = new QGridLayout();
    metaLayout->setSpacing(8);

    auto addMetaRow = [&](const QString& labelText, QLabel*& valueLabel, int row) {
        QLabel* lbl = new QLabel(labelText, rightWidget);
        lbl->setStyleSheet("color: #A9B1BC; font-weight: 500; font-size: 13px;");
        valueLabel = new QLabel("-", rightWidget);
        valueLabel->setStyleSheet("color: #FFFFFF; font-weight: bold; font-size: 13px;");
        metaLayout->addWidget(lbl, row, 0);
        metaLayout->addWidget(valueLabel, row, 1);
    };

    addMetaRow("Invoice #:", m_detailInvoiceNum, 0);
    addMetaRow("Date/Time:", m_detailDateTime, 1);
    addMetaRow("Cashier:", m_detailCashier, 2);
    addMetaRow("Customer:", m_detailCustomer, 3);
    addMetaRow("Phone:", m_detailCustomerPhone, 4);
    addMetaRow("Payment Mode:", m_detailPaymentMode, 5);

    rightLayout->addLayout(metaLayout);

    // Items table inside detail
    QLabel* itemsTitle = new QLabel("Items List", rightWidget);
    itemsTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #4CAF50; margin-top: 10px;");
    rightLayout->addWidget(itemsTitle);

    m_itemsTable = new QTableWidget(rightWidget);
    m_itemsTable->setColumnCount(4);
    m_itemsTable->setHorizontalHeaderLabels({"Product", "Qty", "Price", "Total"});
    m_itemsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_itemsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_itemsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_itemsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_itemsTable->verticalHeader()->setVisible(false);
    m_itemsTable->setStyleSheet(
        "QTableWidget { background-color: #1E2025; border: 1px solid #2A2D35; border-radius: 8px; }"
    );
    m_itemsTable->setFixedHeight(200);
    rightLayout->addWidget(m_itemsTable);

    // Totals Section
    QFrame* divider = new QFrame(rightWidget);
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("color: #2A2D35;");
    rightLayout->addWidget(divider);

    QGridLayout* totalsLayout = new QGridLayout();
    totalsLayout->setSpacing(8);

    auto addTotalRow = [&](const QString& labelText, QLabel*& valueLabel, int row, bool highlight = false) {
        QLabel* lbl = new QLabel(labelText, rightWidget);
        lbl->setStyleSheet(highlight ? "color: #FFFFFF; font-weight: bold; font-size: 15px;" : "color: #A9B1BC; font-size: 13px;");
        valueLabel = new QLabel("₹0.00", rightWidget);
        valueLabel->setStyleSheet(highlight ? "color: #22C55E; font-weight: 800; font-size: 16px;" : "color: #FFFFFF; font-size: 13px;");
        totalsLayout->addWidget(lbl, row, 0, Qt::AlignLeft);
        totalsLayout->addWidget(valueLabel, row, 1, Qt::AlignRight);
    };

    addTotalRow("Subtotal:", m_detailSubtotal, 0);
    addTotalRow("Discount:", m_detailDiscount, 1);
    addTotalRow("Tax (GST):", m_detailTax, 2);
    addTotalRow("Grand Total:", m_detailGrandTotal, 3, true);

    rightLayout->addLayout(totalsLayout);
    rightLayout->addStretch();

    mainLayout->addWidget(rightWidget, 3); // 30% width
}

void SalesHistoryPage::setupConnections() {
    connect(m_refreshButton, &QPushButton::clicked, this, &SalesHistoryPage::refreshData);
    connect(m_searchField, &QLineEdit::textChanged, this, &SalesHistoryPage::refreshData);
    connect(m_invoicesTable, &QTableWidget::itemSelectionChanged, this, &SalesHistoryPage::onInvoiceSelected);
    
    // Auto-refresh when a new invoice is created/finalized
    connect(m_controller.get(), &BillingController::checkoutComplete, this, &SalesHistoryPage::refreshData);
}

void SalesHistoryPage::refreshData() {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    QString sql = "SELECT i.id, i.invoice_number, i.invoice_date, u.full_name AS cashier_name, "
                  "i.customer_id, i.customer_name, i.customer_phone, i.payment_mode, i.grand_total, i.status "
                  "FROM invoices i "
                  "LEFT JOIN users u ON i.user_id = u.id "
                  "WHERE DATE(i.invoice_date) >= ? AND DATE(i.invoice_date) <= ?";
    
    QVariantList params;
    params << m_fromDateEdit->date().toString(Qt::ISODate)
           << m_toDateEdit->date().toString(Qt::ISODate);

    QString search = m_searchField->text().trimmed();
    if (!search.isEmpty()) {
        sql += " AND (i.invoice_number LIKE ? OR u.full_name LIKE ? OR i.customer_name LIKE ? OR i.customer_phone LIKE ?)";
        QString wildcard = "%" + search + "%";
        params << wildcard << wildcard << wildcard << wildcard;
    }

    sql += " ORDER BY i.invoice_date DESC;";

    m_invoicesTable->setRowCount(0);

    QSqlQuery query = db.prepare(sql);
    for (const auto& p : params) {
        query.addBindValue(p);
    }

    if (query.exec()) {
        while (query.next()) {
            int row = m_invoicesTable->rowCount();
            m_invoicesTable->insertRow(row);

            // Store internal ID on first column's user role
            QTableWidgetItem* invNumItem = new QTableWidgetItem(query.value("invoice_number").toString());
            invNumItem->setData(Qt::UserRole, query.value("id").toInt());

            m_invoicesTable->setItem(row, 0, invNumItem);
            m_invoicesTable->setItem(row, 1, new QTableWidgetItem(query.value("invoice_date").toDateTime().toString("yyyy-MM-dd HH:mm")));
            m_invoicesTable->setItem(row, 2, new QTableWidgetItem(query.value("cashier_name").toString()));
            
            QString custName = query.value("customer_name").toString();
            int custId = query.value("customer_id").toInt();
            QString custDisplay = custName.isEmpty() ? "-" : custName;
            if (custId > 0) {
                custDisplay += QString(" (CUS-%1)").arg(custId, 6, 10, QChar('0'));
            }
            m_invoicesTable->setItem(row, 3, new QTableWidgetItem(custDisplay));
            m_invoicesTable->setItem(row, 4, new QTableWidgetItem(query.value("customer_phone").toString().isEmpty() ? "-" : query.value("customer_phone").toString()));
            m_invoicesTable->setItem(row, 5, new QTableWidgetItem(query.value("payment_mode").toString().toUpper()));
            m_invoicesTable->setItem(row, 6, new QTableWidgetItem(QString("₹%1").arg(query.value("grand_total").toDouble(), 0, 'f', 2)));
            m_invoicesTable->setItem(row, 7, new QTableWidgetItem(query.value("status").toString().toUpper()));
        }
    }

    // Clear detail panel initially
    m_detailInvoiceNum->setText("-");
    m_detailDateTime->setText("-");
    m_detailCashier->setText("-");
    m_detailCustomer->setText("-");
    m_detailCustomerPhone->setText("-");
    m_detailPaymentMode->setText("-");
    m_itemsTable->setRowCount(0);
    m_detailSubtotal->setText("₹0.00");
    m_detailDiscount->setText("₹0.00");
    m_detailTax->setText("₹0.00");
    m_detailGrandTotal->setText("₹0.00");
}

void SalesHistoryPage::onInvoiceSelected() {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    int selectedRow = m_invoicesTable->currentRow();
    if (selectedRow < 0) return;

    QTableWidgetItem* firstItem = m_invoicesTable->item(selectedRow, 0);
    if (!firstItem) return;

    int invoiceId = firstItem->data(Qt::UserRole).toInt();

    // Query full invoice details
    QString sql = "SELECT i.invoice_number, i.invoice_date, u.full_name AS cashier_name, "
                  "c.name AS customer_name, c.phone AS customer_phone, i.payment_mode, "
                  "i.subtotal, i.discount_amt, i.coupon_discount, (i.cgst_amt + i.sgst_amt) AS tax, i.grand_total "
                  "FROM invoices i "
                  "LEFT JOIN users u ON i.user_id = u.id "
                  "LEFT JOIN customers c ON i.customer_id = c.id "
                  "WHERE i.id = ?;";
    
    QSqlQuery query = db.prepare(sql);
    query.addBindValue(invoiceId);

    if (query.exec() && query.next()) {
        m_detailInvoiceNum->setText(query.value("invoice_number").toString());
        m_detailDateTime->setText(query.value("invoice_date").toDateTime().toString("yyyy-MM-dd HH:mm:ss"));
        m_detailCashier->setText(query.value("cashier_name").toString());
        QString custName = query.value("customer_name").toString();
        m_detailCustomer->setText(custName.isEmpty() ? "Walk-in" : custName);
        m_detailCustomerPhone->setText(query.value("customer_phone").toString());
        m_detailPaymentMode->setText(query.value("payment_mode").toString().toUpper());

        m_detailSubtotal->setText(QString("₹%1").arg(query.value("subtotal").toDouble(), 0, 'f', 2));
        double discount = query.value("discount_amt").toDouble() + query.value("coupon_discount").toDouble();
        m_detailDiscount->setText(QString("₹%1").arg(discount, 0, 'f', 2));
        m_detailTax->setText(QString("₹%1").arg(query.value("tax").toDouble(), 0, 'f', 2));
        m_detailGrandTotal->setText(QString("₹%1").arg(query.value("grand_total").toDouble(), 0, 'f', 2));
    }

    // Query items
    m_itemsTable->setRowCount(0);
    QString itemSql = "SELECT product_name, quantity, selling_price, total FROM invoice_items WHERE invoice_id = ?;";
    QSqlQuery itemQuery = db.prepare(itemSql);
    itemQuery.addBindValue(invoiceId);

    if (itemQuery.exec()) {
        while (itemQuery.next()) {
            int row = m_itemsTable->rowCount();
            m_itemsTable->insertRow(row);
            m_itemsTable->setItem(row, 0, new QTableWidgetItem(itemQuery.value("product_name").toString()));
            m_itemsTable->setItem(row, 1, new QTableWidgetItem(QString::number(itemQuery.value("quantity").toDouble())));
            m_itemsTable->setItem(row, 2, new QTableWidgetItem(QString("₹%1").arg(itemQuery.value("selling_price").toDouble(), 0, 'f', 2)));
            m_itemsTable->setItem(row, 3, new QTableWidgetItem(QString("₹%1").arg(itemQuery.value("total").toDouble(), 0, 'f', 2)));
        }
    }
}

} // namespace RetailMS

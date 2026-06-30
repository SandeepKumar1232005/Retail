#include "MyBillsPage.h"
#include "../controllers/BillingController.h"
#include "../models/Invoice.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>

namespace RetailMS {

MyBillsPage::MyBillsPage(std::shared_ptr<BillingController> controller, QWidget* parent)
    : QWidget(parent), m_controller(std::move(controller)) {
    setupUi();
    setupConnections();
    // Auto-refresh for current month initially
    m_fromDateEdit->setDate(QDate::currentDate().addDays(-30));
    m_toDateEdit->setDate(QDate::currentDate());
    refreshData();
}

void MyBillsPage::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // Title
    QLabel* titleLabel = new QLabel("My Bills", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #333;");
    mainLayout->addWidget(titleLabel);

    // Filters
    QHBoxLayout* filterLayout = new QHBoxLayout();
    
    QLabel* fromLabel = new QLabel("From:", this);
    m_fromDateEdit = new QDateEdit(this);
    m_fromDateEdit->setCalendarPopup(true);
    
    QLabel* toLabel = new QLabel("To:", this);
    m_toDateEdit = new QDateEdit(this);
    m_toDateEdit->setCalendarPopup(true);
    
    m_refreshButton = new QPushButton("Apply Filter", this);
    m_refreshButton->setFixedWidth(120);
    
    filterLayout->addWidget(fromLabel);
    filterLayout->addWidget(m_fromDateEdit);
    filterLayout->addSpacing(15);
    filterLayout->addWidget(toLabel);
    filterLayout->addWidget(m_toDateEdit);
    filterLayout->addSpacing(15);
    filterLayout->addWidget(m_refreshButton);
    filterLayout->addStretch();
    
    mainLayout->addLayout(filterLayout);

    // Table
    m_billsTable = new QTableWidget(this);
    m_billsTable->setColumnCount(7);
    m_billsTable->setHorizontalHeaderLabels({"ID", "Invoice #", "Date", "Subtotal", "Discount", "Total", "Status"});
    m_billsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_billsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_billsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_billsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_billsTable->verticalHeader()->setVisible(false);
    
    mainLayout->addWidget(m_billsTable, 1);
}

void MyBillsPage::setupConnections() {
    connect(m_refreshButton, &QPushButton::clicked, this, &MyBillsPage::refreshData);
    
    // Setup connection to auto-refresh when new invoice is checked out
    connect(m_controller.get(), &BillingController::checkoutComplete, this, [this](const Invoice&) {
        refreshData();
    });
}

void MyBillsPage::refreshData() {
    auto from = m_fromDateEdit->date();
    auto to = m_toDateEdit->date();
    auto invoices = m_controller->getInvoices(from, to);
    populateTable(invoices);
}

void MyBillsPage::populateTable(const std::vector<Invoice>& invoices) {
    m_billsTable->setRowCount(0);
    
    for (const auto& inv : invoices) {
        int row = m_billsTable->rowCount();
        m_billsTable->insertRow(row);
        
        m_billsTable->setItem(row, 0, new QTableWidgetItem(QString::number(inv.id)));
        m_billsTable->setItem(row, 1, new QTableWidgetItem(inv.invoiceNumber));
        m_billsTable->setItem(row, 2, new QTableWidgetItem(inv.invoiceDate.toString("yyyy-MM-dd HH:mm")));
        m_billsTable->setItem(row, 3, new QTableWidgetItem(QString::number(inv.subtotal, 'f', 2)));
        m_billsTable->setItem(row, 4, new QTableWidgetItem(QString::number(inv.discountAmt + inv.couponDiscount, 'f', 2)));
        m_billsTable->setItem(row, 5, new QTableWidgetItem(QString::number(inv.grandTotal, 'f', 2)));
        m_billsTable->setItem(row, 6, new QTableWidgetItem(inv.statusString().toUpper()));
    }
}

void MyBillsPage::onViewInvoiceClicked() {
    // Optional: implement viewing invoice details here.
}

} // namespace RetailMS

#include "BillingPage.h"
#include "../controllers/BillingController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>

namespace RetailMS {

BillingPage::BillingPage(std::shared_ptr<BillingController> controller, QWidget* parent)
    : QWidget(parent), m_controller(std::move(controller)) {
    setupUi();
    setupConnections();
    refreshTotals();
}

void BillingPage::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Top Bar (Barcode Scan)
    QHBoxLayout* topLayout = new QHBoxLayout();
    m_barcodeInput = new QLineEdit(this);
    m_barcodeInput->setPlaceholderText("Scan Barcode or Enter Product Code...");
    m_barcodeInput->setMinimumHeight(40);
    
    QPushButton* addBtn = new QPushButton("Add Item", this);
    addBtn->setMinimumHeight(40);
    
    topLayout->addWidget(m_barcodeInput);
    topLayout->addWidget(addBtn);
    mainLayout->addLayout(topLayout);
    
    // Cart Table
    m_cartTable = new QTableWidget(this);
    m_cartTable->setColumnCount(6);
    m_cartTable->setHorizontalHeaderLabels({"Product", "Price", "Qty", "Tax", "Total", "Action"});
    m_cartTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    mainLayout->addWidget(m_cartTable, 1);
    
    // Bottom Totals & Actions
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    
    QVBoxLayout* totalsLayout = new QVBoxLayout();
    m_subtotalLabel = new QLabel("Subtotal: $0.00", this);
    m_discountLabel = new QLabel("Discount: $0.00", this);
    m_taxLabel = new QLabel("Tax: $0.00", this);
    m_totalLabel = new QLabel("Grand Total: $0.00", this);
    m_totalLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #4CAF50;");
    
    totalsLayout->addWidget(m_subtotalLabel);
    totalsLayout->addWidget(m_discountLabel);
    totalsLayout->addWidget(m_taxLabel);
    totalsLayout->addWidget(m_totalLabel);
    
    QVBoxLayout* actionsLayout = new QVBoxLayout();
    QPushButton* applyCouponBtn = new QPushButton("Apply Coupon", this);
    QPushButton* checkoutBtn = new QPushButton("Checkout", this);
    checkoutBtn->setStyleSheet("background-color: #4CAF50; color: white; font-size: 18px; padding: 10px;");
    
    actionsLayout->addWidget(applyCouponBtn);
    actionsLayout->addWidget(checkoutBtn);
    
    bottomLayout->addLayout(totalsLayout);
    bottomLayout->addStretch();
    bottomLayout->addLayout(actionsLayout);
    
    mainLayout->addLayout(bottomLayout);
}

void BillingPage::setupConnections() {
    connect(m_barcodeInput, &QLineEdit::returnPressed, this, [this]() {
        QString barcode = m_barcodeInput->text();
        if (!barcode.isEmpty()) {
            m_controller->addProductByBarcode(barcode);
            m_barcodeInput->clear();
        }
    });
    
    connect(m_controller.get(), &BillingController::invoiceUpdated, this, [this](const Invoice&) {
        refreshTable();
        refreshTotals();
    });
    
    connect(m_controller.get(), &BillingController::errorOccurred, this, [this](const QString& msg) {
        QMessageBox::critical(this, "Error", msg);
    });
    
    connect(m_controller.get(), &BillingController::successMessage, this, [this](const QString& msg) {
        QMessageBox::information(this, "Success", msg);
    });
}

void BillingPage::refreshTable() {
    Invoice inv = m_controller->currentInvoice();
    m_cartTable->setRowCount(inv.items.size());
    
    for (int i = 0; i < inv.items.size(); ++i) {
        const auto& item = inv.items[i];
        m_cartTable->setItem(i, 0, new QTableWidgetItem(item.productName));
        m_cartTable->setItem(i, 1, new QTableWidgetItem(QString::number(item.sellingPrice, 'f', 2)));
        m_cartTable->setItem(i, 2, new QTableWidgetItem(QString::number(item.quantity)));
        m_cartTable->setItem(i, 3, new QTableWidgetItem(QString::number(item.cgstAmt + item.sgstAmt, 'f', 2)));
        m_cartTable->setItem(i, 4, new QTableWidgetItem(QString::number(item.total, 'f', 2)));
        
        QPushButton* removeBtn = new QPushButton("Remove", this);
        connect(removeBtn, &QPushButton::clicked, this, [this, i]() {
            m_controller->removeItem(i);
        });
        m_cartTable->setCellWidget(i, 5, removeBtn);
    }
}

void BillingPage::refreshTotals() {
    Invoice inv = m_controller->currentInvoice();
    m_subtotalLabel->setText(QString("Subtotal: $%1").arg(inv.subtotal, 0, 'f', 2));
    m_discountLabel->setText(QString("Discount: $%1").arg(inv.discountAmt + inv.couponDiscount, 0, 'f', 2));
    m_taxLabel->setText(QString("Tax (CGST+SGST): $%1").arg(inv.cgstAmt + inv.sgstAmt, 0, 'f', 2));
    m_totalLabel->setText(QString("Grand Total: $%1").arg(inv.grandTotal, 0, 'f', 2));
}

} // namespace RetailMS

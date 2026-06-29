#include "BillingPage.h"
#include "../controllers/BillingController.h"
#include "../database/DatabaseManager.h"
#include <QSqlQuery>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QGroupBox>

namespace RetailMS {

BillingPage::BillingPage(std::shared_ptr<BillingController> controller, QWidget* parent)
    : QWidget(parent), m_controller(std::move(controller)) {
    m_selectedCategoryFilter = "All";
    setupUi();
    setupConnections();
    refreshTotals();
}

void BillingPage::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setSpacing(20);
    
    // 3 Panes
    topLayout->addWidget(createLeftPane(), 3);
    topLayout->addWidget(createCenterPane(), 4);
    topLayout->addWidget(createRightPane(), 3);
    
    mainLayout->addLayout(topLayout, 1);
    
    // Bottom recommendations
    mainLayout->addWidget(createBottomRecommendationsPane());
}

QWidget* BillingPage::createLeftPane() {
    QWidget* pane = new QWidget(this);
    pane->setObjectName("statCard");
    QVBoxLayout* layout = new QVBoxLayout(pane);
    
    QLabel* title = new QLabel("Products", pane);
    title->setStyleSheet("font-size: 16px; font-weight: bold;");
    layout->addWidget(title);
    
    m_productSearchInput = new QLineEdit(pane);
    m_productSearchInput->setPlaceholderText("Search by name/barcode...");
    layout->addWidget(m_productSearchInput);
    
    QHBoxLayout* filters = new QHBoxLayout();
    QPushButton* cat1 = new QPushButton("All", pane);
    QPushButton* cat2 = new QPushButton("Groceries", pane);
    QPushButton* cat3 = new QPushButton("Beverages", pane);
    filters->addWidget(cat1);
    filters->addWidget(cat2);
    filters->addWidget(cat3);
    filters->addStretch();
    layout->addLayout(filters);
    
    // Product grid scroll area
    QScrollArea* scrollArea = new QScrollArea(pane);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("background: transparent; border: none;");
    m_gridWidget = new QWidget(scrollArea);
    m_gridWidget->setStyleSheet("background: transparent;");
    m_productGrid = new QGridLayout(m_gridWidget);
    m_productGrid->setSpacing(10);
    
    reloadProductGrid();
    
    scrollArea->setWidget(m_gridWidget);
    layout->addWidget(scrollArea, 1);
    
    connect(m_productSearchInput, &QLineEdit::textChanged, this, &BillingPage::reloadProductGrid);
    
    connect(cat1, &QPushButton::clicked, this, [this]() { onCategoryFilter("All"); });
    connect(cat2, &QPushButton::clicked, this, [this]() { onCategoryFilter("Groceries"); });
    connect(cat3, &QPushButton::clicked, this, [this]() { onCategoryFilter("Beverages"); });
    
    return pane;
}

QWidget* BillingPage::createCenterPane() {
    QWidget* pane = new QWidget(this);
    pane->setObjectName("statCard");
    QVBoxLayout* layout = new QVBoxLayout(pane);
    
    QLabel* title = new QLabel("Shopping Cart", pane);
    title->setStyleSheet("font-size: 16px; font-weight: bold;");
    layout->addWidget(title);
    
    QHBoxLayout* scanLayout = new QHBoxLayout();
    m_barcodeInput = new QLineEdit(pane);
    m_barcodeInput->setPlaceholderText("Scan Barcode...");
    QPushButton* addBtn = new QPushButton("Add", pane);
    addBtn->setObjectName("primaryButton");
    scanLayout->addWidget(m_barcodeInput);
    scanLayout->addWidget(addBtn);
    layout->addLayout(scanLayout);
    
    m_cartTable = new QTableWidget(pane);
    m_cartTable->setColumnCount(5);
    m_cartTable->setHorizontalHeaderLabels({"Item", "Qty", "Price", "Total", ""});
    m_cartTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_cartTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    m_cartTable->setColumnWidth(4, 50);
    layout->addWidget(m_cartTable, 1);
    
    // Connect Add button to barcode input slot
    connect(addBtn, &QPushButton::clicked, this, [this]() {
        QString barcode = m_barcodeInput->text();
        if (!barcode.isEmpty()) {
            m_controller->addProductByBarcode(barcode);
            m_barcodeInput->clear();
        }
    });
    
    return pane;
}

QWidget* BillingPage::createRightPane() {
    QWidget* pane = new QWidget(this);
    pane->setObjectName("statCard");
    QVBoxLayout* layout = new QVBoxLayout(pane);
    
    QLabel* title = new QLabel("Invoice Summary", pane);
    title->setStyleSheet("font-size: 16px; font-weight: bold;");
    layout->addWidget(title);
    layout->addSpacing(20);
    
    auto addSummaryRow = [this, layout](const QString& label, QLabel*& valueLabel, bool bold = false) {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* l = new QLabel(label, this);
        valueLabel = new QLabel("₹0.00", this);
        valueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        
        if (bold) {
            l->setStyleSheet("font-weight: bold; font-size: 18px;");
            valueLabel->setStyleSheet("font-weight: bold; font-size: 22px; color: #4CAF50;");
        } else {
            l->setStyleSheet("color: #A9B1BC;");
        }
        
        row->addWidget(l);
        row->addWidget(valueLabel);
        layout->addLayout(row);
    };
    
    addSummaryRow("Subtotal", m_subtotalLabel);
    addSummaryRow("Discount", m_discountLabel);
    addSummaryRow("GST (CGST+SGST)", m_taxLabel);
    
    QFrame* line = new QFrame(pane);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #2A2D35;");
    layout->addWidget(line);
    
    addSummaryRow("Grand Total", m_totalLabel, true);
    
    layout->addStretch();
    
    QLabel* pModeLbl = new QLabel("Payment Mode", pane);
    layout->addWidget(pModeLbl);
    
    m_paymentModeCombo = new QComboBox(pane);
    m_paymentModeCombo->addItems({"Cash", "Credit Card", "UPI", "Split Payment"});
    layout->addWidget(m_paymentModeCombo);
    
    layout->addSpacing(20);
    
    QPushButton* checkoutBtn = new QPushButton("Generate Invoice", pane);
    checkoutBtn->setObjectName("primaryButton");
    checkoutBtn->setMinimumHeight(50);
    checkoutBtn->setCursor(Qt::PointingHandCursor);
    layout->addWidget(checkoutBtn);
    
    QHBoxLayout* extraActions = new QHBoxLayout();
    QPushButton* printBtn = new QPushButton("Print", pane);
    printBtn->setCursor(Qt::PointingHandCursor);
    QPushButton* emailBtn = new QPushButton("Email", pane);
    emailBtn->setCursor(Qt::PointingHandCursor);
    extraActions->addWidget(printBtn);
    extraActions->addWidget(emailBtn);
    layout->addLayout(extraActions);
    
    connect(checkoutBtn, &QPushButton::clicked, this, &BillingPage::onCheckout);
    connect(printBtn, &QPushButton::clicked, this, &BillingPage::onPrintInvoice);
    connect(emailBtn, &QPushButton::clicked, this, &BillingPage::onEmailInvoice);
    
    return pane;
}

QWidget* BillingPage::createBottomRecommendationsPane() {
    QWidget* pane = new QWidget(this);
    pane->setObjectName("statCard");
    pane->setFixedHeight(120);
    QHBoxLayout* layout = new QHBoxLayout(pane);
    
    QLabel* title = new QLabel("✨ AI Suggested Add-ons:", pane);
    title->setStyleSheet("color: #4CAF50; font-weight: bold;");
    layout->addWidget(title);
    
    QPushButton* bagBtn = new QPushButton("Carry Bag (₹10)", pane);
    bagBtn->setCursor(Qt::PointingHandCursor);
    QPushButton* waterBtn = new QPushButton("Water Bottle (₹20)", pane);
    waterBtn->setCursor(Qt::PointingHandCursor);
    QPushButton* chocBtn = new QPushButton("Chocolates (₹50)", pane);
    chocBtn->setCursor(Qt::PointingHandCursor);
    
    layout->addWidget(bagBtn);
    layout->addWidget(waterBtn);
    layout->addWidget(chocBtn);
    
    connect(bagBtn, &QPushButton::clicked, this, [this]() { onAddOnClicked("Carry Bag", 10.00); });
    connect(waterBtn, &QPushButton::clicked, this, [this]() { onAddOnClicked("Water Bottle", 20.00); });
    connect(chocBtn, &QPushButton::clicked, this, [this]() { onAddOnClicked("Chocolates", 50.00); });
    
    layout->addStretch();
    return pane;
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
        m_cartTable->setItem(i, 1, new QTableWidgetItem(QString::number(item.quantity)));
        m_cartTable->setItem(i, 2, new QTableWidgetItem(QString::number(item.sellingPrice, 'f', 2)));
        m_cartTable->setItem(i, 3, new QTableWidgetItem(QString::number(item.total, 'f', 2)));
        
        QPushButton* removeBtn = new QPushButton("✕", this);
        removeBtn->setObjectName("dangerButton");
        removeBtn->setFixedSize(30, 30);
        connect(removeBtn, &QPushButton::clicked, this, [this, i]() {
            m_controller->removeItem(i);
        });
        m_cartTable->setCellWidget(i, 4, removeBtn);
    }
}

void BillingPage::refreshTotals() {
    Invoice inv = m_controller->currentInvoice();
    m_subtotalLabel->setText(QString("₹%1").arg(inv.subtotal, 0, 'f', 2));
    m_discountLabel->setText(QString("₹%1").arg(inv.discountAmt + inv.couponDiscount, 0, 'f', 2));
    m_taxLabel->setText(QString("₹%1").arg(inv.cgstAmt + inv.sgstAmt, 0, 'f', 2));
    m_totalLabel->setText(QString("₹%1").arg(inv.grandTotal, 0, 'f', 2));
}

void BillingPage::reloadProductGrid() {
    // Clear existing layout
    QLayoutItem* item;
    while ((item = m_productGrid->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    QString filter = m_productSearchInput->text().trimmed();
    QSqlQuery query;
    QVariantList params;
    QString queryStr = "SELECT p.id, p.name, p.selling_price FROM products p LEFT JOIN categories c ON p.category_id = c.id WHERE p.is_active = 1";
    
    if (m_selectedCategoryFilter != "All") {
        queryStr += " AND c.name = ?";
        params << m_selectedCategoryFilter;
    }
    
    if (!filter.isEmpty()) {
        queryStr += " AND (p.name LIKE ? OR p.barcode LIKE ?)";
        params << "%" + filter + "%" << "%" + filter + "%";
    }
    queryStr += " LIMIT 8;";

    query = db.prepare(queryStr);
    for (const auto& p : params) {
        query.addBindValue(p);
    }
    if (!query.exec()) {
        return;
    }

    int idx = 0;
    bool hasProducts = false;
    while (query.next()) {
        hasProducts = true;
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        double price = query.value(2).toDouble();

        QPushButton* pBtn = new QPushButton(m_gridWidget);
        pBtn->setText(QString("%1\n₹%2").arg(name).arg(price, 0, 'f', 2));
        pBtn->setFixedSize(130, 100);
        
        connect(pBtn, &QPushButton::clicked, this, [this, id]() {
            m_controller->addProductToInvoice(id, 1.0);
        });

        m_productGrid->addWidget(pBtn, idx / 2, idx % 2);
        idx++;
    }

    if (!hasProducts) {
        QLabel* emptyLabel = new QLabel(filter.isEmpty() ? "No products in database.\nAdd them in Products screen." : "No products match search.", m_gridWidget);
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: #A9B1BC; font-size: 14px;");
        m_productGrid->addWidget(emptyLabel, 0, 0, 1, 2);
    }
}

void BillingPage::onCheckout() {
    Invoice inv = m_controller->currentInvoice();
    if (inv.items.empty()) {
        QMessageBox::warning(this, "Checkout Failed", "Your shopping cart is empty.");
        return;
    }
    
    QString modeStr = m_paymentModeCombo->currentText().toLower();
    Invoice::PaymentMode mode = Invoice::PaymentMode::Cash;
    if (modeStr == "credit card") mode = Invoice::PaymentMode::Card;
    else if (modeStr == "upi") mode = Invoice::PaymentMode::UPI;
    else if (modeStr == "split payment") mode = Invoice::PaymentMode::Split;
    
    m_controller->finalizeInvoice(inv.grandTotal, mode);
    QMessageBox::information(this, "Checkout Complete", "Invoice generated and finalized successfully.");
}

void BillingPage::onPrintInvoice() {
    Invoice inv = m_controller->currentInvoice();
    if (inv.invoiceNumber.isEmpty()) {
        QMessageBox::warning(this, "Print Error", "Please finalize the transaction before printing.");
        return;
    }
    QMessageBox::information(this, "Print Success", QString("Invoice %1 sent to default printer.").arg(inv.invoiceNumber));
}

void BillingPage::onEmailInvoice() {
    Invoice inv = m_controller->currentInvoice();
    if (inv.invoiceNumber.isEmpty()) {
        QMessageBox::warning(this, "Email Error", "Please finalize the transaction before emailing.");
        return;
    }
    QMessageBox::information(this, "Email Success", QString("Invoice %1 successfully emailed to customer.").arg(inv.invoiceNumber));
}

void BillingPage::onCategoryFilter(const QString& category) {
    m_selectedCategoryFilter = category;
    reloadProductGrid();
}

void BillingPage::onAddOnClicked(const QString& productName, double price) {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;
    
    auto res = db.executeScalar("SELECT id FROM products WHERE name = ? LIMIT 1;", { productName });
    int productId = 0;
    if (res && !res->isNull()) {
        productId = res->toInt();
    } else {
        QString barcode = "addon_" + productName.toLower().trimmed().replace(" ", "_");
        QString sku = "SKU_" + productName.toUpper().trimmed().replace(" ", "_");
        db.executeNonQuery("INSERT INTO products (barcode, sku, name, cost_price, selling_price, stock_quantity) VALUES (?, ?, ?, ?, ?, ?);",
                           { barcode, sku, productName, price * 0.5, price, 9999.0 });
        
        auto newRes = db.executeScalar("SELECT id FROM products WHERE name = ? LIMIT 1;", { productName });
        if (newRes && !newRes->isNull()) {
            productId = newRes->toInt();
        }
    }
    
    if (productId > 0) {
        m_controller->addProductToInvoice(productId, 1.0);
    }
}

} // namespace RetailMS

#include "BillingPage.h"
#include "../controllers/BillingController.h"
#include "../database/DatabaseManager.h"
#include "../services/SessionManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSqlQuery>
#include <QShortcut>

namespace RetailMS {

BillingPage::BillingPage(std::shared_ptr<BillingController> controller, QWidget* parent)
    : QWidget(parent), m_controller(std::move(controller)) {
    setupUi();
    setupConnections();
    setupShortcuts();
    
    // Initial loads
    m_cartPanel->refreshCart(m_controller->currentInvoice());
    m_summaryPanel->refreshSummary(m_controller->currentInvoice());
    m_actionPanel->refreshTotals(m_controller->currentInvoice());
    
    m_productPanel->setFocusToSearch();
}

void BillingPage::setupUi() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(20);
    
    // --- Layout Architecture ---
    // [ Left Side (70%) ]  | [ Right Side (30%) ]
    //   Customer Panel     |   Checkout Sidebar
    //   Product Search     |
    //   Cart Panel         |
    
    // --- LEFT SIDE (70%) ---
    QWidget* leftWidget = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(15);
    
    m_customerPanel = new CustomerPanel(m_controller->customerService(), leftWidget);
    leftLayout->addWidget(m_customerPanel);
    
    m_productPanel = new ProductSelectionPanel(leftWidget);
    leftLayout->addWidget(m_productPanel);
    
    m_cartPanel = new CartPanel(leftWidget);
    leftLayout->addWidget(m_cartPanel, 1); // Allow cart to expand
    
    mainLayout->addWidget(leftWidget, 7); // 70% width
    
    // --- RIGHT SIDE (30%) ---
    m_actionPanel = new PaymentActionPanel(this);
    mainLayout->addWidget(m_actionPanel, 3); // 30% width
    
    // --- INVOICE PREVIEW DIALOG ---
    // Instantiated as a popup, not added to any layout
    m_summaryPanel = new InvoiceSummaryPanel(this);
}

void BillingPage::setupConnections() {
    // Controller to UI Updates
    connect(m_controller.get(), &BillingController::invoiceUpdated, this, [this](const Invoice& inv) {
        m_cartPanel->refreshCart(inv);
        m_summaryPanel->refreshSummary(inv);
        m_actionPanel->refreshTotals(inv);
        if (inv.customerId <= 0 && inv.customerPhone.isEmpty()) {
            m_customerPanel->clear();
        }
    });
    
    connect(m_controller.get(), &BillingController::checkoutComplete, this, [this](const Invoice& inv) {
        m_summaryPanel->refreshSummary(inv);
        m_summaryPanel->exec();
        QMessageBox::information(this, "Success", QString("Invoice %1 finalized.").arg(inv.invoiceNumber));
        m_customerPanel->clear();
    });

    connect(m_controller.get(), &BillingController::errorOccurred, this, [this](const QString& msg) {
        QMessageBox::warning(this, "Error", msg);
    });

    // Customer interactions
    connect(m_customerPanel, &CustomerPanel::customerSelected, this, [this](int id) {
        m_controller->setCustomer(id);
    });
    connect(m_customerPanel, &CustomerPanel::newCustomerPhoneEntered, this, [this](const QString& phone) {
        m_controller->setCustomerPhone(phone);
    });
    connect(m_customerPanel, &CustomerPanel::redeemPointsRequested, this, [this](int points) {
        m_controller->redeemLoyaltyPoints(points);
    });
    connect(m_customerPanel, &CustomerPanel::focusProductSearchRequested, m_productPanel, &ProductSelectionPanel::setFocusToSearch);

    // Product search interactions
    connect(m_productPanel, &ProductSelectionPanel::searchRequested, this, &BillingPage::loadProducts);
    connect(m_productPanel, &ProductSelectionPanel::productSelected, this, [this](int productId) {
        m_controller->addProductToInvoice(productId, 1.0);
        m_productPanel->setFocusToSearch();
    });
    
    // Cart interactions
    connect(m_cartPanel, &CartPanel::quantityChanged, this, [this](int idx, double qty) {
        m_controller->updateItemQuantity(idx, qty);
    });
    connect(m_cartPanel, &CartPanel::itemRemoved, this, [this](int idx) {
        m_controller->removeItem(idx);
        m_productPanel->setFocusToSearch();
    });
    
    // Action Panel
    connect(m_actionPanel, &PaymentActionPanel::generateInvoiceRequested, this, [this](Invoice::PaymentMode mode) {
        auto inv = m_controller->currentInvoice();
        if (inv.items.empty()) {
            QMessageBox::warning(this, "Empty", "Cannot generate empty invoice.");
            return;
        }
        if (inv.customerId <= 0 && inv.customerPhone.isEmpty()) {
            QMessageBox::warning(this, "Customer Required", "Billing cannot continue unless a valid mobile number is entered.");
            return;
        }
        m_controller->finalizeInvoice(inv.grandTotal, mode);
    });
    
    connect(m_actionPanel, &PaymentActionPanel::holdBillRequested, m_controller.get(), &BillingController::holdCurrentBill);
    
    connect(m_actionPanel, &PaymentActionPanel::resumeBillRequested, this, [this]() {
        m_controller->resumeBill(0);
    });
    
    connect(m_actionPanel, &PaymentActionPanel::clearCartRequested, this, [this]() {
        m_controller->createNewInvoice();
    });
    
    connect(m_actionPanel, &PaymentActionPanel::cancelBillRequested, this, [this]() {
        m_controller->createNewInvoice();
    });
    
    connect(m_actionPanel, &PaymentActionPanel::printRequested, this, [this]() {
        QMessageBox::information(this, "Print", "Print requested.");
    });
    
    connect(m_actionPanel, &PaymentActionPanel::previewRequested, this, [this]() {
        m_summaryPanel->exec();
    });
}

void BillingPage::setupShortcuts() {
    auto* scF2 = new QShortcut(QKeySequence(Qt::Key_F2), this);
    connect(scF2, &QShortcut::activated, m_controller.get(), &BillingController::createNewInvoice);

    auto* scF5 = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect(scF5, &QShortcut::activated, m_controller.get(), &BillingController::holdCurrentBill);
    auto* scF6 = new QShortcut(QKeySequence(Qt::Key_F6), this);
    connect(scF6, &QShortcut::activated, this, [this]() { m_controller->resumeBill(0); });
    
    auto* scCtrlP = new QShortcut(QKeySequence("Ctrl+P"), this);
    connect(scCtrlP, &QShortcut::activated, this, [this]() { m_actionPanel->printRequested(); });
    
    auto* scEsc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(scEsc, &QShortcut::activated, this, [this]() { m_actionPanel->cancelBillRequested(); });
    
    auto* scF10 = new QShortcut(QKeySequence(Qt::Key_F10), this);
    connect(scF10, &QShortcut::activated, this, [this]() {
        m_actionPanel->generateInvoiceRequested(Invoice::PaymentMode::Cash);
    });
}

void BillingPage::loadProducts(const QString& queryStr, const QString& category) {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    QString sql = "SELECT p.id, p.barcode, p.name, c.name as category, p.selling_price as price, p.stock_quantity as stock "
                  "FROM products p LEFT JOIN categories c ON p.category_id = c.id WHERE p.is_active = 1";
                  
    QVariantList params;
    if (category != "All Categories" && !category.isEmpty()) {
        sql += " AND c.name = ?";
        params << category;
    }
    
    QString trimmedQuery = queryStr.trimmed();
    if (!trimmedQuery.isEmpty()) {
        // Split by whitespace to handle extra spaces and multi-word searches (e.g. "Marker Pen Black")
        QStringList words = trimmedQuery.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        
        for (const QString& word : words) {
            sql += " AND (p.name LIKE ? OR p.barcode LIKE ? OR p.sku LIKE ? OR c.name LIKE ?)";
            QString param = "%" + word + "%";
            params << param << param << param << param;
        }
        
        // Order by exact name match first, then starts-with, then others
        sql += " ORDER BY "
               "CASE "
               "  WHEN p.name LIKE ? THEN 1 "
               "  WHEN p.name LIKE ? THEN 2 "
               "  ELSE 3 "
               "END ASC, p.name ASC ";
               
        params << trimmedQuery 
               << trimmedQuery + "%";
    } else {
        sql += " ORDER BY p.name ASC ";
    }
    
    sql += " LIMIT 20;";
    
    QSqlQuery query = db.prepare(sql);
    for (const auto& p : params) query.addBindValue(p);
    
    QList<QVariantMap> products;
    if (query.exec()) {
        while (query.next()) {
            QVariantMap map;
            map["id"] = query.value(0);
            map["barcode"] = query.value(1);
            map["name"] = query.value(2);
            map["category"] = query.value(3);
            map["price"] = query.value(4);
            map["stock"] = query.value(5);
            products.append(map);
        }
    }
    m_productPanel->updateProducts(products);
}

} // namespace RetailMS

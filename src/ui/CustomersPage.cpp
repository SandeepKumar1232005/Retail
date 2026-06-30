#include "CustomersPage.h"
#include "../services/CustomerService.h"
#include "../controllers/BillingController.h"
#include "../database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QSqlQuery>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QSpinBox>

namespace RetailMS {

CustomersPage::CustomersPage(std::shared_ptr<CustomerService> customerService,
                             std::shared_ptr<BillingController> billingController,
                             QWidget* parent)
    : QWidget(parent),
      m_customerService(std::move(customerService)),
      m_billingController(std::move(billingController)) {
    setupUi();
    setupConnections();
    refreshData();
}

void CustomersPage::setupUi() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // --- LEFT SIDE: CUSTOMERS TABLE ---
    QWidget* leftWidget = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(15);

    QLabel* titleLabel = new QLabel("Customer Directory", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #FFFFFF;");
    leftLayout->addWidget(titleLabel);

    // Filter controls
    QHBoxLayout* filterLayout = new QHBoxLayout();
    m_searchField = new QLineEdit(this);
    m_searchField->setPlaceholderText("Search by Name or Phone...");
    m_searchField->setFixedWidth(300);

    m_refreshButton = new QPushButton("Refresh", this);
    m_refreshButton->setObjectName("primaryButton");
    m_refreshButton->setFixedWidth(100);

    m_rulesButton = new QPushButton("⚙ Loyalty Rules", this);
    m_rulesButton->setFixedWidth(120);

    filterLayout->addWidget(m_searchField);
    filterLayout->addWidget(m_refreshButton);
    filterLayout->addWidget(m_rulesButton);
    filterLayout->addStretch();
    leftLayout->addLayout(filterLayout);

    // Customers Table
    m_customersTable = new QTableWidget(this);
    m_customersTable->setColumnCount(5);
    m_customersTable->setHorizontalHeaderLabels({"ID", "Name", "Mobile Phone", "Points", "Tier"});
    m_customersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_customersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_customersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_customersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_customersTable->verticalHeader()->setVisible(false);
    m_customersTable->setStyleSheet(
        "QTableWidget { background-color: #1E2025; border: 1px solid #2A2D35; border-radius: 8px; }"
    );
    leftLayout->addWidget(m_customersTable, 1);

    mainLayout->addWidget(leftWidget, 5); // 50% width

    // --- RIGHT SIDE: MASTER-DETAIL DASHBOARD ---
    QWidget* rightWidget = new QWidget(this);
    rightWidget->setObjectName("statCard");
    rightWidget->setStyleSheet("QWidget#statCard { background-color: #16181B; border: 1px solid #2A2D35; border-radius: 12px; }");
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(20, 20, 20, 20);
    rightLayout->setSpacing(15);

    QLabel* profileTitle = new QLabel("Customer Profile Summary", rightWidget);
    profileTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF;");
    rightLayout->addWidget(profileTitle);

    // Profile Details Grid
    QGridLayout* detailsGrid = new QGridLayout();
    detailsGrid->setSpacing(10);

    auto addDetailRow = [&](const QString& labelText, QLabel*& valLabel, int r, int c) {
        QLabel* lbl = new QLabel(labelText, rightWidget);
        lbl->setStyleSheet("color: #A9B1BC; font-weight: 500; font-size: 13px;");
        valLabel = new QLabel("-", rightWidget);
        valLabel->setStyleSheet("color: #FFFFFF; font-weight: bold; font-size: 13px;");
        detailsGrid->addWidget(lbl, r, c * 2);
        detailsGrid->addWidget(valLabel, r, c * 2 + 1);
    };

    addDetailRow("Name:", m_detailName, 0, 0);
    addDetailRow("Phone:", m_detailPhone, 1, 0);
    addDetailRow("Email:", m_detailEmail, 2, 0);

    addDetailRow("Membership Tier:", m_detailTier, 0, 1);
    addDetailRow("Loyalty Points:", m_detailPoints, 1, 1);
    addDetailRow("Total Spent:", m_detailTotalSpent, 2, 1);

    addDetailRow("Total Orders:", m_detailTotalOrders, 3, 0);
    addDetailRow("Avg Order Value:", m_detailAvgOrderVal, 3, 1);
    addDetailRow("Last Visit:", m_detailLastVisit, 4, 0);

    rightLayout->addLayout(detailsGrid);

    // Action buttons for admin
    QHBoxLayout* actionsLayout = new QHBoxLayout();
    m_editBtn = new QPushButton("Edit Profile", rightWidget);
    m_editBtn->setFixedWidth(120);
    m_adjustPointsBtn = new QPushButton("Adjust Points", rightWidget);
    m_adjustPointsBtn->setFixedWidth(120);

    actionsLayout->addWidget(m_editBtn);
    actionsLayout->addWidget(m_adjustPointsBtn);
    actionsLayout->addStretch();
    rightLayout->addLayout(actionsLayout);

    // Dynamic Lists (History & Favorites)
    QHBoxLayout* listsLayout = new QHBoxLayout();
    listsLayout->setSpacing(15);

    // History Table (Left)
    QWidget* histContainer = new QWidget(rightWidget);
    QVBoxLayout* histLayout = new QVBoxLayout(histContainer);
    histLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* histTitle = new QLabel("Purchase History", histContainer);
    histTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #4CAF50;");
    histLayout->addWidget(histTitle);

    m_historyTable = new QTableWidget(histContainer);
    m_historyTable->setColumnCount(4);
    m_historyTable->setHorizontalHeaderLabels({"Inv #", "Date", "Total", "Mode"});
    m_historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_historyTable->verticalHeader()->setVisible(false);
    m_historyTable->setStyleSheet("QTableWidget { background-color: #1E2025; border: 1px solid #2A2D35; border-radius: 8px; }");
    m_historyTable->setFixedHeight(220);
    histLayout->addWidget(m_historyTable);
    listsLayout->addWidget(histContainer, 3);

    // Favorites Table (Right)
    QWidget* favContainer = new QWidget(rightWidget);
    QVBoxLayout* favLayout = new QVBoxLayout(favContainer);
    favLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* favTitle = new QLabel("Favorite Products", favContainer);
    favTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #FFC107;");
    favLayout->addWidget(favTitle);

    m_favoritesTable = new QTableWidget(favContainer);
    m_favoritesTable->setColumnCount(2);
    m_favoritesTable->setHorizontalHeaderLabels({"Product", "Qty Bought"});
    m_favoritesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_favoritesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_favoritesTable->verticalHeader()->setVisible(false);
    m_favoritesTable->setStyleSheet("QTableWidget { background-color: #1E2025; border: 1px solid #2A2D35; border-radius: 8px; }");
    m_favoritesTable->setFixedHeight(220);
    favLayout->addWidget(m_favoritesTable);
    listsLayout->addWidget(favContainer, 2);

    rightLayout->addLayout(listsLayout);
    rightLayout->addStretch();

    mainLayout->addWidget(rightWidget, 5); // 50% width
}

void CustomersPage::setupConnections() {
    connect(m_refreshButton, &QPushButton::clicked, this, &CustomersPage::refreshData);
    connect(m_searchField, &QLineEdit::textChanged, this, &CustomersPage::refreshData);
    connect(m_customersTable, &QTableWidget::itemSelectionChanged, this, &CustomersPage::onCustomerSelected);

    connect(m_editBtn, &QPushButton::clicked, this, &CustomersPage::onEditCustomerClicked);
    connect(m_adjustPointsBtn, &QPushButton::clicked, this, &CustomersPage::onAdjustPointsClicked);
    connect(m_rulesButton, &QPushButton::clicked, this, &CustomersPage::onLoyaltyRulesClicked);

    // Refresh directory when checkout happens successfully
    connect(m_billingController.get(), &BillingController::checkoutComplete, this, &CustomersPage::refreshData);
}

void CustomersPage::refreshData() {
    if (!m_customerService) return;

    QString search = m_searchField->text().trimmed();
    std::vector<Customer> customers;
    if (search.isEmpty()) {
        customers = m_customerService->getAllCustomers();
    } else {
        customers = m_customerService->searchCustomers(search);
    }

    m_customersTable->setRowCount(0);
    for (const auto& c : customers) {
        int row = m_customersTable->rowCount();
        m_customersTable->insertRow(row);

        QTableWidgetItem* idItem = new QTableWidgetItem(QString("CUS-%1").arg(c.id, 6, 10, QChar('0')));
        idItem->setData(Qt::UserRole, c.id);
        m_customersTable->setItem(row, 0, idItem);
        m_customersTable->setItem(row, 1, new QTableWidgetItem(c.name));
        m_customersTable->setItem(row, 2, new QTableWidgetItem(c.phone));
        m_customersTable->setItem(row, 3, new QTableWidgetItem(QString::number(c.loyaltyPoints)));
        
        QTableWidgetItem* tierItem = new QTableWidgetItem(c.tier.toUpper());
        if (c.tier == "platinum") tierItem->setForeground(QBrush(QColor("#8B5CF6")));
        else if (c.tier == "gold") tierItem->setForeground(QBrush(QColor("#F59E0B")));
        else if (c.tier == "silver") tierItem->setForeground(QBrush(QColor("#9CA3AF")));
        m_customersTable->setItem(row, 4, tierItem);
    }

    // Clear detail summaries
    m_detailName->setText("-");
    m_detailPhone->setText("-");
    m_detailEmail->setText("-");
    m_detailTier->setText("-");
    m_detailPoints->setText("-");
    m_detailTotalSpent->setText("-");
    m_detailTotalOrders->setText("-");
    m_detailAvgOrderVal->setText("-");
    m_detailLastVisit->setText("-");
    m_historyTable->setRowCount(0);
    m_favoritesTable->setRowCount(0);
    m_selectedCustomerId = -1;
}

void CustomersPage::onCustomerSelected() {
    int row = m_customersTable->currentRow();
    if (row < 0) return;

    QTableWidgetItem* idItem = m_customersTable->item(row, 0);
    if (!idItem) return;

    int customerId = idItem->data(Qt::UserRole).toInt();
    m_selectedCustomerId = customerId;
    loadCustomerDetails(customerId);
}

void CustomersPage::loadCustomerDetails(int customerId) {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    auto optC = m_customerService->getCustomerById(customerId);
    if (!optC) return;
    Customer c = optC.value();

    m_detailName->setText(c.name);
    m_detailPhone->setText(c.phone);
    m_detailEmail->setText(c.email.isEmpty() ? "N/A" : c.email);

    QString tierColor = "#9CA3AF";
    if (c.tier == "platinum") tierColor = "#8B5CF6";
    else if (c.tier == "gold") tierColor = "#F59E0B";
    else if (c.tier == "silver") tierColor = "#9CA3AF";
    m_detailTier->setText(c.tier.toUpper());
    m_detailTier->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 13px;").arg(tierColor));

    m_detailPoints->setText(QString::number(c.loyaltyPoints));
    m_detailTotalSpent->setText(QString("₹%1").arg(c.totalSpent, 0, 'f', 2));

    // dynamic queries
    auto ordRes = db.executeScalar("SELECT COUNT(*) FROM invoices WHERE customer_id = ? AND status = 'paid';", {customerId});
    int totalOrders = ordRes ? ordRes->toInt() : 0;
    m_detailTotalOrders->setText(QString::number(totalOrders));

    auto avgRes = db.executeScalar("SELECT AVG(grand_total) FROM invoices WHERE customer_id = ? AND status = 'paid';", {customerId});
    double avgOrderVal = avgRes && !avgRes->isNull() ? avgRes->toDouble() : 0.0;
    m_detailAvgOrderVal->setText(QString("₹%1").arg(avgOrderVal, 0, 'f', 2));

    auto visitRes = db.executeScalar("SELECT MAX(invoice_date) FROM invoices WHERE customer_id = ? AND status = 'paid';", {customerId});
    QString lastVisit = visitRes && !visitRes->isNull() ? visitRes->toDateTime().toString("yyyy-MM-dd HH:mm") : "N/A";
    m_detailLastVisit->setText(lastVisit);

    // Purchase history Table
    m_historyTable->setRowCount(0);
    QSqlQuery query = db.prepare("SELECT invoice_number, invoice_date, grand_total, payment_mode FROM invoices WHERE customer_id = ? ORDER BY invoice_date DESC;");
    query.addBindValue(customerId);
    if (query.exec()) {
        while (query.next()) {
            int r = m_historyTable->rowCount();
            m_historyTable->insertRow(r);
            m_historyTable->setItem(r, 0, new QTableWidgetItem(query.value(0).toString()));
            m_historyTable->setItem(r, 1, new QTableWidgetItem(query.value(1).toDateTime().toString("yyyy-MM-dd HH:mm")));
            m_historyTable->setItem(r, 2, new QTableWidgetItem(QString("₹%1").arg(query.value(2).toDouble(), 0, 'f', 2)));
            m_historyTable->setItem(r, 3, new QTableWidgetItem(query.value(3).toString().toUpper()));
        }
    }

    // Favorite products Table
    m_favoritesTable->setRowCount(0);
    QSqlQuery queryFav = db.prepare(
        "SELECT product_name, SUM(quantity) as qty "
        "FROM invoice_items ii "
        "JOIN invoices i ON ii.invoice_id = i.id "
        "WHERE i.customer_id = ? AND i.status = 'paid' "
        "GROUP BY product_name "
        "ORDER BY qty DESC LIMIT 5;"
    );
    queryFav.addBindValue(customerId);
    if (queryFav.exec()) {
        while (queryFav.next()) {
            int r = m_favoritesTable->rowCount();
            m_favoritesTable->insertRow(r);
            m_favoritesTable->setItem(r, 0, new QTableWidgetItem(queryFav.value(0).toString()));
            m_favoritesTable->setItem(r, 1, new QTableWidgetItem(QString::number(queryFav.value(1).toDouble())));
        }
    }
}

void CustomersPage::onEditCustomerClicked() {
    if (m_selectedCustomerId < 0) return;

    auto optC = m_customerService->getCustomerById(m_selectedCustomerId);
    if (!optC) return;
    Customer c = optC.value();

    QDialog dialog(this);
    dialog.setWindowTitle("Edit Customer Profile");
    dialog.setStyleSheet(
        "QDialog { background-color: #1B1D20; }"
        "QLabel { color: #FFFFFF; font-size: 13px; }"
        "QLineEdit { background-color: #1E2025; border: 1px solid #374151; border-radius: 4px; padding: 6px; color: #FFF; font-size: 13px; }"
        "QLineEdit:focus { border: 1px solid #4CAF50; }"
    );

    QFormLayout* form = new QFormLayout(&dialog);
    form->setSpacing(10);

    QLineEdit* nameEdit = new QLineEdit(&dialog);
    nameEdit->setText(c.name);

    QLineEdit* phoneEdit = new QLineEdit(&dialog);
    phoneEdit->setText(c.phone);

    QLineEdit* emailEdit = new QLineEdit(&dialog);
    emailEdit->setText(c.email);

    QLineEdit* dobEdit = new QLineEdit(&dialog);
    dobEdit->setText(c.dob.isValid() ? c.dob.toString("yyyy-MM-dd") : "");

    QLineEdit* addressEdit = new QLineEdit(&dialog);
    addressEdit->setText(c.address);

    form->addRow("Name:", nameEdit);
    form->addRow("Mobile Phone:", phoneEdit);
    form->addRow("Email:", emailEdit);
    form->addRow("Date of Birth:", dobEdit);
    form->addRow("Address:", addressEdit);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 4px; padding: 6px 16px; font-weight: bold; }");
    form->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, [&]() {
        if (nameEdit->text().trimmed().isEmpty() || phoneEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Error", "Name and Mobile Phone are required.");
            return;
        }
        dialog.accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        c.name = nameEdit->text().trimmed();
        c.phone = phoneEdit->text().trimmed();
        c.email = emailEdit->text().trimmed();
        c.address = addressEdit->text().trimmed();
        if (!dobEdit->text().trimmed().isEmpty()) {
            c.dob = QDate::fromString(dobEdit->text().trimmed(), "yyyy-MM-dd");
        } else {
            c.dob = QDate();
        }

        if (m_customerService->updateCustomer(c)) {
            QMessageBox::information(this, "Success", "Customer profile updated.");
            refreshData();
        }
    }
}

void CustomersPage::onAdjustPointsClicked() {
    if (m_selectedCustomerId < 0) return;

    auto optC = m_customerService->getCustomerById(m_selectedCustomerId);
    if (!optC) return;
    Customer c = optC.value();

    QDialog dialog(this);
    dialog.setWindowTitle("Adjust Loyalty Points");
    dialog.setStyleSheet(
        "QDialog { background-color: #1B1D20; }"
        "QLabel { color: #FFFFFF; font-size: 13px; }"
    );

    QFormLayout* form = new QFormLayout(&dialog);
    form->setSpacing(10);

    QLabel* currentPoints = new QLabel(QString::number(c.loyaltyPoints), &dialog);
    currentPoints->setStyleSheet("font-weight: bold; color: #FFC107; font-size: 14px;");

    QSpinBox* adjustSpin = new QSpinBox(&dialog);
    adjustSpin->setRange(-9999, 9999);
    adjustSpin->setValue(0);
    adjustSpin->setStyleSheet("QSpinBox { background-color: #1E2025; border: 1px solid #374151; color: white; padding: 6px; }");

    form->addRow("Current Loyalty Points:", currentPoints);
    form->addRow("Points Adjustment (+/-):", adjustSpin);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 4px; padding: 6px 16px; font-weight: bold; }");
    form->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        int adjustment = adjustSpin->value();
        if (adjustment == 0) return;

        c.loyaltyPoints += adjustment;
        if (c.loyaltyPoints < 0) c.loyaltyPoints = 0;

        m_customerService->updateTier(c);
        if (m_customerService->updateCustomer(c)) {
            QMessageBox::information(this, "Success", QString("Adjusted points. New points: %1").arg(c.loyaltyPoints));
            refreshData();
        }
    }
}

void CustomersPage::onLoyaltyRulesClicked() {
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    // Load current values from settings
    auto rateRes = db.executeScalar("SELECT value FROM settings WHERE key = 'loyalty_conversion_rate';");
    double rate = rateRes ? rateRes->toDouble() : 100.0;

    auto r100Res = db.executeScalar("SELECT value FROM settings WHERE key = 'points_discount_100';");
    double disc100 = r100Res ? r100Res->toDouble() : 50.0;

    auto r250Res = db.executeScalar("SELECT value FROM settings WHERE key = 'points_discount_250';");
    double disc250 = r250Res ? r250Res->toDouble() : 150.0;

    auto r500Res = db.executeScalar("SELECT value FROM settings WHERE key = 'points_discount_500';");
    double disc500 = r500Res ? r500Res->toDouble() : 400.0;

    QDialog dialog(this);
    dialog.setWindowTitle("Configure Loyalty Rules");
    dialog.setStyleSheet(
        "QDialog { background-color: #1B1D20; }"
        "QLabel { color: #FFFFFF; font-size: 13px; }"
    );

    QFormLayout* form = new QFormLayout(&dialog);
    form->setSpacing(12);

    QDoubleSpinBox* rateSpin = new QDoubleSpinBox(&dialog);
    rateSpin->setRange(1.0, 9999.0);
    rateSpin->setValue(rate);
    rateSpin->setStyleSheet("QDoubleSpinBox { background-color: #1E2025; border: 1px solid #374151; color: white; padding: 6px; }");
    form->addRow("Rupees spent to earn 1 Point (₹):", rateSpin);

    QDoubleSpinBox* disc100Spin = new QDoubleSpinBox(&dialog);
    disc100Spin->setRange(0.0, 9999.0);
    disc100Spin->setValue(disc100);
    disc100Spin->setStyleSheet("QDoubleSpinBox { background-color: #1E2025; border: 1px solid #374151; color: white; padding: 6px; }");
    form->addRow("Discount for 100 Points (₹):", disc100Spin);

    QDoubleSpinBox* disc250Spin = new QDoubleSpinBox(&dialog);
    disc250Spin->setRange(0.0, 9999.0);
    disc250Spin->setValue(disc250);
    disc250Spin->setStyleSheet("QDoubleSpinBox { background-color: #1E2025; border: 1px solid #374151; color: white; padding: 6px; }");
    form->addRow("Discount for 250 Points (₹):", disc250Spin);

    QDoubleSpinBox* disc500Spin = new QDoubleSpinBox(&dialog);
    disc500Spin->setRange(0.0, 9999.0);
    disc500Spin->setValue(disc500);
    disc500Spin->setStyleSheet("QDoubleSpinBox { background-color: #1E2025; border: 1px solid #374151; color: white; padding: 6px; }");
    form->addRow("Discount for 500 Points (₹):", disc500Spin);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 4px; padding: 6px 16px; font-weight: bold; }");
    form->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        db.executeNonQuery("INSERT OR REPLACE INTO settings (key, value) VALUES ('loyalty_conversion_rate', ?);", {rateSpin->value()});
        db.executeNonQuery("INSERT OR REPLACE INTO settings (key, value) VALUES ('points_discount_100', ?);", {disc100Spin->value()});
        db.executeNonQuery("INSERT OR REPLACE INTO settings (key, value) VALUES ('points_discount_250', ?);", {disc250Spin->value()});
        db.executeNonQuery("INSERT OR REPLACE INTO settings (key, value) VALUES ('points_discount_500', ?);", {disc500Spin->value()});

        QMessageBox::information(this, "Success", "Loyalty configuration rules updated successfully.");
    }
}

} // namespace RetailMS

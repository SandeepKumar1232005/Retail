#include "CustomerPanel.h"
#include "../../services/CustomerService.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>

namespace RetailMS {

CustomerPanel::CustomerPanel(std::shared_ptr<CustomerService> customerService, QWidget* parent)
    : QWidget(parent), m_customerService(std::move(customerService)) {
    setupUi();

    m_searchDebounce = new QTimer(this);
    m_searchDebounce->setSingleShot(true);
    m_searchDebounce->setInterval(300);
    connect(m_searchDebounce, &QTimer::timeout, this, [this]() {
        onPhoneTextChanged(m_phoneInput->text());
    });

    connect(m_phoneInput, &QLineEdit::textChanged, this, [this](const QString&) {
        m_searchDebounce->start();
    });
}

void CustomerPanel::setCustomerService(std::shared_ptr<CustomerService> svc) {
    m_customerService = std::move(svc);
}

void CustomerPanel::setupUi() {
    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    QFrame* frame = new QFrame(this);
    frame->setObjectName("customerCard");
    frame->setStyleSheet(
        "QFrame#customerCard { background-color: transparent; border-bottom: 1px solid #2A2D35; padding: 5px; } "
        "QLabel { color: #FFFFFF; font-size: 13px; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(frame);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mainLayout->setSpacing(6);

    // ── Row 1: Phone search, Walk-in, New Customer, Redeem ──
    QHBoxLayout* row1 = new QHBoxLayout();
    row1->setSpacing(10);

    QLabel* iconLabel = new QLabel("👤 Customer", frame);
    iconLabel->setStyleSheet("font-weight: bold; color: #8C94A1; font-size: 13px;");
    row1->addWidget(iconLabel);

    m_phoneInput = new QLineEdit(frame);
    m_phoneInput->setPlaceholderText("Enter Mobile Number...");
    m_phoneInput->setFixedWidth(180);
    m_phoneInput->setStyleSheet(
        "QLineEdit { background-color: #1E2025; border: 1px solid #374151; border-radius: 4px; padding: 4px 8px; color: #FFF; font-size: 13px; }"
        "QLineEdit:focus { border: 1px solid #4CAF50; }"
    );
    row1->addWidget(m_phoneInput);

    m_welcomeLabel = new QLabel("", frame);
    m_welcomeLabel->setStyleSheet("color: #4CAF50; font-weight: bold; font-size: 13px;");
    m_welcomeLabel->hide();
    row1->addWidget(m_welcomeLabel);

    row1->addStretch();

    m_redeemBtn = new QPushButton("🎁 Redeem Points", frame);
    m_redeemBtn->setStyleSheet(
        "QPushButton { background-color: #FFC107; color: #111; border: none; border-radius: 4px; padding: 4px 10px; font-weight: bold; font-size: 12px; }"
        "QPushButton:hover { background-color: #FFD54F; }"
    );
    m_redeemBtn->setCursor(Qt::PointingHandCursor);
    m_redeemBtn->hide();
    row1->addWidget(m_redeemBtn);

    mainLayout->addLayout(row1);

    // ── Row 2: Customer details strip ──
    QHBoxLayout* row2 = new QHBoxLayout();
    row2->setSpacing(20);

    m_nameLabel = new QLabel("-", frame);
    m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #FFFFFF;");
    row2->addWidget(m_nameLabel);

    m_tierLabel = new QLabel("Walk-in", frame);
    m_tierLabel->setStyleSheet("background-color: #374151; padding: 2px 8px; border-radius: 8px; font-size: 11px;");
    row2->addWidget(m_tierLabel);

    m_pointsLabel = new QLabel("Pts: 0", frame);
    m_pointsLabel->setStyleSheet("color: #FFC107; font-weight: bold; font-size: 12px;");
    row2->addWidget(m_pointsLabel);

    m_totalSpentLabel = new QLabel("Spent: ₹0", frame);
    m_totalSpentLabel->setStyleSheet("color: #A9B1BC; font-size: 12px;");
    row2->addWidget(m_totalSpentLabel);

    m_totalOrdersLabel = new QLabel("Orders: 0", frame);
    m_totalOrdersLabel->setStyleSheet("color: #A9B1BC; font-size: 12px;");
    row2->addWidget(m_totalOrdersLabel);

    m_lastVisitLabel = new QLabel("Last Visit: -", frame);
    m_lastVisitLabel->setStyleSheet("color: #A9B1BC; font-size: 12px;");
    row2->addWidget(m_lastVisitLabel);

    row2->addStretch();

    mainLayout->addLayout(row2);

    outerLayout->addWidget(frame);

    // ── Connections ──
    connect(m_redeemBtn, &QPushButton::clicked, this, &CustomerPanel::onRedeemPointsClicked);
    connect(m_phoneInput, &QLineEdit::returnPressed, this, [this]() {
        emit focusProductSearchRequested();
    });
}

void CustomerPanel::onPhoneTextChanged(const QString& text) {
    QString phone = text.trimmed();
    if (phone.length() < 3 || !m_customerService) {
        return;
    }

    // Search by phone number
    auto results = m_customerService->searchCustomers(phone);

    // Look for an exact phone match first
    for (const auto& c : results) {
        if (c.phone == phone) {
            m_currentCustomerId = c.id;
            m_currentLoyaltyPoints = c.loyaltyPoints;

            // Count orders from database
            setCustomerDetails(c.name, c.phone, c.loyaltyPoints, c.tier, c.totalSpent, 0, "");

            m_welcomeLabel->setText(QString("Welcome back, %1! %2 pts").arg(c.name).arg(c.loyaltyPoints));
            m_welcomeLabel->show();

            if (c.loyaltyPoints >= 100) {
                double discount = m_customerService->getRedemptionDiscount(c.loyaltyPoints);
                m_redeemBtn->setText(QString("🎁 Redeem %1 pts (₹%2 off)").arg(c.loyaltyPoints).arg(discount, 0, 'f', 0));
                m_redeemBtn->show();
            } else {
                m_redeemBtn->hide();
            }

            emit customerSelected(c.id);
            return;
        }
    }

    // No exact match - new customer mode
    m_currentCustomerId = -1;
    m_currentLoyaltyPoints = 0;
    m_welcomeLabel->setText("Will be created automatically");
    m_welcomeLabel->show();
    m_redeemBtn->hide();
    m_nameLabel->setText("-");
    m_tierLabel->setText("New Customer");
    m_tierLabel->setStyleSheet("background-color: #22C55E; padding: 2px 8px; border-radius: 8px; font-size: 11px; color: #111;");
    m_pointsLabel->setText("Pts: 0");
    m_totalSpentLabel->setText("Spent: ₹0");
    m_totalOrdersLabel->setText("Orders: 0");
    m_lastVisitLabel->setText("Last Visit: -");
    
    emit newCustomerPhoneEntered(phone);
}

void CustomerPanel::onRedeemPointsClicked() {
    if (m_currentLoyaltyPoints < 100) return;

    // Determine best redemption
    int pointsToRedeem = 0;
    double discount = 0.0;

    if (m_currentLoyaltyPoints >= 500) {
        pointsToRedeem = 500;
        discount = 400.0;
    } else if (m_currentLoyaltyPoints >= 250) {
        pointsToRedeem = 250;
        discount = 150.0;
    } else if (m_currentLoyaltyPoints >= 100) {
        pointsToRedeem = 100;
        discount = 50.0;
    }

    auto result = QMessageBox::question(this, "Redeem Loyalty Points",
        QString("Redeem %1 points for ₹%2 discount?\n\nRemaining points: %3")
            .arg(pointsToRedeem).arg(discount, 0, 'f', 2).arg(m_currentLoyaltyPoints - pointsToRedeem),
        QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        emit redeemPointsRequested(pointsToRedeem);
        m_redeemBtn->hide();
    }
}

void CustomerPanel::clear() {
    m_phoneInput->clear();
    m_nameLabel->setText("-");
    m_tierLabel->setText("-");
    m_tierLabel->setStyleSheet("background-color: #374151; padding: 2px 8px; border-radius: 8px; font-size: 11px;");
    m_pointsLabel->setText("Pts: 0");
    m_totalSpentLabel->setText("Spent: ₹0");
    m_totalOrdersLabel->setText("Orders: 0");
    m_lastVisitLabel->setText("Last Visit: -");
    m_welcomeLabel->hide();
    m_redeemBtn->hide();
    m_currentCustomerId = -1;
    m_currentLoyaltyPoints = 0;
}

void CustomerPanel::setCustomerDetails(const QString& name, const QString& phone, int loyaltyPoints, const QString& tier,
                                       double totalSpent, int totalOrders, const QString& lastVisit) {
    m_nameLabel->setText(name);
    m_pointsLabel->setText(QString("Pts: %1").arg(loyaltyPoints));
    m_currentLoyaltyPoints = loyaltyPoints;

    // Style tier badge
    QString tierUpper = tier.toUpper();
    QString tierColor = "#374151";
    if (tier == "platinum") tierColor = "#8B5CF6";
    else if (tier == "gold") tierColor = "#F59E0B";
    else if (tier == "silver") tierColor = "#9CA3AF";
    else tierColor = "#374151";

    m_tierLabel->setText(tierUpper);
    m_tierLabel->setStyleSheet(QString("background-color: %1; padding: 2px 8px; border-radius: 8px; font-size: 11px; color: white; font-weight: bold;").arg(tierColor));

    m_totalSpentLabel->setText(QString("Spent: ₹%1").arg(totalSpent, 0, 'f', 2));
    m_totalOrdersLabel->setText(QString("Orders: %1").arg(totalOrders));
    m_lastVisitLabel->setText(lastVisit.isEmpty() ? "Last Visit: -" : QString("Last Visit: %1").arg(lastVisit));
}

} // namespace RetailMS

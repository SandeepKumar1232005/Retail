#include "CheckoutSuccessDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QIcon>
#include <QStyle>
#include <QApplication>

namespace RetailMS {

CheckoutSuccessDialog::CheckoutSuccessDialog(const Invoice& invoice, const QString& paymentModeStr, QWidget* parent)
    : QDialog(parent), m_invoice(invoice), m_paymentModeStr(paymentModeStr) {
    
    this->setWindowTitle("Checkout Complete");
    this->setFixedSize(400, 350);
    this->setStyleSheet(
        "QDialog { background-color: #1A1D24; }"
        "QLabel { color: #FFFFFF; font-family: 'Inter', sans-serif; }"
        "QPushButton { font-family: 'Inter', sans-serif; font-weight: 600; border-radius: 6px; padding: 10px; }"
    );
    
    setupUi();
}

void CheckoutSuccessDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // Success Icon and Title
    QLabel* iconLabel = new QLabel(this);
    iconLabel->setText("✅");
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 48px;");
    
    QLabel* titleLabel = new QLabel("Payment Successful", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #4CAF50;");
    
    mainLayout->addWidget(iconLabel);
    mainLayout->addWidget(titleLabel);

    // Separator
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #2A2D35;");
    mainLayout->addWidget(line);

    // Summary Info
    QGridLayout* summaryLayout = new QGridLayout();
    summaryLayout->setSpacing(10);
    
    QLabel* itemsLbl = new QLabel("Items:", this);
    itemsLbl->setStyleSheet("color: #A9B1BC;");
    QLabel* itemsVal = new QLabel(QString::number(m_invoice.items.size()), this);
    itemsVal->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    QLabel* pmodeLbl = new QLabel("Payment Mode:", this);
    pmodeLbl->setStyleSheet("color: #A9B1BC;");
    QLabel* pmodeVal = new QLabel(m_paymentModeStr, this);
    pmodeVal->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    QLabel* totalLbl = new QLabel("Grand Total:", this);
    totalLbl->setStyleSheet("color: #A9B1BC; font-size: 16px; font-weight: bold;");
    QLabel* totalVal = new QLabel(QString("₹%1").arg(m_invoice.grandTotal, 0, 'f', 2), this);
    totalVal->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    totalVal->setStyleSheet("color: #4CAF50; font-size: 18px; font-weight: bold;");
    
    summaryLayout->addWidget(itemsLbl, 0, 0);
    summaryLayout->addWidget(itemsVal, 0, 1);
    summaryLayout->addWidget(pmodeLbl, 1, 0);
    summaryLayout->addWidget(pmodeVal, 1, 1);
    summaryLayout->addWidget(totalLbl, 2, 0);
    summaryLayout->addWidget(totalVal, 2, 1);
    
    mainLayout->addLayout(summaryLayout);
    mainLayout->addStretch();

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(15);
    
    QPushButton* printBtn = new QPushButton("🖨️ Print Bill", this);
    printBtn->setCursor(Qt::PointingHandCursor);
    printBtn->setStyleSheet("background-color: #2A2D35; color: white; border: 1px solid #3A3D45;");
    
    QPushButton* newTransBtn = new QPushButton("New Transaction", this);
    newTransBtn->setCursor(Qt::PointingHandCursor);
    newTransBtn->setStyleSheet("background-color: #4F46E5; color: white; border: none;");
    
    btnLayout->addWidget(printBtn);
    btnLayout->addWidget(newTransBtn);
    mainLayout->addLayout(btnLayout);

    // Connections
    connect(printBtn, &QPushButton::clicked, this, [this]() {
        emit printRequested();
    });
    
    connect(newTransBtn, &QPushButton::clicked, this, [this]() {
        emit newTransactionRequested();
        this->accept();
    });
}

} // namespace RetailMS

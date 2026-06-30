#include "PaymentActionPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGridLayout>
#include <QMenu>
#include <QAction>

namespace RetailMS {

PaymentActionPanel::PaymentActionPanel(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void PaymentActionPanel::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(15);

    QString cardStyle = "QFrame { background-color: #1E2025; border-radius: 8px; padding: 15px; }";

    // --- CARD 1: Billing Summary ---
    QFrame* summaryCard = new QFrame(this);
    summaryCard->setStyleSheet(cardStyle);
    QGridLayout* summaryLayout = new QGridLayout(summaryCard);
    
    QLabel* subTitle = new QLabel("Subtotal:", summaryCard);
    subTitle->setStyleSheet("color: #A9B1BC; font-size: 14px;");
    m_subtotalLabel = new QLabel("0.00", summaryCard);
    m_subtotalLabel->setStyleSheet("color: #FFF; font-size: 14px; font-weight: bold;");
    m_subtotalLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    QLabel* discTitle = new QLabel("Discount:", summaryCard);
    discTitle->setStyleSheet("color: #A9B1BC; font-size: 14px;");
    m_discountLabel = new QLabel("0.00", summaryCard);
    m_discountLabel->setStyleSheet("color: #F44336; font-size: 14px; font-weight: bold;");
    m_discountLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    QLabel* gstTitle = new QLabel("GST:", summaryCard);
    gstTitle->setStyleSheet("color: #A9B1BC; font-size: 14px;");
    m_gstLabel = new QLabel("0.00", summaryCard);
    m_gstLabel->setStyleSheet("color: #FFF; font-size: 14px; font-weight: bold;");
    m_gstLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    QLabel* netTitle = new QLabel("NET AMOUNT", summaryCard);
    netTitle->setStyleSheet("color: #A9B1BC; font-size: 16px; font-weight: bold; margin-top: 10px;");
    m_netAmountLabel = new QLabel("0.00", summaryCard);
    m_netAmountLabel->setStyleSheet("color: #4CAF50; font-size: 28px; font-weight: bold; margin-top: 10px;");
    m_netAmountLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    summaryLayout->addWidget(subTitle, 0, 0);
    summaryLayout->addWidget(m_subtotalLabel, 0, 1);
    summaryLayout->addWidget(discTitle, 1, 0);
    summaryLayout->addWidget(m_discountLabel, 1, 1);
    summaryLayout->addWidget(gstTitle, 2, 0);
    summaryLayout->addWidget(m_gstLabel, 2, 1);
    
    QFrame* hline = new QFrame(summaryCard);
    hline->setFrameShape(QFrame::HLine);
    hline->setStyleSheet("background-color: #2A2D35; margin-top: 5px; margin-bottom: 5px;");
    summaryLayout->addWidget(hline, 3, 0, 1, 2);
    
    summaryLayout->addWidget(netTitle, 4, 0);
    summaryLayout->addWidget(m_netAmountLabel, 4, 1);
    
    mainLayout->addWidget(summaryCard);

    // --- CARD 2: Payment Modes ---
    QFrame* paymentCard = new QFrame(this);
    paymentCard->setStyleSheet(cardStyle);
    QVBoxLayout* paymentLayout = new QVBoxLayout(paymentCard);
    
    QLabel* payTitle = new QLabel("Payment Mode", paymentCard);
    payTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #FFFFFF; margin-bottom: 5px;");
    paymentLayout->addWidget(payTitle);
    
    m_paymentModeGroup = new QButtonGroup(this);
    
    QString btnStyle = 
        "QPushButton { background-color: #2A2D35; color: #FFF; border: 2px solid transparent; border-radius: 6px; padding: 12px 5px; font-size: 13px; font-weight: bold; }"
        "QPushButton:checked { border: 2px solid #4CAF50; background-color: #1a2a1a; color: #4CAF50; }";
        
    QGridLayout* payGrid = new QGridLayout();
    payGrid->setSpacing(10);
    
    QPushButton* btnCash = new QPushButton("💵 Cash", paymentCard);
    btnCash->setCheckable(true);
    btnCash->setChecked(true);
    btnCash->setStyleSheet(btnStyle);
    btnCash->setCursor(Qt::PointingHandCursor);
    m_paymentModeGroup->addButton(btnCash, static_cast<int>(Invoice::PaymentMode::Cash));
    
    QPushButton* btnCard = new QPushButton("💳 Card", paymentCard);
    btnCard->setCheckable(true);
    btnCard->setStyleSheet(btnStyle);
    btnCard->setCursor(Qt::PointingHandCursor);
    m_paymentModeGroup->addButton(btnCard, static_cast<int>(Invoice::PaymentMode::Card));
    
    QPushButton* btnUpi = new QPushButton("📱 UPI", paymentCard);
    btnUpi->setCheckable(true);
    btnUpi->setStyleSheet(btnStyle);
    btnUpi->setCursor(Qt::PointingHandCursor);
    m_paymentModeGroup->addButton(btnUpi, static_cast<int>(Invoice::PaymentMode::UPI));
    
    QPushButton* btnWallet = new QPushButton("💼 Wallet", paymentCard);
    btnWallet->setCheckable(true);
    btnWallet->setStyleSheet(btnStyle);
    btnWallet->setCursor(Qt::PointingHandCursor);
    m_paymentModeGroup->addButton(btnWallet, static_cast<int>(Invoice::PaymentMode::Wallet));
    
    payGrid->addWidget(btnCash, 0, 0);
    payGrid->addWidget(btnCard, 0, 1);
    payGrid->addWidget(btnUpi, 1, 0);
    payGrid->addWidget(btnWallet, 1, 1);
    
    paymentLayout->addLayout(payGrid);
    
    mainLayout->addWidget(paymentCard);
    
    mainLayout->addStretch();

    // --- CARD 3: Actions ---
    QFrame* actionCard = new QFrame(this);
    actionCard->setStyleSheet(cardStyle);
    QVBoxLayout* actionLayout = new QVBoxLayout(actionCard);
    
    m_generateBtn = new QPushButton("Generate Bill (F10)", actionCard);
    m_generateBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 6px; padding: 15px; font-weight: bold; font-size: 16px; }"
                                 "QPushButton:hover { background-color: #45a049; }");
    m_generateBtn->setCursor(Qt::PointingHandCursor);
    
    QString secBtnStyle = "QPushButton, QToolButton { background-color: #2A2D35; color: #FFF; border: none; border-radius: 6px; padding: 12px; font-weight: bold; font-size: 13px; }"
                          "QPushButton:hover, QToolButton:hover { background-color: #374151; }";
                          
    QHBoxLayout* secActionsLayout = new QHBoxLayout();
    
    m_printBtn = new QPushButton("🖨️ Print", actionCard);
    m_printBtn->setStyleSheet(secBtnStyle);
    m_printBtn->setCursor(Qt::PointingHandCursor);
    
    m_clearBtn = new QPushButton("🗑️ Clear", actionCard);
    m_clearBtn->setStyleSheet(secBtnStyle);
    m_clearBtn->setCursor(Qt::PointingHandCursor);
    
    m_advancedMenuBtn = new QToolButton(actionCard);
    m_advancedMenuBtn->setText("More...");
    m_advancedMenuBtn->setPopupMode(QToolButton::InstantPopup);
    m_advancedMenuBtn->setStyleSheet(secBtnStyle + "QToolButton::menu-indicator { image: none; }");
    m_advancedMenuBtn->setCursor(Qt::PointingHandCursor);
    
    QMenu* advMenu = new QMenu(this);
    advMenu->setStyleSheet("QMenu { background-color: #2A2D35; color: #FFF; border: 1px solid #374151; border-radius: 4px; }"
                           "QMenu::item { padding: 8px 25px; }"
                           "QMenu::item:selected { background-color: #4CAF50; }");
                           
    QAction* holdAct = advMenu->addAction("⏸️ Hold Bill");
    QAction* resumeAct = advMenu->addAction("▶️ Resume Bill");
    QAction* cancelAct = advMenu->addAction("❌ Cancel Bill");
    advMenu->addSeparator();
    QAction* previewAct = advMenu->addAction("👁️ Preview Invoice");
    advMenu->addAction("✉️ Email Invoice");
    advMenu->addAction("💬 WhatsApp Invoice");
    
    m_advancedMenuBtn->setMenu(advMenu);
    
    secActionsLayout->addWidget(m_printBtn);
    secActionsLayout->addWidget(m_clearBtn);
    secActionsLayout->addWidget(m_advancedMenuBtn);
    
    actionLayout->addWidget(m_generateBtn);
    actionLayout->addLayout(secActionsLayout);
    
    mainLayout->addWidget(actionCard);
    
    // Connections
    connect(m_generateBtn, &QPushButton::clicked, this, [this]() {
        Invoice::PaymentMode mode = static_cast<Invoice::PaymentMode>(m_paymentModeGroup->checkedId());
        emit generateInvoiceRequested(mode);
    });
    connect(m_printBtn, &QPushButton::clicked, this, &PaymentActionPanel::printRequested);
    connect(m_clearBtn, &QPushButton::clicked, this, &PaymentActionPanel::clearCartRequested);
    connect(holdAct, &QAction::triggered, this, &PaymentActionPanel::holdBillRequested);
    connect(resumeAct, &QAction::triggered, this, &PaymentActionPanel::resumeBillRequested);
    connect(cancelAct, &QAction::triggered, this, &PaymentActionPanel::cancelBillRequested);
    connect(previewAct, &QAction::triggered, this, &PaymentActionPanel::previewRequested);
}

void PaymentActionPanel::refreshTotals(const Invoice& invoice) {
    m_subtotalLabel->setText(QString::number(invoice.subtotal, 'f', 2));
    m_discountLabel->setText(QString::number(invoice.discountAmt + invoice.couponDiscount, 'f', 2));
    m_gstLabel->setText(QString::number(invoice.cgstAmt + invoice.sgstAmt, 'f', 2));
    m_netAmountLabel->setText(QString::number(invoice.grandTotal, 'f', 2));
}

} // namespace RetailMS

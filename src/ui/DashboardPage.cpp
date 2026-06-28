#include "DashboardPage.h"

namespace RetailMS {

DashboardPage::DashboardPage(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void DashboardPage::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QLabel* title = new QLabel("Dashboard Overview", this);
    title->setStyleSheet("font-size: 28px; font-weight: bold; margin-bottom: 20px;");
    mainLayout->addWidget(title);
    
    QHBoxLayout* statsLayout = new QHBoxLayout();
    
    auto createStatCard = [this](const QString& title, QLabel*& valueLabel) -> QWidget* {
        QWidget* card = new QWidget(this);
        card->setStyleSheet("background-color: #1E1E1E; border-radius: 8px; padding: 20px;");
        QVBoxLayout* l = new QVBoxLayout(card);
        QLabel* t = new QLabel(title, card);
        t->setStyleSheet("color: #B0B0B0; font-size: 16px;");
        valueLabel = new QLabel("0", card);
        valueLabel->setStyleSheet("color: #4CAF50; font-size: 32px; font-weight: bold;");
        l->addWidget(t);
        l->addWidget(valueLabel);
        return card;
    };
    
    statsLayout->addWidget(createStatCard("Today's Sales", m_todaySalesLabel));
    statsLayout->addWidget(createStatCard("Total Orders", m_totalOrdersLabel));
    statsLayout->addWidget(createStatCard("Low Stock Items", m_lowStockLabel));
    
    mainLayout->addLayout(statsLayout);
    mainLayout->addStretch();
}

}

#include "SidebarWidget.h"
#include <QLabel>

namespace RetailMS {

SidebarWidget::SidebarWidget(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void SidebarWidget::setupUi() {
    this->setFixedWidth(250);
    this->setObjectName("sidebar");

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 20, 0, 20);
    m_layout->setSpacing(10);

    QLabel* logo = new QLabel("RetailMS", this);
    logo->setAlignment(Qt::AlignCenter);
    logo->setObjectName("sidebarLogo");
    m_layout->addWidget(logo);
    m_layout->addSpacing(30);

    // Nav buttons
    m_layout->addWidget(createNavButton("Dashboard", "", 0));
    m_layout->addWidget(createNavButton("Billing", "", 1));
    m_layout->addWidget(createNavButton("Products", "", 2));
    m_layout->addWidget(createNavButton("Customers", "", 3));
    m_layout->addWidget(createNavButton("Inventory", "", 4));

    m_layout->addStretch(); // push everything up
}

QPushButton* SidebarWidget::createNavButton(const QString& text, const QString& iconPath, int pageIndex) {
    QPushButton* btn = new QPushButton(text, this);
    btn->setObjectName("sidebarButton");
    btn->setCheckable(true);
    btn->setAutoExclusive(true);
    
    connect(btn, &QPushButton::clicked, this, [this, pageIndex]() {
        emit pageSelected(pageIndex);
    });
    
    if (pageIndex == 0) btn->setChecked(true);
    return btn;
}

} // namespace RetailMS

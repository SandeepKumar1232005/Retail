#include "SidebarWidget.h"
#include "../services/SessionManager.h"
#include <QLabel>
#include <QEvent>

namespace RetailMS {

SidebarWidget::SidebarWidget(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void SidebarWidget::setupUi() {
    this->setFixedWidth(260); // Expanded width by default
    this->setObjectName("sidebar");

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 20);
    m_layout->setSpacing(5);

    m_logoLabel = new QLabel("RetailMS", this);
    m_logoLabel->setObjectName("sidebarLogo");
    m_logoLabel->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_logoLabel);
    m_layout->addSpacing(20);

    bool isAdmin = SessionManager::instance().isLoggedIn() && SessionManager::instance().currentUser().isAdmin();

    m_navButtons.append(createNavButton("Dashboard", "📊", 0));
    m_navButtons.append(createNavButton("Billing", "💳", 1));

    if (isAdmin) {
        m_navButtons.append(createNavButton("Products", "📦", 2));
        m_navButtons.append(createNavButton("Inventory", "🏢", 3));
        m_navButtons.append(createNavButton("Categories", "📁", 4));
        m_navButtons.append(createNavButton("Sales History", "📝", 5));
        m_navButtons.append(createNavButton("Reports", "📈", 6));
        m_navButtons.append(createNavButton("Staff Mgt", "👨‍💼", 7));
        m_navButtons.append(createNavButton("Customers", "👥", 8));
    } else {
        m_navButtons.append(createNavButton("My Bills", "📝", 9));
        m_navButtons.append(createNavButton("Products", "📦", 2));
        m_navButtons.append(createNavButton("Profile", "👤", 10));
    }

    m_layout->addStretch();
    
    // Animation setup using the sidebarWidth property
    m_animation = new QPropertyAnimation(this, "sidebarWidth");
    m_animation->setDuration(150);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);
}

QString SidebarWidget::getPageName(int index) const {
    for (auto btn : m_navButtons) {
        // The slot captures pageIndex by value, but we didn't store it on the button.
        // Let's rely on property if we stored it, or just match the index to the list.
    }
    // Static map
    QStringList pages = {
        "Dashboard", "Billing", "Products", "Inventory", "Categories", 
        "Sales History", "Reports", "Staff Management", "Customers", 
        "My Bills", "Profile", "Access Denied"
    };
    if (index >= 0 && index < pages.size()) return pages[index];
    return "Unknown Page";
}

QPushButton* SidebarWidget::createNavButton(const QString& text, const QString& icon, int pageIndex) {
    QPushButton* btn = new QPushButton(this);
    btn->setText(QString("%1    %2").arg(icon, text));
    btn->setObjectName("sidebarButton");
    btn->setCheckable(true);
    btn->setAutoExclusive(true);
    btn->setProperty("fullText", text);
    btn->setProperty("iconText", icon);
    
    btn->setFixedHeight(50);
    btn->setCursor(Qt::PointingHandCursor);
    
    connect(btn, &QPushButton::clicked, this, [this, pageIndex]() {
        emit pageSelected(pageIndex);
    });
    
    if (pageIndex == 0) btn->setChecked(true);
    
    m_layout->addWidget(btn);
    return btn;
}

void SidebarWidget::enterEvent(QEnterEvent* event) {
    QWidget::enterEvent(event);
    if (m_isCollapsed) {
        toggleCollapse(false);
    }
}

void SidebarWidget::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
    if (!m_isCollapsed) {
        toggleCollapse(true);
    }
}

void SidebarWidget::toggleCollapse(bool collapse) {
    m_isCollapsed = collapse;
    
    m_animation->stop();
    m_animation->setStartValue(this->width());
    
    if (collapse) {
        m_animation->setEndValue(80); // Collapsed width
        m_logoLabel->setText("R");
        for (auto btn : m_navButtons) {
            btn->setText(btn->property("iconText").toString());
            btn->setStyleSheet("text-align: center; padding-left: 0;");
        }
    } else {
        m_animation->setEndValue(260); // Expanded width
        m_logoLabel->setText("RetailMS");
        for (auto btn : m_navButtons) {
            btn->setText(QString("%1    %2").arg(btn->property("iconText").toString(), btn->property("fullText").toString()));
            btn->setStyleSheet(""); // Revert to QSS styling
        }
    }
    
    m_animation->start();
}

} // namespace RetailMS

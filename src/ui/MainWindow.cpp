#include "MainWindow.h"
#include "SidebarWidget.h"
#include "DashboardPage.h"
#include "BillingPage.h"
#include "ProductPage.h"
#include "../controllers/BillingController.h"
#include "../controllers/ProductController.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTimer>
#include <QDateTime>

namespace RetailMS {

MainWindow::MainWindow(std::shared_ptr<BillingController> billingController,
                       std::shared_ptr<ProductController> productController,
                       QWidget* parent)
    : QMainWindow(parent),
      m_billingController(std::move(billingController)),
      m_productController(std::move(productController)) {
    setupUi();
    setupConnections();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    this->setWindowTitle("Retail Management System - Enterprise");
    this->resize(1440, 900);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar
    m_sidebar = new SidebarWidget(centralWidget);
    mainLayout->addWidget(m_sidebar);

    // Right Content Area (Vertical)
    QWidget* rightAreaWidget = new QWidget(centralWidget);
    QVBoxLayout* rightAreaLayout = new QVBoxLayout(rightAreaWidget);
    rightAreaLayout->setContentsMargins(0, 0, 0, 0);
    rightAreaLayout->setSpacing(0);
    
    setupTopNavBar(rightAreaWidget, rightAreaLayout);

    // Stacked Widget for Pages
    m_contentArea = new QStackedWidget(rightAreaWidget);
    m_contentArea->setObjectName("contentArea");
    rightAreaLayout->addWidget(m_contentArea, 1);
    
    mainLayout->addWidget(rightAreaWidget, 1);
    
    // Add Real Pages
    m_dashboardPage = new DashboardPage(m_contentArea);
    m_contentArea->addWidget(m_dashboardPage);
    m_billingPage = new BillingPage(m_billingController, m_contentArea);
    m_contentArea->addWidget(m_billingPage);
    m_contentArea->addWidget(new ProductPage(m_productController, m_contentArea));
    
    // Add Dummy Pages for the rest
    auto addPage = [this](const QString& title) {
        QLabel* label = new QLabel(title, m_contentArea);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 24px; color: #555;");
        m_contentArea->addWidget(label);
    };

    addPage("Customers Page");
    addPage("Inventory Page");
    addPage("Reports Page");
    addPage("Settings Page");
}

void MainWindow::setupTopNavBar(QWidget* parent, QVBoxLayout* contentLayout) {
    QWidget* topNavBar = new QWidget(parent);
    topNavBar->setObjectName("topNavBar");
    topNavBar->setFixedHeight(64);
    
    QHBoxLayout* navLayout = new QHBoxLayout(topNavBar);
    navLayout->setContentsMargins(20, 0, 20, 0);
    navLayout->setSpacing(16);
    
    // Breadcrumb
    m_breadcrumbLabel = new QLabel("Dashboard", topNavBar);
    m_breadcrumbLabel->setObjectName("breadcrumbLabel");
    navLayout->addWidget(m_breadcrumbLabel);
    
    navLayout->addStretch();
    
    // Global Search
    m_searchBox = new QLineEdit(topNavBar);
    m_searchBox->setPlaceholderText("Search (Ctrl+K)...");
    m_searchBox->setFixedWidth(300);
    navLayout->addWidget(m_searchBox);
    
    navLayout->addStretch();
    
    // Date Time
    m_dateTimeLabel = new QLabel(topNavBar);
    m_dateTimeLabel->setObjectName("dateTimeLabel");
    navLayout->addWidget(m_dateTimeLabel);
    
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        m_dateTimeLabel->setText(QDateTime::currentDateTime().toString("MMM dd, yyyy  |  hh:mm ap"));
    });
    timer->start(1000);
    m_dateTimeLabel->setText(QDateTime::currentDateTime().toString("MMM dd, yyyy  |  hh:mm ap"));
    
    // Profile / Notifications Placeholder
    QPushButton* notifBtn = new QPushButton("🔔", topNavBar);
    notifBtn->setFixedSize(40, 40);
    notifBtn->setStyleSheet("background: transparent; border: none; font-size: 18px;");
    navLayout->addWidget(notifBtn);
    
    QPushButton* profileBtn = new QPushButton("JD", topNavBar);
    profileBtn->setFixedSize(40, 40);
    profileBtn->setStyleSheet("background: #4CAF50; border-radius: 20px; color: white; font-weight: bold; border: none;");
    navLayout->addWidget(profileBtn);

    contentLayout->addWidget(topNavBar);
}

void MainWindow::setupConnections() {
    connect(m_sidebar, &SidebarWidget::pageSelected, this, [this](int index) {
        m_contentArea->setCurrentIndex(index);
        
        // Update breadcrumb
        QStringList pages = {"Dashboard", "Billing", "Products", "Customers", "Inventory", "Reports", "Settings"};
        if (index >= 0 && index < pages.size()) {
            m_breadcrumbLabel->setText(pages[index]);
        }
    });

    connect(m_billingController.get(), &BillingController::checkoutComplete, m_dashboardPage, &DashboardPage::refreshData);
    connect(m_productController.get(), &ProductController::productListChanged, m_dashboardPage, &DashboardPage::refreshData);
    connect(m_productController.get(), &ProductController::productListChanged, m_billingPage, &BillingPage::reloadProductGrid);
}

} // namespace RetailMS

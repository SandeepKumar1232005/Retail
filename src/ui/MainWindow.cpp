#include "MainWindow.h"
#include "SidebarWidget.h"
#include "DashboardPage.h"
#include "BillingPage.h"
#include "ProductPage.h"
#include "MyBillsPage.h"
#include "InventoryPage.h"
#include "SalesHistoryPage.h"
#include "ReportsPage.h"
#include "employees/StaffManagementPage.h"
#include "CustomersPage.h"
#include "../controllers/BillingController.h"
#include "../controllers/ProductController.h"
#include "../services/SessionManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTimer>
#include <QDateTime>
#include <QResizeEvent>

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

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    if (m_sidebar && centralWidget()) {
        m_sidebar->setGeometry(0, 0, m_sidebar->width(), centralWidget()->height());
    }
}

void MainWindow::setupUi() {
    this->setWindowTitle("Retail Management System - Enterprise");
    this->resize(1440, 900);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar Spacer (Occupies fixed width so content doesn't shift)
    QWidget* sidebarSpacer = new QWidget(centralWidget);
    sidebarSpacer->setFixedWidth(75);
    mainLayout->addWidget(sidebarSpacer);

    // Floating Sidebar
    m_sidebar = new SidebarWidget(centralWidget);

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

    m_inventoryPage = new InventoryPage(m_billingController, m_productController, m_contentArea);
    m_contentArea->addWidget(m_inventoryPage); // 3

    addPage("Categories Page"); // 4

    m_salesHistoryPage = new SalesHistoryPage(m_billingController, m_contentArea);
    m_contentArea->addWidget(m_salesHistoryPage); // 5

    m_reportsPage = new ReportsPage(m_billingController, m_contentArea);
    m_contentArea->addWidget(m_reportsPage); // 6

    m_staffManagementPage = new StaffManagementPage(m_billingController, m_contentArea);
    m_contentArea->addWidget(m_staffManagementPage); // 7
    
    m_customersPage = new CustomersPage(m_billingController->customerService(), m_billingController, m_contentArea);
    m_contentArea->addWidget(m_customersPage); // 8
    
    // My Bills Page (9)
    m_myBillsPage = new MyBillsPage(m_billingController, m_contentArea);
    m_contentArea->addWidget(m_myBillsPage);
    
    addPage("Profile Page"); // 10
    
    // Access Denied Page (11)
    QLabel* accessDenied = new QLabel("403 - Access Denied", m_contentArea);
    accessDenied->setAlignment(Qt::AlignCenter);
    accessDenied->setStyleSheet("font-size: 32px; color: #EF4444; font-weight: bold;");
    m_contentArea->addWidget(accessDenied); // 11
    
    // Ensure sidebar is layered strictly above all right-side content
    m_sidebar->raise();
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
        bool isAdmin = SessionManager::instance().isLoggedIn() && SessionManager::instance().currentUser().isAdmin();
        
        // RBAC routing
        if (!isAdmin) {
            // Staff allowed: 0 (Dashboard), 1 (Billing), 2 (Products), 9 (My Bills), 10 (Profile)
            if (index != 0 && index != 1 && index != 2 && index != 9 && index != 10) {
                index = 11; // Access Denied
            }
        }
        
        m_contentArea->setCurrentIndex(index);
        m_breadcrumbLabel->setText(m_sidebar->getPageName(index));
    });

    connect(m_billingController.get(), &BillingController::checkoutComplete, m_dashboardPage, &DashboardPage::refreshData);
    connect(m_productController.get(), &ProductController::productListChanged, m_dashboardPage, &DashboardPage::refreshData);
    connect(m_productController.get(), &ProductController::productListChanged, m_billingPage, [this]() {
        m_billingPage->loadProducts();
    });
}

} // namespace RetailMS

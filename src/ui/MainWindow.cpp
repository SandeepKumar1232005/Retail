#include "MainWindow.h"
#include "SidebarWidget.h"
#include "DashboardPage.h"
#include "BillingPage.h"
#include "ProductPage.h"
#include "../controllers/BillingController.h"
#include "../controllers/ProductController.h"
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>

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
    this->setWindowTitle("Retail Management System");
    this->resize(1280, 720);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_sidebar = new SidebarWidget(this);
    m_contentArea = new QStackedWidget(this);
    m_contentArea->setObjectName("contentArea");

    mainLayout->addWidget(m_sidebar);
    mainLayout->addWidget(m_contentArea, 1);
    
    // Add Real Pages
    m_contentArea->addWidget(new DashboardPage(this));
    m_contentArea->addWidget(new BillingPage(m_billingController, this));
    m_contentArea->addWidget(new ProductPage(m_productController, this));
    
    // Add Dummy Pages for the rest
    auto addPage = [this](const QString& title) {
        QLabel* label = new QLabel(title, this);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 24px; color: #555;");
        m_contentArea->addWidget(label);
    };

    addPage("Customers Page");
    addPage("Inventory Page");
}

void MainWindow::setupConnections() {
    connect(m_sidebar, &SidebarWidget::pageSelected, m_contentArea, &QStackedWidget::setCurrentIndex);
}

} // namespace RetailMS

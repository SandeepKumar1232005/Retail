#include <QApplication>
#include <QDebug>
#include <QFile>
#include <memory>
#include "database/DatabaseManager.h"
#include "ui/MainWindow.h"

// Repositories
#include "repository/InvoiceRepository.h"
#include "repository/ProductRepository.h"
#include "repository/InventoryLogRepository.h"
#include "repository/CustomerRepository.h"
#include "repository/CouponRepository.h"

// Services
#include "services/BillingService.h"
#include "services/ProductService.h"
#include "services/InventoryService.h"
#include "services/CustomerService.h"
#include "services/CouponService.h"

// Controllers
#include "controllers/BillingController.h"
#include "controllers/ProductController.h"

using namespace RetailMS;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Load Stylesheet
    QFile file("resources/style.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        app.setStyleSheet(styleSheet);
    } else {
        qWarning() << "Could not open resources/style.qss";
    }

    qDebug() << "Starting Retail Management System...";

    // Initialize Database
    auto dbManager = std::shared_ptr<DatabaseManager>(&DatabaseManager::instance(), [](DatabaseManager*){});
    if (!dbManager->connect("retail_ms.db")) {
        qCritical() << "Failed to connect to database!";
        return 1;
    }
    dbManager->runMigrations("src/database/migrations");
    
    // Setup Repositories
    auto invoiceRepo = std::make_shared<InvoiceRepository>(dbManager);
    auto productRepo = std::make_shared<ProductRepository>(dbManager);
    auto inventoryLogRepo = std::make_shared<InventoryLogRepository>(dbManager);
    auto customerRepo = std::make_shared<CustomerRepository>(dbManager);
    auto couponRepo = std::make_shared<CouponRepository>(dbManager);
    
    // Setup Services
    auto productService = std::make_shared<ProductService>(productRepo);
    auto inventoryService = std::make_shared<InventoryService>(inventoryLogRepo, productRepo);
    auto customerService = std::make_shared<CustomerService>(customerRepo);
    auto couponService = std::make_shared<CouponService>(couponRepo);
    auto billingService = std::make_shared<BillingService>(
        invoiceRepo, productService, inventoryService, customerService, couponService
    );
    
    // Setup Controllers
    auto billingController = std::make_shared<BillingController>(billingService);
    auto productController = std::make_shared<ProductController>(productService);

    qDebug() << "Application started successfully.";
    
    // Setup UI
    MainWindow window(billingController, productController);
    window.show();

    return app.exec();
}

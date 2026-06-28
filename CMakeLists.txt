cmake_minimum_required(VERSION 3.25)

project(RetailManagementSystem VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt settings
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 COMPONENTS Core Gui Widgets Sql PrintSupport Charts Test REQUIRED)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

include(FetchContent)

# SQLite3
FetchContent_Declare(
    sqlite3
    URL https://sqlite.org/2023/sqlite-amalgamation-3430200.zip
)
FetchContent_GetProperties(sqlite3)
if(NOT sqlite3_POPULATED)
    FetchContent_Populate(sqlite3)
    add_library(sqlite3 STATIC ${sqlite3_SOURCE_DIR}/sqlite3.c)
    target_include_directories(sqlite3 PUBLIC ${sqlite3_SOURCE_DIR})
endif()

# ZXing-C++
FetchContent_Declare(
    ZXing
    GIT_REPOSITORY https://github.com/zxing-cpp/zxing-cpp.git
    GIT_TAG        v2.2.1
)
FetchContent_MakeAvailable(ZXing)

# Collect source files
file(GLOB_RECURSE SRC_FILES
    "src/*.cpp"
    "src/*.h"
    "src/*.ui"
)

# Remove main.cpp from SRC_FILES to create a library for testing if needed
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# Core Library
add_library(RetailCore STATIC ${SRC_FILES})
target_include_directories(RetailCore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_link_libraries(RetailCore PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::PrintSupport
    Qt6::Charts
    OpenSSL::SSL
    OpenSSL::Crypto
    sqlite3
    ZXing::ZXing
)

# Executable
add_executable(RetailManagementSystem src/main.cpp)
target_link_libraries(RetailManagementSystem PRIVATE RetailCore)

# Resources
# qt6_add_resources(RetailManagementSystem "resources/resources.qrc") # Un-comment when resources are added

# Tests
enable_testing()
add_subdirectory(tests)

target_sources(RetailMS PRIVATE 
    src/main.cpp
    src/controllers/BaseController.cpp
    src/controllers/BillingController.cpp
    src/controllers/ProductController.cpp
    src/database/DatabaseManager.cpp
    src/database/QueryBuilder.cpp
    src/database/Transaction.cpp
    src/database/migrations/MigrationRunner.cpp
    src/exceptions/AppException.cpp
    src/models/Category.cpp
    src/models/Coupon.cpp
    src/models/Customer.cpp
    src/models/Employee.cpp
    src/models/Expense.cpp
    src/models/InventoryLog.cpp
    src/models/Invoice.cpp
    src/models/InvoiceItem.cpp
    src/models/Notification.cpp
    src/models/Product.cpp
    src/models/PurchaseOrder.cpp
    src/models/Supplier.cpp
    src/models/User.cpp
    src/repository/CategoryRepository.cpp
    src/repository/CouponRepository.cpp
    src/repository/CustomerRepository.cpp
    src/repository/EmployeeRepository.cpp
    src/repository/ExpenseRepository.cpp
    src/repository/InventoryLogRepository.cpp
    src/repository/InvoiceRepository.cpp
    src/repository/NotificationRepository.cpp
    src/repository/ProductRepository.cpp
    src/repository/PurchaseOrderRepository.cpp
    src/repository/SupplierRepository.cpp
    src/repository/UserRepository.cpp
    src/services/AuthService.cpp
    src/services/BackupService.cpp
    src/services/BillingService.cpp
    src/services/CouponService.cpp
    src/services/CustomerService.cpp
    src/services/EmployeeService.cpp
    src/services/ExpenseService.cpp
    src/services/InventoryService.cpp
    src/services/NotificationService.cpp
    src/services/ProductService.cpp
    src/services/ReportService.cpp
    src/services/SupplierService.cpp
    src/ui/BillingPage.cpp
    src/ui/DashboardPage.cpp
    src/ui/MainWindow.cpp
    src/ui/ProductPage.cpp
    src/ui/SidebarWidget.cpp
    src/utils/CryptoUtils.cpp
    src/utils/DateTimeUtils.cpp
    src/utils/Logger.cpp
    src/utils/Validators.cpp
    src/controllers/BaseController.h
    src/controllers/BillingController.h
    src/controllers/ProductController.h
    src/database/DatabaseManager.h
    src/database/QueryBuilder.h
    src/database/Transaction.h
    src/database/migrations/MigrationRunner.h
    src/exceptions/AppException.h
    src/models/BaseModel.h
    src/models/Category.h
    src/models/Coupon.h
    src/models/Customer.h
    src/models/Employee.h
    src/models/Expense.h
    src/models/InventoryLog.h
    src/models/Invoice.h
    src/models/InvoiceItem.h
    src/models/Notification.h
    src/models/Product.h
    src/models/PurchaseOrder.h
    src/models/Supplier.h
    src/models/User.h
    src/repository/CategoryRepository.h
    src/repository/CouponRepository.h
    src/repository/CustomerRepository.h
    src/repository/EmployeeRepository.h
    src/repository/ExpenseRepository.h
    src/repository/InventoryLogRepository.h
    src/repository/InvoiceRepository.h
    src/repository/IRepository.h
    src/repository/NotificationRepository.h
    src/repository/ProductRepository.h
    src/repository/PurchaseOrderRepository.h
    src/repository/SupplierRepository.h
    src/repository/UserRepository.h
    src/services/AuthService.h
    src/services/BackupService.h
    src/services/BillingService.h
    src/services/CouponService.h
    src/services/CustomerService.h
    src/services/EmployeeService.h
    src/services/ExpenseService.h
    src/services/InventoryService.h
    src/services/NotificationService.h
    src/services/ProductService.h
    src/services/ReportService.h
    src/services/SessionManager.h
    src/services/SupplierService.h
    src/ui/BillingPage.h
    src/ui/DashboardPage.h
    src/ui/MainWindow.h
    src/ui/ProductPage.h
    src/ui/SidebarWidget.h
    src/utils/CryptoUtils.h
    src/utils/DateTimeUtils.h
    src/utils/Logger.h
    src/utils/Validators.h
)

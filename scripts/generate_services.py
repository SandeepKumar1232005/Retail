import os

services = [
    "ProductService",
    "BillingService",
    "InventoryService",
    "CustomerService",
    "SupplierService",
    "EmployeeService",
    "ReportService",
    "NotificationService",
    "BackupService",
    "CouponService",
    "ExpenseService"
]

svc_dir = "f:/PROJECT/RETAIL/src/services"

header_template = """#pragma once
#include <QObject>
#include <memory>
#include "../database/DatabaseManager.h"

namespace RetailMS {{

class {svc_name} : public QObject {{
    Q_OBJECT
public:
    explicit {svc_name}(std::shared_ptr<DatabaseManager> db, QObject* parent = nullptr) 
        : QObject(parent), m_db(db) {{}}

private:
    std::shared_ptr<DatabaseManager> m_db;
}};

}} // namespace RetailMS
"""

cpp_template = """#include "{svc_name}.h"

namespace RetailMS {{

// Implementation for {svc_name}

}} // namespace RetailMS
"""

for svc in services:
    if svc in ["AuthService"]: continue # Already done manually
    h_path = os.path.join(svc_dir, f"{svc}.h")
    cpp_path = os.path.join(svc_dir, f"{svc}.cpp")
    
    # Let's not overwrite if we plan to write fully later, just scaffold what's missing
    if not os.path.exists(h_path):
        with open(h_path, 'w') as f:
            f.write(header_template.format(svc_name=svc))
    if not os.path.exists(cpp_path):
        with open(cpp_path, 'w') as f:
            f.write(cpp_template.format(svc_name=svc))

print("Services generated successfully.")

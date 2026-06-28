#include "InventoryService.h"
#include "../repository/InventoryLogRepository.h"
#include "../repository/ProductRepository.h"
#include "../models/InventoryLog.h"
#include "../models/Product.h"
#include "SessionManager.h"
#include "../exceptions/AppException.h"
#include "../utils/Logger.h"

namespace RetailMS {

InventoryService::InventoryService(std::shared_ptr<InventoryLogRepository> logRepo,
                                   std::shared_ptr<ProductRepository> productRepo,
                                   QObject* parent)
    : QObject(parent), m_logRepo(std::move(logRepo)), m_productRepo(std::move(productRepo)) {}

bool InventoryService::adjustStock(int productId, double quantity, const QString& type, const QString& notes, int referenceId) {
    auto optProduct = m_productRepo->findById(productId);
    if (!optProduct) return false;
    
    Product p = optProduct.value();
    
    InventoryLog log;
    log.productId = productId;
    if (SessionManager::instance().isLoggedIn()) {
        log.userId = SessionManager::instance().currentUser().id;
    }
    log.type = type;
    log.quantity = quantity;
    log.beforeQty = p.stockQuantity;
    log.afterQty = p.stockQuantity + quantity;
    log.referenceId = referenceId;
    log.notes = notes;

    p.stockQuantity += quantity;
    
    if (m_productRepo->update(p)) {
        m_logRepo->save(log);
        return true;
    }
    
    return false;
}

}

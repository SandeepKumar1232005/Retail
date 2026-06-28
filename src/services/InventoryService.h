#pragma once
#include <QObject>
#include <memory>
#include <QString>

namespace RetailMS {

class InventoryLogRepository;
class ProductRepository;

class InventoryService : public QObject {
    Q_OBJECT
public:
    explicit InventoryService(std::shared_ptr<InventoryLogRepository> logRepo,
                              std::shared_ptr<ProductRepository> productRepo,
                              QObject* parent = nullptr);

    bool adjustStock(int productId, double quantity, const QString& type, const QString& notes = "", int referenceId = -1);

private:
    std::shared_ptr<InventoryLogRepository> m_logRepo;
    std::shared_ptr<ProductRepository> m_productRepo;
};

}

#pragma once
#include "BaseModel.h"
#include <QString>
#include <QDate>
#include <vector>

namespace RetailMS {

class PurchaseOrderItem {
public:
    int id{-1};
    int poId{-1};
    int productId{-1};
    double quantityOrdered{0.0};
    double quantityReceived{0.0};
    double costPrice{0.0};
    double total{0.0};
    
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& obj);
};

class PurchaseOrder : public BaseModel {
public:
    QString poNumber;
    int supplierId{-1};
    int userId{-1};
    QString status{"pending"};
    double totalAmount{0.0};
    QString notes;
    QDate expectedDate;
    QDate receivedDate;
    
    std::vector<PurchaseOrderItem> items;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
};

} // namespace RetailMS

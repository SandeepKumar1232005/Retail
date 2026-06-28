#pragma once
#include "BaseModel.h"
#include <QString>

namespace RetailMS {

class InventoryLog : public BaseModel {
public:
    int productId{-1};
    int userId{-1};
    QString type;
    double quantity{0.0};
    double beforeQty{0.0};
    double afterQty{0.0};
    int referenceId{-1};
    QString referenceNo;
    QString notes;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
};

} // namespace RetailMS

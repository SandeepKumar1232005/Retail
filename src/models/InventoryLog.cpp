#include "InventoryLog.h"
#include <QVariant>

namespace RetailMS {

QJsonObject InventoryLog::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["product_id"] = productId;
    obj["user_id"] = userId;
    obj["type"] = type;
    obj["quantity"] = quantity;
    obj["before_qty"] = beforeQty;
    obj["after_qty"] = afterQty;
    obj["reference_id"] = referenceId;
    obj["reference_no"] = referenceNo;
    obj["notes"] = notes;
    return obj;
}

void InventoryLog::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    productId = obj["product_id"].toInt(-1);
    userId = obj["user_id"].toInt(-1);
    type = obj["type"].toString();
    quantity = obj["quantity"].toDouble();
    beforeQty = obj["before_qty"].toDouble();
    afterQty = obj["after_qty"].toDouble();
    referenceId = obj["reference_id"].toInt(-1);
    referenceNo = obj["reference_no"].toString();
    notes = obj["notes"].toString();
}

} // namespace RetailMS

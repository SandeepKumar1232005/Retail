#include "PurchaseOrder.h"
#include <QJsonArray>
#include <QVariant>

namespace RetailMS {

QJsonObject PurchaseOrderItem::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["po_id"] = poId;
    obj["product_id"] = productId;
    obj["quantity_ordered"] = quantityOrdered;
    obj["quantity_received"] = quantityReceived;
    obj["cost_price"] = costPrice;
    obj["total"] = total;
    return obj;
}

void PurchaseOrderItem::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    poId = obj["po_id"].toInt(-1);
    productId = obj["product_id"].toInt(-1);
    quantityOrdered = obj["quantity_ordered"].toDouble();
    quantityReceived = obj["quantity_received"].toDouble();
    costPrice = obj["cost_price"].toDouble();
    total = obj["total"].toDouble();
}

QJsonObject PurchaseOrder::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["po_number"] = poNumber;
    obj["supplier_id"] = supplierId;
    obj["user_id"] = userId;
    obj["status"] = status;
    obj["total_amount"] = totalAmount;
    obj["notes"] = notes;
    if (expectedDate.isValid()) obj["expected_date"] = expectedDate.toString(Qt::ISODate);
    if (receivedDate.isValid()) obj["received_date"] = receivedDate.toString(Qt::ISODate);
    
    QJsonArray itemsArr;
    for (const auto& item : items) {
        itemsArr.append(item.toJson());
    }
    obj["items"] = itemsArr;
    return obj;
}

void PurchaseOrder::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    poNumber = obj["po_number"].toString();
    supplierId = obj["supplier_id"].toInt(-1);
    userId = obj["user_id"].toInt(-1);
    status = obj["status"].toString("pending");
    totalAmount = obj["total_amount"].toDouble();
    notes = obj["notes"].toString();
    if (obj.contains("expected_date")) expectedDate = QDate::fromString(obj["expected_date"].toString(), Qt::ISODate);
    if (obj.contains("received_date")) receivedDate = QDate::fromString(obj["received_date"].toString(), Qt::ISODate);
    
    items.clear();
    QJsonArray itemsArr = obj["items"].toArray();
    for (int i = 0; i < itemsArr.size(); ++i) {
        PurchaseOrderItem item;
        item.fromJson(itemsArr[i].toObject());
        items.push_back(item);
    }
}

} // namespace RetailMS

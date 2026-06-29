#include "Product.h"
#include <QVariant>

namespace RetailMS {

bool Product::isExpired() const {
    if (!expiryDate.isValid()) return false;
    return QDate::currentDate() > expiryDate;
}

bool Product::isExpiringSoon(int daysAhead) const {
    if (!expiryDate.isValid()) return false;
    return QDate::currentDate().daysTo(expiryDate) <= daysAhead && !isExpired();
}

double Product::effectivePrice(double qty) const {
    double base = sellingPrice * qty;
    return base - (base * discountPct / 100.0);
}

double Product::profitMargin() const {
    if (costPrice <= 0) return 100.0;
    return ((sellingPrice - costPrice) / costPrice) * 100.0;
}

QJsonObject Product::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["sku"] = sku;
    obj["barcode"] = barcode;
    if (categoryId > 0) {
        obj["category_id"] = categoryId;
    } else {
        obj["category_id"] = QJsonValue(QJsonValue::Null);
    }
    if (supplierId > 0) {
        obj["supplier_id"] = supplierId;
    } else {
        obj["supplier_id"] = QJsonValue(QJsonValue::Null);
    }
    obj["cost_price"] = costPrice;
    obj["selling_price"] = sellingPrice;
    obj["mrp"] = mrp;
    obj["discount_pct"] = discountPct;
    obj["gst_rate"] = gstRate;
    obj["is_gst_inclusive"] = isGstInclusive;
    obj["unit"] = unit;
    obj["stock_quantity"] = stockQuantity;
    obj["min_stock"] = minStock;
    obj["max_stock"] = maxStock;
    if (expiryDate.isValid()) {
        obj["expiry_date"] = expiryDate.toString(Qt::ISODate);
    }
    obj["batch_number"] = batchNumber;
    obj["hsn_code"] = hsnCode;
    obj["description"] = description;
    obj["image_path"] = imagePath;
    obj["is_active"] = isActive;
    return obj;
}

void Product::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    name = obj["name"].toString();
    sku = obj["sku"].toString();
    barcode = obj["barcode"].toString();
    categoryId = obj["category_id"].toInt(-1);
    supplierId = obj["supplier_id"].toInt(-1);
    costPrice = obj["cost_price"].toDouble();
    sellingPrice = obj["selling_price"].toDouble();
    mrp = obj["mrp"].toDouble();
    discountPct = obj["discount_pct"].toDouble();
    gstRate = obj["gst_rate"].toDouble(18.0);
    isGstInclusive = obj["is_gst_inclusive"].toBool();
    unit = obj["unit"].toString("pcs");
    stockQuantity = obj["stock_quantity"].toDouble();
    minStock = obj["min_stock"].toDouble(5.0);
    maxStock = obj["max_stock"].toDouble();
    if (obj.contains("expiry_date")) {
        expiryDate = QDate::fromString(obj["expiry_date"].toString(), Qt::ISODate);
    }
    batchNumber = obj["batch_number"].toString();
    hsnCode = obj["hsn_code"].toString();
    description = obj["description"].toString();
    imagePath = obj["image_path"].toString();
    isActive = obj["is_active"].toBool(true);
}

} // namespace RetailMS

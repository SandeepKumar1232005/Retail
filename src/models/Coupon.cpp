#include "Coupon.h"
#include <QVariant>

namespace RetailMS {

bool Coupon::isValid(double orderValue) const {
    if (!isActive) return false;
    if (usageLimit > 0 && usedCount >= usageLimit) return false;
    if (orderValue < minOrderValue) return false;
    
    QDate today = QDate::currentDate();
    if (validFrom.isValid() && today < validFrom) return false;
    if (validUntil.isValid() && today > validUntil) return false;
    
    return true;
}

double Coupon::calculateDiscount(double orderValue) const {
    if (!isValid(orderValue)) return 0.0;
    
    double discount = 0.0;
    if (type == "percent") {
        discount = orderValue * (value / 100.0);
        if (maxDiscount > 0 && discount > maxDiscount) {
            discount = maxDiscount;
        }
    } else {
        discount = value;
    }
    return discount;
}

QJsonObject Coupon::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["code"] = code;
    obj["description"] = description;
    obj["type"] = type;
    obj["value"] = value;
    obj["min_order_value"] = minOrderValue;
    obj["max_discount"] = maxDiscount;
    obj["usage_limit"] = usageLimit;
    obj["used_count"] = usedCount;
    if (validFrom.isValid()) obj["valid_from"] = validFrom.toString(Qt::ISODate);
    if (validUntil.isValid()) obj["valid_until"] = validUntil.toString(Qt::ISODate);
    obj["is_active"] = isActive;
    return obj;
}

void Coupon::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    code = obj["code"].toString();
    description = obj["description"].toString();
    type = obj["type"].toString("flat");
    value = obj["value"].toDouble();
    minOrderValue = obj["min_order_value"].toDouble();
    maxDiscount = obj["max_discount"].toDouble();
    usageLimit = obj["usage_limit"].toInt();
    usedCount = obj["used_count"].toInt();
    if (obj.contains("valid_from")) validFrom = QDate::fromString(obj["valid_from"].toString(), Qt::ISODate);
    if (obj.contains("valid_until")) validUntil = QDate::fromString(obj["valid_until"].toString(), Qt::ISODate);
    isActive = obj["is_active"].toBool(true);
}

} // namespace RetailMS

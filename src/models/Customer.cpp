#include "Customer.h"
#include <QVariant>

namespace RetailMS {

QJsonObject Customer::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["phone"] = phone;
    obj["email"] = email;
    obj["address"] = address;
    obj["city"] = city;
    obj["pincode"] = pincode;
    obj["gst_number"] = gstNumber;
    obj["loyalty_points"] = loyaltyPoints;
    obj["total_spent"] = totalSpent;
    obj["tier"] = tier;
    if (dob.isValid()) obj["dob"] = dob.toString(Qt::ISODate);
    if (anniversary.isValid()) obj["anniversary"] = anniversary.toString(Qt::ISODate);
    obj["notes"] = notes;
    return obj;
}

void Customer::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    name = obj["name"].toString();
    phone = obj["phone"].toString();
    email = obj["email"].toString();
    address = obj["address"].toString();
    city = obj["city"].toString();
    pincode = obj["pincode"].toString();
    gstNumber = obj["gst_number"].toString();
    loyaltyPoints = obj["loyalty_points"].toInt();
    totalSpent = obj["total_spent"].toDouble();
    tier = obj["tier"].toString("silver");
    if (obj.contains("dob")) dob = QDate::fromString(obj["dob"].toString(), Qt::ISODate);
    if (obj.contains("anniversary")) anniversary = QDate::fromString(obj["anniversary"].toString(), Qt::ISODate);
    notes = obj["notes"].toString();
}

} // namespace RetailMS

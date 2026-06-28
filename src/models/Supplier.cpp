#include "Supplier.h"

namespace RetailMS {

QJsonObject Supplier::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["contact_name"] = contactName;
    obj["phone"] = phone;
    obj["email"] = email;
    obj["address"] = address;
    obj["gst_number"] = gstNumber;
    obj["pan_number"] = panNumber;
    obj["bank_account"] = bankAccount;
    obj["ifsc_code"] = ifscCode;
    obj["credit_days"] = creditDays;
    obj["is_active"] = isActive;
    return obj;
}

void Supplier::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    name = obj["name"].toString();
    contactName = obj["contact_name"].toString();
    phone = obj["phone"].toString();
    email = obj["email"].toString();
    address = obj["address"].toString();
    gstNumber = obj["gst_number"].toString();
    panNumber = obj["pan_number"].toString();
    bankAccount = obj["bank_account"].toString();
    ifscCode = obj["ifsc_code"].toString();
    creditDays = obj["credit_days"].toInt(30);
    isActive = obj["is_active"].toBool(true);
}

} // namespace RetailMS

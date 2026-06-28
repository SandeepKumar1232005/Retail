#include "User.h"

namespace RetailMS {

QString User::roleString() const {
    switch (role) {
        case UserRole::Admin: return "admin";
        case UserRole::Manager: return "manager";
        case UserRole::Cashier: return "cashier";
    }
    return "cashier";
}

QJsonObject User::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["username"] = username;
    obj["full_name"] = fullName;
    obj["role"] = roleString();
    obj["email"] = email;
    obj["phone"] = phone;
    obj["is_active"] = isActive;
    // Do not serialize password/salt here for security
    return obj;
}

void User::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    username = obj["username"].toString();
    fullName = obj["full_name"].toString();
    QString r = obj["role"].toString();
    if (r == "admin") role = UserRole::Admin;
    else if (r == "manager") role = UserRole::Manager;
    else role = UserRole::Cashier;
    
    email = obj["email"].toString();
    phone = obj["phone"].toString();
    isActive = obj["is_active"].toBool(true);
}

} // namespace RetailMS

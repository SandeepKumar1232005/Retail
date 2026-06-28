#pragma once
#include "BaseModel.h"
#include <QString>

namespace RetailMS {

enum class UserRole { Admin, Cashier, Manager };

class User : public BaseModel {
public:
    QString  username;
    QString  passwordHash;
    QString  salt;
    QString  fullName;
    UserRole role;
    QString  email;
    QString  phone;
    bool     isActive{true};
    QDateTime lastLogin;

    bool isAdmin()   const { return role == UserRole::Admin; }
    bool isCashier() const { return role == UserRole::Cashier; }
    QString roleString() const;
    
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
};

} // namespace RetailMS

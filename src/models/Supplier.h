#pragma once
#include "BaseModel.h"
#include <QString>

namespace RetailMS {

class Supplier : public BaseModel {
public:
    QString name;
    QString contactName;
    QString phone;
    QString email;
    QString address;
    QString gstNumber;
    QString panNumber;
    QString bankAccount;
    QString ifscCode;
    int creditDays{30};
    bool isActive{true};

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
};

} // namespace RetailMS

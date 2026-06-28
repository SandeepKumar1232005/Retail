#pragma once
#include "BaseModel.h"
#include <QString>
#include <QDate>

namespace RetailMS {

class Customer : public BaseModel {
public:
    QString name;
    QString phone;
    QString email;
    QString address;
    QString city;
    QString pincode;
    QString gstNumber;
    int loyaltyPoints{0};
    double totalSpent{0.0};
    QString tier{"silver"};
    QDate dob;
    QDate anniversary;
    QString notes;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
};

} // namespace RetailMS

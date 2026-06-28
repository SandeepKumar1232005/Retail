#pragma once
#include "BaseModel.h"
#include <QString>
#include <QDate>

namespace RetailMS {

class Employee : public BaseModel {
public:
    int userId{-1};
    QString employeeNo;
    QString department;
    QString designation;
    double salary{0.0};
    QDate joinDate;
    QString address;
    QString emergencyContact;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
};

} // namespace RetailMS

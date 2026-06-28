#include "Employee.h"
#include <QVariant>

namespace RetailMS {

QJsonObject Employee::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["user_id"] = userId;
    obj["employee_no"] = employeeNo;
    obj["department"] = department;
    obj["designation"] = designation;
    obj["salary"] = salary;
    if (joinDate.isValid()) obj["join_date"] = joinDate.toString(Qt::ISODate);
    obj["address"] = address;
    obj["emergency_contact"] = emergencyContact;
    return obj;
}

void Employee::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    userId = obj["user_id"].toInt(-1);
    employeeNo = obj["employee_no"].toString();
    department = obj["department"].toString();
    designation = obj["designation"].toString();
    salary = obj["salary"].toDouble();
    if (obj.contains("join_date")) joinDate = QDate::fromString(obj["join_date"].toString(), Qt::ISODate);
    address = obj["address"].toString();
    emergencyContact = obj["emergency_contact"].toString();
}

} // namespace RetailMS

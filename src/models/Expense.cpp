#include "Expense.h"
#include <QVariant>

namespace RetailMS {

QJsonObject Expense::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["category"] = category;
    obj["description"] = description;
    obj["amount"] = amount;
    obj["payment_mode"] = paymentMode;
    obj["paid_to"] = paidTo;
    obj["receipt_path"] = receiptPath;
    if (expenseDate.isValid()) obj["expense_date"] = expenseDate.toString(Qt::ISODate);
    obj["user_id"] = userId;
    return obj;
}

void Expense::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    category = obj["category"].toString();
    description = obj["description"].toString();
    amount = obj["amount"].toDouble();
    paymentMode = obj["payment_mode"].toString();
    paidTo = obj["paid_to"].toString();
    receiptPath = obj["receipt_path"].toString();
    if (obj.contains("expense_date")) expenseDate = QDate::fromString(obj["expense_date"].toString(), Qt::ISODate);
    userId = obj["user_id"].toInt(-1);
}

} // namespace RetailMS

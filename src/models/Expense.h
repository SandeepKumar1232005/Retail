#pragma once
#include "BaseModel.h"
#include <QString>
#include <QDate>

namespace RetailMS {

class Expense : public BaseModel {
public:
    QString category;
    QString description;
    double amount{0.0};
    QString paymentMode;
    QString paidTo;
    QString receiptPath;
    QDate expenseDate;
    int userId{-1};

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
};

} // namespace RetailMS

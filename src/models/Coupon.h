#pragma once
#include "BaseModel.h"
#include <QString>
#include <QDate>

namespace RetailMS {

class Coupon : public BaseModel {
public:
    QString code;
    QString description;
    QString type{"flat"}; // flat or percent
    double value{0.0};
    double minOrderValue{0.0};
    double maxDiscount{0.0};
    int usageLimit{0};
    int usedCount{0};
    QDate validFrom;
    QDate validUntil;
    bool isActive{true};

    bool isValid(double orderValue) const;
    double calculateDiscount(double orderValue) const;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
};

} // namespace RetailMS

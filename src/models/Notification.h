#pragma once
#include "BaseModel.h"
#include <QString>

namespace RetailMS {

class Notification : public BaseModel {
public:
    QString type;
    QString message;
    QString reference;
    bool isRead{false};

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
};

} // namespace RetailMS

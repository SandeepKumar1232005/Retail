#pragma once
#include "BaseModel.h"
#include <QString>

namespace RetailMS {

class Category : public BaseModel {
public:
    QString name;
    QString description;
    int parentId{-1};
    QString icon;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
};

} // namespace RetailMS

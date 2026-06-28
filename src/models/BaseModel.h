#pragma once

#include <QDateTime>
#include <QJsonObject>

namespace RetailMS {

class BaseModel {
public:
    virtual ~BaseModel() = default;
    
    int id{-1};
    QDateTime createdAt;
    QDateTime updatedAt;

    virtual QJsonObject toJson() const = 0;
    virtual void fromJson(const QJsonObject& obj) = 0;
    
    bool isNew() const { return id == -1; }
};

} // namespace RetailMS

#include "Category.h"

namespace RetailMS {

QJsonObject Category::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["name"] = name;
    obj["description"] = description;
    obj["parent_id"] = parentId;
    obj["icon"] = icon;
    return obj;
}

void Category::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    name = obj["name"].toString();
    description = obj["description"].toString();
    parentId = obj["parent_id"].toInt(-1);
    icon = obj["icon"].toString();
}

} // namespace RetailMS

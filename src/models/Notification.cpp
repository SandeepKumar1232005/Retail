#include "Notification.h"
#include <QVariant>

namespace RetailMS {

QJsonObject Notification::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["type"] = type;
    obj["message"] = message;
    obj["reference"] = reference;
    obj["is_read"] = isRead;
    return obj;
}

void Notification::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    type = obj["type"].toString();
    message = obj["message"].toString();
    reference = obj["reference"].toString();
    isRead = obj["is_read"].toBool(false);
}

} // namespace RetailMS

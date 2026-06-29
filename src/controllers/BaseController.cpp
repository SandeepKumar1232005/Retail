#include "BaseController.h"
#include "../utils/Logger.h"
#include <exception>

namespace RetailMS {

BaseController::BaseController(QObject* parent) : QObject(parent) {}

void BaseController::handleError(const QString& context, const std::exception& e) {
    QString msg = QString("Error in %1: %2").arg(context).arg(e.what());
    LOG_ERROR(msg);
    emit errorOccurred(msg);
}

} // namespace RetailMS

#include "AppException.h"

namespace RetailMS {

AppException::AppException(QString msg, QString code)
    : m_message(std::move(msg)), m_code(std::move(code)) {
}

const char* AppException::what() const noexcept {
    if (m_whatBuffer.isEmpty()) {
        QString fullMsg = QString("[%1] %2").arg(m_code, m_message);
        m_whatBuffer = fullMsg.toUtf8();
    }
    return m_whatBuffer.constData();
}

void ValidationException::addFieldError(const QString& field, const QString& error) {
    m_fieldErrors.insert(field, error);
}

const QMap<QString, QString>& ValidationException::fieldErrors() const {
    return m_fieldErrors;
}

} // namespace RetailMS

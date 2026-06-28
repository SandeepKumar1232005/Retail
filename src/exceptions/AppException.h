#pragma once

#include <exception>
#include <QString>
#include <QMap>

namespace RetailMS {

class AppException : public std::exception {
public:
    explicit AppException(QString msg, QString code = "ERR_GENERAL");
    const char* what() const noexcept override;
    const QString& message() const { return m_message; }
    const QString& errorCode() const { return m_code; }

protected:
    QString m_message;
    QString m_code;
    mutable QByteArray m_whatBuffer; // For returning const char*
};

class DatabaseException : public AppException { 
public:
    using AppException::AppException; 
};

class AuthException : public AppException { 
public:
    using AppException::AppException; 
};

class ValidationException : public AppException {
public:
    using AppException::AppException;
    void addFieldError(const QString& field, const QString& error);
    const QMap<QString, QString>& fieldErrors() const;
private:
    QMap<QString, QString> m_fieldErrors;
};

class StockException : public AppException { 
public:
    using AppException::AppException; 
};

class PrintException : public AppException { 
public:
    using AppException::AppException; 
};

class BackupException : public AppException { 
public:
    using AppException::AppException; 
};

} // namespace RetailMS

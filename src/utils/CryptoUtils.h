#pragma once
#include <QString>

namespace RetailMS {
namespace CryptoUtils {
    QString generateSalt(int length = 32);
    QString hashPassword(const QString& password, const QString& salt);
    bool    verifyPassword(const QString& password, const QString& salt, const QString& hash);
    QString generateToken(int length = 64);
}
}

#include "CryptoUtils.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <QByteArray>

namespace RetailMS {
namespace CryptoUtils {

QString generateSalt(int length) {
    QByteArray salt(length, 0);
    RAND_bytes(reinterpret_cast<unsigned char*>(salt.data()), length);
    return salt.toBase64();
}

QString hashPassword(const QString& password, const QString& salt) {
    QByteArray pwdBytes = password.toUtf8();
    QByteArray saltBytes = QByteArray::fromBase64(salt.toUtf8());
    QByteArray out(32, 0); // SHA-256 output size is 32 bytes
    
    PKCS5_PBKDF2_HMAC(pwdBytes.constData(), pwdBytes.length(),
                      reinterpret_cast<const unsigned char*>(saltBytes.constData()), saltBytes.length(),
                      100000, EVP_sha256(),
                      out.length(), reinterpret_cast<unsigned char*>(out.data()));
                      
    return out.toBase64();
}

bool verifyPassword(const QString& password, const QString& salt, const QString& hash) {
    QString computedHash = hashPassword(password, salt);
    return computedHash == hash;
}

QString generateToken(int length) {
    QByteArray token(length, 0);
    RAND_bytes(reinterpret_cast<unsigned char*>(token.data()), length);
    return token.toHex();
}

}
}

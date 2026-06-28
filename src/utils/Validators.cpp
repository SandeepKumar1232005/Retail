#include "Validators.h"
#include <QRegularExpression>

namespace RetailMS {
namespace Validators {

bool isValidEmail(const QString& email) {
    QRegularExpression re("^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z0-9-.]+$");
    return re.match(email).hasMatch();
}

bool isValidPhone(const QString& phone) {
    QRegularExpression re("^\\+?[0-9]{10,14}$");
    return re.match(phone).hasMatch();
}

bool isValidGSTNumber(const QString& gst) {
    QRegularExpression re("^[0-9]{2}[A-Z]{5}[0-9]{4}[A-Z]{1}[1-9A-Z]{1}Z[0-9A-Z]{1}$");
    return re.match(gst).hasMatch();
}

bool isValidIFSC(const QString& ifsc) {
    QRegularExpression re("^[A-Z]{4}0[A-Z0-9]{6}$");
    return re.match(ifsc).hasMatch();
}

bool isValidPAN(const QString& pan) {
    QRegularExpression re("^[A-Z]{5}[0-9]{4}[A-Z]{1}$");
    return re.match(pan).hasMatch();
}

}
}

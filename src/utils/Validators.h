#pragma once
#include <QString>

namespace RetailMS {
namespace Validators {
    bool isValidEmail(const QString& email);
    bool isValidPhone(const QString& phone);
    bool isValidGSTNumber(const QString& gst);
    bool isValidIFSC(const QString& ifsc);
    bool isValidPAN(const QString& pan);
}
}

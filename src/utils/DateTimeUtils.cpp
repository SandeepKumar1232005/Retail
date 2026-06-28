#include "DateTimeUtils.h"

namespace RetailMS {
namespace DateTimeUtils {

QString formatDateTime(const QDateTime& dt) {
    return dt.toString("yyyy-MM-dd HH:mm:ss");
}

QString formatDate(const QDate& date) {
    return date.toString("yyyy-MM-dd");
}

QDateTime parseDateTime(const QString& str) {
    return QDateTime::fromString(str, Qt::ISODate);
}

QDate parseDate(const QString& str) {
    return QDate::fromString(str, Qt::ISODate);
}

QDate startOfWeek(const QDate& date) {
    return date.addDays(-(date.dayOfWeek() - 1));
}

QDate startOfMonth(const QDate& date) {
    return QDate(date.year(), date.month(), 1);
}

QDate startOfYear(const QDate& date) {
    return QDate(date.year(), 1, 1);
}

}
}

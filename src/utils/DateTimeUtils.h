#pragma once
#include <QDateTime>
#include <QString>

namespace RetailMS {
namespace DateTimeUtils {
    QString formatDateTime(const QDateTime& dt);
    QString formatDate(const QDate& date);
    QDateTime parseDateTime(const QString& str);
    QDate parseDate(const QString& str);
    QDate startOfWeek(const QDate& date);
    QDate startOfMonth(const QDate& date);
    QDate startOfYear(const QDate& date);
}
}

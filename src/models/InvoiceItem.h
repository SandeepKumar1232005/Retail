#pragma once
#include <QString>
#include <QJsonObject>

namespace RetailMS {

class InvoiceItem {
public:
    int      id{-1};
    int      invoiceId{-1};
    int      productId{-1};
    QString  productName;
    QString  barcode;
    double   quantity{0.0};
    QString  unit{"pcs"};
    double   costPrice{0.0};
    double   sellingPrice{0.0};
    double   discountPct{0.0};
    double   discountAmt{0.0};
    double   taxableAmt{0.0};
    double   cgstRate{0.0};
    double   sgstRate{0.0};
    double   cgstAmt{0.0};
    double   sgstAmt{0.0};
    double   total{0.0};

    QJsonObject toJson() const;
    void fromJson(const QJsonObject& obj);
};

} // namespace RetailMS

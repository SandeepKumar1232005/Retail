#pragma once
#include "BaseModel.h"
#include <QString>
#include <QDate>

namespace RetailMS {

class Product : public BaseModel {
public:
    QString  name;
    QString  sku;
    QString  barcode;
    int      categoryId{-1};
    QString  categoryName; // For automatic mapping
    int      supplierId{-1};
    double   costPrice{0.0};
    double   sellingPrice{0.0};
    double   mrp{0.0};
    double   discountPct{0.0};
    double   gstRate{18.0};
    bool     isGstInclusive{false};
    QString  unit{"pcs"};
    double   stockQuantity{0.0};
    double   minStock{5.0};
    double   maxStock{0.0};
    QDate    expiryDate;
    QString  batchNumber;
    QString  hsnCode;
    QString  description;
    QString  imagePath;
    bool     isActive{true};

    double  cgstRate()      const { return gstRate / 2.0; }
    double  sgstRate()      const { return gstRate / 2.0; }
    bool    isLowStock()    const { return stockQuantity <= minStock; }
    bool    isOutOfStock()  const { return stockQuantity <= 0; }
    bool    isExpired()     const;
    bool    isExpiringSoon(int daysAhead = 30) const;
    double  effectivePrice(double qty = 1.0) const;  // after discount
    double  profitMargin()  const;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
};

} // namespace RetailMS

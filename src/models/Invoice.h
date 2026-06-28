#pragma once
#include "BaseModel.h"
#include "InvoiceItem.h"
#include <QString>
#include <vector>

namespace RetailMS {

class Invoice : public BaseModel {
public:
    enum class Status { Draft, Paid, Refunded, Voided };
    enum class PaymentMode { Cash, UPI, Card, Split };

    QString  invoiceNumber;
    int      customerId{-1};
    int      userId{-1};
    std::vector<InvoiceItem> items;

    double   subtotal{0.0};
    double   discountAmt{0.0};
    QString  couponCode;
    double   couponDiscount{0.0};
    double   cgstAmt{0.0};
    double   sgstAmt{0.0};
    double   igstAmt{0.0};
    double   roundOff{0.0};
    double   grandTotal{0.0};
    double   amountPaid{0.0};
    double   changeReturned{0.0};
    PaymentMode paymentMode{PaymentMode::Cash};
    QString  paymentRef;
    Status   status{Status::Draft};
    QString  notes;
    int      loyaltyRedeemed{0};
    int      loyaltyEarned{0};
    QDateTime invoiceDate;

    void recalculate();                    // recompute all totals from items
    double taxableAmount() const;
    bool   isVoided() const { return status == Status::Voided; }
    QString statusString() const;
    QString paymentModeString() const;
    
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;
};

} // namespace RetailMS

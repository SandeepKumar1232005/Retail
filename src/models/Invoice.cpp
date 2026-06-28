#include "Invoice.h"
#include <QJsonArray>
#include <cmath>

namespace RetailMS {

void Invoice::recalculate() {
    subtotal = 0.0;
    discountAmt = 0.0;
    cgstAmt = 0.0;
    sgstAmt = 0.0;
    igstAmt = 0.0;
    grandTotal = 0.0;

    for (const auto& item : items) {
        subtotal += (item.sellingPrice * item.quantity);
        discountAmt += item.discountAmt;
        cgstAmt += item.cgstAmt;
        sgstAmt += item.sgstAmt;
        // igstAmt could be calculated if interstate logic applies, for now it's manual or calculated elsewhere
    }
    
    double totalBeforeRound = subtotal - discountAmt - couponDiscount + cgstAmt + sgstAmt + igstAmt;
    grandTotal = std::round(totalBeforeRound);
    roundOff = grandTotal - totalBeforeRound;
    
    if (amountPaid > 0) {
        changeReturned = amountPaid - grandTotal;
    }
}

double Invoice::taxableAmount() const {
    double taxAmount = 0.0;
    for (const auto& item : items) {
        taxAmount += item.taxableAmt;
    }
    return taxAmount;
}

QString Invoice::statusString() const {
    switch (status) {
        case Status::Draft: return "draft";
        case Status::Paid: return "paid";
        case Status::Refunded: return "refunded";
        case Status::Voided: return "voided";
    }
    return "draft";
}

QString Invoice::paymentModeString() const {
    switch (paymentMode) {
        case PaymentMode::Cash: return "cash";
        case PaymentMode::UPI: return "upi";
        case PaymentMode::Card: return "card";
        case PaymentMode::Split: return "split";
    }
    return "cash";
}

QJsonObject Invoice::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["invoice_number"] = invoiceNumber;
    obj["customer_id"] = customerId;
    obj["user_id"] = userId;
    
    QJsonArray itemsArr;
    for (const auto& item : items) {
        itemsArr.append(item.toJson());
    }
    obj["items"] = itemsArr;
    
    obj["subtotal"] = subtotal;
    obj["discount_amt"] = discountAmt;
    obj["coupon_code"] = couponCode;
    obj["coupon_discount"] = couponDiscount;
    obj["cgst_amt"] = cgstAmt;
    obj["sgst_amt"] = sgstAmt;
    obj["igst_amt"] = igstAmt;
    obj["round_off"] = roundOff;
    obj["grand_total"] = grandTotal;
    obj["amount_paid"] = amountPaid;
    obj["change_returned"] = changeReturned;
    obj["payment_mode"] = paymentModeString();
    obj["payment_ref"] = paymentRef;
    obj["status"] = statusString();
    obj["notes"] = notes;
    obj["loyalty_redeemed"] = loyaltyRedeemed;
    obj["loyalty_earned"] = loyaltyEarned;
    if (invoiceDate.isValid()) {
        obj["invoice_date"] = invoiceDate.toString(Qt::ISODate);
    }
    return obj;
}

void Invoice::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    invoiceNumber = obj["invoice_number"].toString();
    customerId = obj["customer_id"].toInt(-1);
    userId = obj["user_id"].toInt(-1);
    
    items.clear();
    QJsonArray itemsArr = obj["items"].toArray();
    for (int i = 0; i < itemsArr.size(); ++i) {
        InvoiceItem item;
        item.fromJson(itemsArr[i].toObject());
        items.push_back(item);
    }
    
    subtotal = obj["subtotal"].toDouble();
    discountAmt = obj["discount_amt"].toDouble();
    couponCode = obj["coupon_code"].toString();
    couponDiscount = obj["coupon_discount"].toDouble();
    cgstAmt = obj["cgst_amt"].toDouble();
    sgstAmt = obj["sgst_amt"].toDouble();
    igstAmt = obj["igst_amt"].toDouble();
    roundOff = obj["round_off"].toDouble();
    grandTotal = obj["grand_total"].toDouble();
    amountPaid = obj["amount_paid"].toDouble();
    changeReturned = obj["change_returned"].toDouble();
    
    QString pm = obj["payment_mode"].toString();
    if (pm == "upi") paymentMode = PaymentMode::UPI;
    else if (pm == "card") paymentMode = PaymentMode::Card;
    else if (pm == "split") paymentMode = PaymentMode::Split;
    else paymentMode = PaymentMode::Cash;
    
    paymentRef = obj["payment_ref"].toString();
    
    QString st = obj["status"].toString();
    if (st == "paid") status = Status::Paid;
    else if (st == "refunded") status = Status::Refunded;
    else if (st == "voided") status = Status::Voided;
    else status = Status::Draft;
    
    notes = obj["notes"].toString();
    loyaltyRedeemed = obj["loyalty_redeemed"].toInt();
    loyaltyEarned = obj["loyalty_earned"].toInt();
    
    if (obj.contains("invoice_date")) {
        invoiceDate = QDateTime::fromString(obj["invoice_date"].toString(), Qt::ISODate);
    }
}

} // namespace RetailMS

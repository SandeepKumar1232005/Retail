#include "InvoiceItem.h"

namespace RetailMS {

QJsonObject InvoiceItem::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["invoice_id"] = invoiceId;
    obj["product_id"] = productId;
    obj["product_name"] = productName;
    obj["barcode"] = barcode;
    obj["quantity"] = quantity;
    obj["unit"] = unit;
    obj["cost_price"] = costPrice;
    obj["selling_price"] = sellingPrice;
    obj["discount_pct"] = discountPct;
    obj["discount_amt"] = discountAmt;
    obj["taxable_amt"] = taxableAmt;
    obj["cgst_rate"] = cgstRate;
    obj["sgst_rate"] = sgstRate;
    obj["cgst_amt"] = cgstAmt;
    obj["sgst_amt"] = sgstAmt;
    obj["total"] = total;
    return obj;
}

void InvoiceItem::fromJson(const QJsonObject& obj) {
    id = obj["id"].toInt(-1);
    invoiceId = obj["invoice_id"].toInt(-1);
    productId = obj["product_id"].toInt(-1);
    productName = obj["product_name"].toString();
    barcode = obj["barcode"].toString();
    quantity = obj["quantity"].toDouble();
    unit = obj["unit"].toString();
    costPrice = obj["cost_price"].toDouble();
    sellingPrice = obj["selling_price"].toDouble();
    discountPct = obj["discount_pct"].toDouble();
    discountAmt = obj["discount_amt"].toDouble();
    taxableAmt = obj["taxable_amt"].toDouble();
    cgstRate = obj["cgst_rate"].toDouble();
    sgstRate = obj["sgst_rate"].toDouble();
    cgstAmt = obj["cgst_amt"].toDouble();
    sgstAmt = obj["sgst_amt"].toDouble();
    total = obj["total"].toDouble();
}

} // namespace RetailMS

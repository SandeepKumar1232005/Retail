#pragma once
#include "BaseController.h"
#include <memory>
#include "../models/Invoice.h"

namespace RetailMS {

class BillingService;

class BillingController : public BaseController {
    Q_OBJECT
public:
    explicit BillingController(std::shared_ptr<BillingService> billingService, QObject* parent = nullptr);

    Invoice currentInvoice() const;
    
public slots:
    void createNewInvoice();
    void addProductToInvoice(int productId, double qty = 1.0);
    void addProductByBarcode(const QString& barcode, double qty = 1.0);
    void updateItemQuantity(int index, double qty);
    void removeItem(int index);
    void applyDiscount(double pct);
    void applyCoupon(const QString& code);
    void removeCoupon();
    void setCustomer(int customerId);
    void finalizeInvoice(double amountPaid, Invoice::PaymentMode mode, const QString& ref = "");

signals:
    void invoiceUpdated(const Invoice& inv);
    void checkoutComplete(const Invoice& inv);

private:
    std::shared_ptr<BillingService> m_billingService;
    Invoice m_currentInvoice;
};

} // namespace RetailMS

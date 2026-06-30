#pragma once
#include "BaseController.h"
#include <memory>
#include <vector>
#include "../models/Invoice.h"

namespace RetailMS {

class BillingService;
class CustomerService;

class BillingController : public BaseController {
    Q_OBJECT
public:
    explicit BillingController(std::shared_ptr<BillingService> billingService, QObject* parent = nullptr);

    Invoice currentInvoice() const;
    std::vector<Invoice> getInvoices(const QDate& from, const QDate& to) const;
    
public slots:
    void createNewInvoice();
    void holdCurrentBill();
    void resumeBill(int index);
    void addProductToInvoice(int productId, double qty = 1.0);
    void addProductByBarcode(const QString& barcode, double qty = 1.0);
    void updateItemQuantity(int index, double qty);
    void removeItem(int index);
    void applyDiscount(double pct);
    void applyCoupon(const QString& code);
    void removeCoupon();
    void setCustomer(int customerId);
    void setCustomerPhone(const QString& phone);
    void redeemLoyaltyPoints(int points);
    void finalizeInvoice(double amountPaid, Invoice::PaymentMode mode, const QString& ref = "");
    
    std::shared_ptr<CustomerService> customerService() const;

signals:
    void invoiceUpdated(const Invoice& inv);
    void checkoutComplete(const Invoice& inv);

private:
    std::shared_ptr<BillingService> m_billingService;
    Invoice m_currentInvoice;
    std::vector<Invoice> m_heldInvoices;
};

} // namespace RetailMS

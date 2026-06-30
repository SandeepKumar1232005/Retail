#pragma once
#include <QObject>
#include <memory>
#include <vector>
#include "../models/Invoice.h"
#include "../models/Product.h"
#include "../models/Customer.h"

namespace RetailMS {

class InvoiceRepository;
class ProductService;
class InventoryService;
class CustomerService;
class CouponService;

class BillingService : public QObject {
    Q_OBJECT
public:
    BillingService(std::shared_ptr<InvoiceRepository> invoiceRepo,
                   std::shared_ptr<ProductService> productService,
                   std::shared_ptr<InventoryService> inventoryService,
                   std::shared_ptr<CustomerService> customerService,
                   std::shared_ptr<CouponService> couponService,
                   QObject* parent = nullptr);

    Invoice createDraftInvoice();
    void    addItem(Invoice& inv, int productId, double qty);
    void    addItemByBarcode(Invoice& inv, const QString& barcode, double qty);
    void    removeItem(Invoice& inv, int itemIndex);
    void    updateQuantity(Invoice& inv, int itemIndex, double qty);
    void    applyDiscount(Invoice& inv, double pct);
    bool    applyCoupon(Invoice& inv, const QString& code);
    void    removeCoupon(Invoice& inv);
    void    applyLoyaltyRedemption(Invoice& inv, const Customer& c, int points);
    void    redeemLoyaltyPoints(Invoice& inv, int points);
    Invoice finaliseInvoice(Invoice& inv, double amountPaid, Invoice::PaymentMode mode, const QString& ref = {});
    bool    voidInvoice(int invoiceId, const QString& reason);
    Invoice processRefund(int invoiceId);

    QString generateInvoiceNumber() const;
    std::vector<Invoice> getInvoicesByDateRange(const QDate& from, const QDate& to) const;
    std::shared_ptr<CustomerService> customerService() const { return m_customerService; }

signals:
    void invoiceCreated(const Invoice& inv);
    void stockDepleted(const Product& p);

private:
    std::shared_ptr<InvoiceRepository> m_invoiceRepo;
    std::shared_ptr<ProductService> m_productService;
    std::shared_ptr<InventoryService> m_inventoryService;
    std::shared_ptr<CustomerService> m_customerService;
    std::shared_ptr<CouponService> m_couponService;

    void deductStock(const Invoice& inv);
    void awardLoyaltyPoints(const Invoice& inv);
    void logActivity(const QString& action, const QJsonObject& details);
};

} // namespace RetailMS

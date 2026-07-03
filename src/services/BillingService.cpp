#include "BillingService.h"
#include "../repository/InvoiceRepository.h"
#include "ProductService.h"
#include "InventoryService.h"
#include "CustomerService.h"
#include "CouponService.h"
#include "../services/SessionManager.h"
#include "../utils/Logger.h"
#include "../exceptions/AppException.h"
#include <QDate>
#include "../database/DatabaseManager.h"
#include "../database/Transaction.h"

namespace RetailMS {

BillingService::BillingService(std::shared_ptr<InvoiceRepository> invoiceRepo,
                               std::shared_ptr<ProductService> productService,
                               std::shared_ptr<InventoryService> inventoryService,
                               std::shared_ptr<CustomerService> customerService,
                               std::shared_ptr<CouponService> couponService,
                               QObject* parent)
    : QObject(parent),
      m_invoiceRepo(std::move(invoiceRepo)),
      m_productService(std::move(productService)),
      m_inventoryService(std::move(inventoryService)),
      m_customerService(std::move(customerService)),
      m_couponService(std::move(couponService)) {}

Invoice BillingService::createDraftInvoice() {
    Invoice inv;
    inv.invoiceNumber = generateInvoiceNumber();
    if (SessionManager::instance().isLoggedIn()) {
        inv.userId = SessionManager::instance().currentUser().id;
    }
    inv.status = Invoice::Status::Draft;
    inv.invoiceDate = QDateTime::currentDateTime();
    return inv;
}

void BillingService::addItem(Invoice& inv, int productId, double qty) {
    auto optProduct = m_productService->getProductById(productId);
    if (!optProduct) throw AppException("Product not found");
    Product p = optProduct.value();
    
    InvoiceItem item;
    item.productId = p.id;
    item.productName = p.name;
    item.barcode = p.barcode;
    item.quantity = qty;
    item.unit = p.unit;
    item.costPrice = p.costPrice;
    item.sellingPrice = p.sellingPrice;
    
    item.cgstRate = p.cgstRate();
    item.sgstRate = p.sgstRate();
    
    item.total = p.effectivePrice(qty); // incorporates discountPct
    item.discountPct = p.discountPct;
    item.discountAmt = (p.sellingPrice * qty) - item.total;
    
    if (p.isGstInclusive) {
        item.taxableAmt = item.total / (1.0 + p.gstRate / 100.0);
        double tax = item.total - item.taxableAmt;
        item.cgstAmt = tax / 2.0;
        item.sgstAmt = tax / 2.0;
    } else {
        item.taxableAmt = item.total;
        item.cgstAmt = item.taxableAmt * item.cgstRate / 100.0;
        item.sgstAmt = item.taxableAmt * item.sgstRate / 100.0;
        item.total += (item.cgstAmt + item.sgstAmt);
    }

    bool found = false;
    for (auto& existingItem : inv.items) {
        if (existingItem.productId == p.id) {
            existingItem.quantity += qty;
            existingItem.total = p.effectivePrice(existingItem.quantity);
            existingItem.discountAmt = (p.sellingPrice * existingItem.quantity) - existingItem.total;
            
            if (p.isGstInclusive) {
                existingItem.taxableAmt = existingItem.total / (1.0 + p.gstRate / 100.0);
                double tax = existingItem.total - existingItem.taxableAmt;
                existingItem.cgstAmt = tax / 2.0;
                existingItem.sgstAmt = tax / 2.0;
            } else {
                existingItem.taxableAmt = existingItem.total;
                existingItem.cgstAmt = existingItem.taxableAmt * existingItem.cgstRate / 100.0;
                existingItem.sgstAmt = existingItem.taxableAmt * existingItem.sgstRate / 100.0;
                existingItem.total += (existingItem.cgstAmt + existingItem.sgstAmt);
            }
            found = true;
            break;
        }
    }
    
    if (!found) {
        inv.items.push_back(item);
    }
    
    inv.recalculate();
}

void BillingService::addItemByBarcode(Invoice& inv, const QString& barcode, double qty) {
    auto optProduct = m_productService->getProductByBarcode(barcode);
    if (!optProduct) throw AppException("Product with barcode not found");
    addItem(inv, optProduct.value().id, qty);
}

void BillingService::removeItem(Invoice& inv, int itemIndex) {
    if (itemIndex >= 0 && itemIndex < static_cast<int>(inv.items.size())) {
        inv.items.erase(inv.items.begin() + itemIndex);
        inv.recalculate();
    }
}

void BillingService::updateQuantity(Invoice& inv, int itemIndex, double qty) {
    if (itemIndex >= 0 && itemIndex < static_cast<int>(inv.items.size())) {
        int pid = inv.items[itemIndex].productId;
        removeItem(inv, itemIndex);
        addItem(inv, pid, qty);
    }
}

void BillingService::applyDiscount(Invoice& inv, double pct) {
    inv.discountAmt = (inv.subtotal * pct / 100.0);
    inv.recalculate();
}

bool BillingService::applyCoupon(Invoice& inv, const QString& code) {
    double discount = 0.0;
    if (m_couponService->validateCoupon(code, inv.subtotal - inv.discountAmt, discount)) {
        inv.couponCode = code;
        inv.couponDiscount = discount;
        inv.recalculate();
        return true;
    }
    return false;
}

void BillingService::removeCoupon(Invoice& inv) {
    inv.couponCode.clear();
    inv.couponDiscount = 0.0;
    inv.recalculate();
}

void BillingService::applyLoyaltyRedemption(Invoice& inv, const Customer& c, int points) {
    if (c.loyaltyPoints >= points) {
        double inRupees = m_customerService->pointsToRupees(points);
        inv.discountAmt += inRupees; // simplistically apply as general discount
        inv.loyaltyRedeemed = points;
        inv.recalculate();
    }
}

void BillingService::redeemLoyaltyPoints(Invoice& inv, int points) {
    if (inv.customerId > 0) {
        auto optC = m_customerService->getCustomerById(inv.customerId);
        if (optC) {
            applyLoyaltyRedemption(inv, optC.value(), points);
        }
    }
}

Invoice BillingService::finaliseInvoice(Invoice& inv, double amountPaid, Invoice::PaymentMode mode, const QString& ref) {
    inv.amountPaid = amountPaid;
    inv.paymentMode = mode;
    inv.paymentRef = ref;
    inv.status = Invoice::Status::Paid;
    inv.recalculate();
    
    Transaction tx(DatabaseManager::instance());
    
    try {
        deductStock(inv);
        
        // Auto-create or link customer
        if (inv.customerId <= 0 && !inv.customerPhone.isEmpty()) {
            auto existing = m_customerService->getCustomerByPhone(inv.customerPhone);
            if (existing) {
                inv.customerId = existing->id;
                inv.customerName = existing->name;
            } else {
                Customer newCus;
                newCus.phone = inv.customerPhone;
                newCus.name = "Customer";
                newCus.tier = "regular";
                newCus.loyaltyPoints = 0;
                newCus.totalSpent = 0.0;
                int newId = m_customerService->saveCustomer(newCus);
                if (newId > 0) {
                    inv.customerId = newId;
                    inv.customerName = newCus.name;
                }
            }
        } else if (inv.customerId > 0 && inv.customerPhone.isEmpty()) {
            auto optC = m_customerService->getCustomerById(inv.customerId);
            if (optC) {
                inv.customerPhone = optC->phone;
                inv.customerName = optC->name;
            }
        }

        if (inv.customerId > 0) {
            awardLoyaltyPoints(inv);
        }
        
        if (!inv.couponCode.isEmpty()) {
            m_couponService->recordCouponUsage(inv.couponCode);
        }
        
        inv.id = m_invoiceRepo->save(inv);
        
        tx.commit();
    } catch (...) {
        tx.rollback();
        throw;
    }

    emit invoiceCreated(inv);
    return inv;
}

bool BillingService::voidInvoice(int invoiceId, const QString& reason) {
    if (SessionManager::instance().isLoggedIn() && !SessionManager::instance().currentUser().isAdmin()) {
        throw AuthException("Unauthorized: Admin access required to void invoices.");
    }
    auto optInv = m_invoiceRepo->findById(invoiceId);
    if (!optInv) return false;
    Invoice inv = optInv.value();
    if (inv.status == Invoice::Status::Voided) return false;
    
    inv.status = Invoice::Status::Voided;
    inv.notes = "Voided: " + reason;
    m_invoiceRepo->update(inv);
    
    for (const auto& item : inv.items) {
        m_inventoryService->adjustStock(item.productId, item.quantity, "return", "Invoice voided", inv.id);
    }
    
    return true;
}

Invoice BillingService::processRefund(int invoiceId) {
    if (SessionManager::instance().isLoggedIn() && !SessionManager::instance().currentUser().isAdmin()) {
        throw AuthException("Unauthorized: Admin access required to process refunds.");
    }
    auto optInv = m_invoiceRepo->findById(invoiceId);
    if (!optInv) throw AppException("Invoice not found");
    Invoice inv = optInv.value();
    
    inv.status = Invoice::Status::Refunded;
    m_invoiceRepo->update(inv);
    
    for (const auto& item : inv.items) {
        m_inventoryService->adjustStock(item.productId, item.quantity, "return", "Refunded", inv.id);
    }
    return inv;
}

QString BillingService::generateInvoiceNumber() const {
    QString date = QDate::currentDate().toString("yyyyMMdd");
    int count = m_invoiceRepo->count();
    return QString("INV-%1-%2").arg(date).arg(count + 1, 4, 10, QChar('0'));
}

std::vector<Invoice> BillingService::getInvoicesByDateRange(const QDate& from, const QDate& to) const {
    if (SessionManager::instance().isLoggedIn() && !SessionManager::instance().currentUser().isAdmin()) {
        // Staff can only see their own invoices
        return m_invoiceRepo->findWhere("date(invoice_date) >= ? AND date(invoice_date) <= ? AND user_id = ?", 
                                        {from.toString(Qt::ISODate), to.toString(Qt::ISODate), SessionManager::instance().currentUser().id});
    }
    return m_invoiceRepo->findWhere("date(invoice_date) >= ? AND date(invoice_date) <= ?", 
                                    {from.toString(Qt::ISODate), to.toString(Qt::ISODate)});
}

void BillingService::deductStock(const Invoice& inv) {
    for (const auto& item : inv.items) {
        m_inventoryService->adjustStock(item.productId, -item.quantity, "sale", "Invoice " + inv.invoiceNumber, inv.id);
        
        auto optProd = m_productService->getProductById(item.productId);
        if (optProd && optProd->isLowStock()) {
            emit stockDepleted(optProd.value());
        }
    }
}

void BillingService::awardLoyaltyPoints(const Invoice& inv) {
    if (inv.customerId > 0) {
        auto optC = m_customerService->getCustomerById(inv.customerId);
        if (optC) {
            Customer c = optC.value();
            c.loyaltyPoints -= inv.loyaltyRedeemed;
            int earned = m_customerService->calculatePointsEarned(inv.grandTotal);
            c.loyaltyPoints += earned;
            c.totalSpent += inv.grandTotal;
            c.totalOrders += 1;
            c.lastVisit = QDate::currentDate();
            m_customerService->updateTier(c);
            m_customerService->updateCustomer(c);
        }
    }
}

void BillingService::logActivity(const QString& action, const QJsonObject& details) {
    LOG_INFO(QString("BillingService: %1").arg(action));
}

} // namespace RetailMS

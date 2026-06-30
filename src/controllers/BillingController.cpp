#include "BillingController.h"
#include "../services/BillingService.h"
#include "../services/CustomerService.h"
#include "../exceptions/AppException.h"

namespace RetailMS {

BillingController::BillingController(std::shared_ptr<BillingService> billingService, QObject* parent)
    : BaseController(parent), m_billingService(std::move(billingService)) {
    m_currentInvoice = m_billingService->createDraftInvoice();
}

Invoice BillingController::currentInvoice() const {
    return m_currentInvoice;
}

std::vector<Invoice> BillingController::getInvoices(const QDate& from, const QDate& to) const {
    return m_billingService->getInvoicesByDateRange(from, to);
}

void BillingController::createNewInvoice() {
    m_currentInvoice = m_billingService->createDraftInvoice();
    emit invoiceUpdated(m_currentInvoice);
}

void BillingController::holdCurrentBill() {
    if (!m_currentInvoice.items.empty()) {
        m_heldInvoices.push_back(m_currentInvoice);
        emit successMessage("Bill put on hold.");
        createNewInvoice();
    }
}

void BillingController::resumeBill(int index) {
    if (index >= 0 && index < static_cast<int>(m_heldInvoices.size())) {
        if (!m_currentInvoice.items.empty()) {
            m_heldInvoices.push_back(m_currentInvoice);
        }
        m_currentInvoice = m_heldInvoices[index];
        m_heldInvoices.erase(m_heldInvoices.begin() + index);
        emit invoiceUpdated(m_currentInvoice);
        emit successMessage("Bill resumed.");
    }
}

void BillingController::addProductToInvoice(int productId, double qty) {
    try {
        m_billingService->addItem(m_currentInvoice, productId, qty);
        emit invoiceUpdated(m_currentInvoice);
    } catch (const std::exception& e) {
        handleError("Add Product", e);
    }
}

void BillingController::addProductByBarcode(const QString& barcode, double qty) {
    try {
        m_billingService->addItemByBarcode(m_currentInvoice, barcode, qty);
        emit invoiceUpdated(m_currentInvoice);
    } catch (const std::exception& e) {
        handleError("Add Product By Barcode", e);
    }
}

void BillingController::updateItemQuantity(int index, double qty) {
    try {
        m_billingService->updateQuantity(m_currentInvoice, index, qty);
        emit invoiceUpdated(m_currentInvoice);
    } catch (const std::exception& e) {
        handleError("Update Quantity", e);
    }
}

void BillingController::removeItem(int index) {
    try {
        m_billingService->removeItem(m_currentInvoice, index);
        emit invoiceUpdated(m_currentInvoice);
    } catch (const std::exception& e) {
        handleError("Remove Item", e);
    }
}

void BillingController::applyDiscount(double pct) {
    m_billingService->applyDiscount(m_currentInvoice, pct);
    emit invoiceUpdated(m_currentInvoice);
}

void BillingController::applyCoupon(const QString& code) {
    if (m_billingService->applyCoupon(m_currentInvoice, code)) {
        emit successMessage("Coupon applied successfully");
        emit invoiceUpdated(m_currentInvoice);
    } else {
        emit errorOccurred("Invalid or expired coupon");
    }
}

void BillingController::removeCoupon() {
    m_billingService->removeCoupon(m_currentInvoice);
    emit invoiceUpdated(m_currentInvoice);
}

void BillingController::setCustomer(int customerId) {
    m_currentInvoice.customerId = customerId;
    if (customerId > 0) {
        auto optC = m_billingService->customerService()->getCustomerById(customerId);
        if (optC) {
            m_currentInvoice.customerName = optC->name;
            m_currentInvoice.customerPhone = optC->phone;
        }
    }
    emit invoiceUpdated(m_currentInvoice);
}

void BillingController::setCustomerPhone(const QString& phone) {
    m_currentInvoice.customerPhone = phone;
    // We do not clear customerId here, because it might be a valid walk-in/new logic flow where it is -1.
    emit invoiceUpdated(m_currentInvoice);
}

void BillingController::redeemLoyaltyPoints(int points) {
    try {
        m_billingService->redeemLoyaltyPoints(m_currentInvoice, points);
        emit invoiceUpdated(m_currentInvoice);
    } catch (const std::exception& e) {
        handleError("Redeem Loyalty Points", e);
    }
}

void BillingController::finalizeInvoice(double amountPaid, Invoice::PaymentMode mode, const QString& ref) {
    try {
        Invoice finalInv = m_billingService->finaliseInvoice(m_currentInvoice, amountPaid, mode, ref);
        emit checkoutComplete(finalInv);
        createNewInvoice();
    } catch (const std::exception& e) {
        handleError("Finalize Invoice", e);
    }
}

std::shared_ptr<CustomerService> BillingController::customerService() const {
    return m_billingService->customerService();
}

} // namespace RetailMS

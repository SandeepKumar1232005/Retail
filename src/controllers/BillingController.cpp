#include "BillingController.h"
#include "../services/BillingService.h"
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

void BillingController::addProductToInvoice(int productId, double qty) {
    try {
        m_billingService->addItem(m_currentInvoice, productId, qty);
        emit invoiceUpdated(m_currentInvoice);
    } catch (const std::exception& e) {
        handleError("Add Product", e);
    }
}

void BillingController::addProductByBarcode(const QString& barcode, double qty) {
    emit errorOccurred("Add by barcode not fully wired in controller yet");
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
    emit invoiceUpdated(m_currentInvoice);
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

} // namespace RetailMS

#include "CustomerService.h"
#include "../repository/CustomerRepository.h"

namespace RetailMS {

CustomerService::CustomerService(std::shared_ptr<CustomerRepository> repo, QObject* parent)
    : QObject(parent), m_repo(std::move(repo)) {}

std::optional<Customer> CustomerService::getCustomerById(int id) const {
    return m_repo->findById(id);
}

std::optional<Customer> CustomerService::getCustomerByPhone(const QString& phone) const {
    auto results = m_repo->findWhere("phone = ?", {phone});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

std::vector<Customer> CustomerService::searchCustomers(const QString& query) const {
    return m_repo->findWhere("name LIKE ? OR phone LIKE ?", 
                             {"%" + query + "%", "%" + query + "%"});
}

std::vector<Customer> CustomerService::getAllCustomers() const {
    return m_repo->findAll();
}

int CustomerService::saveCustomer(const Customer& customer) {
    if (!customer.phone.isEmpty()) {
        auto existing = getCustomerByPhone(customer.phone);
        if (existing) {
            Customer toUpdate = existing.value();
            toUpdate.name = customer.name;
            toUpdate.tier = customer.tier;
            // update other fields if needed, but for now just merging name and tier is enough
            updateCustomer(toUpdate);
            return toUpdate.id;
        }
    }
    return m_repo->save(customer);
}

bool CustomerService::updateCustomer(const Customer& customer) {
    return m_repo->update(customer);
}

bool CustomerService::deleteCustomer(int id) {
    return m_repo->remove(id);
}

int CustomerService::calculatePointsEarned(double grandTotal) const {
    // ₹100 spent = 1 Loyalty Point (configurable)
    double pointsPerHundred = 1.0;
    return static_cast<int>(grandTotal / 100.0 * pointsPerHundred);
}

double CustomerService::pointsToRupees(int points) const {
    // Tiered redemption: 100 pts = ₹50, 250 pts = ₹150, 500 pts = ₹400
    // For partial/exact redemptions, use the best applicable rate
    if (points >= 500) return (points / 500) * 400.0 + getRedemptionDiscount(points % 500);
    if (points >= 250) return (points / 250) * 150.0 + getRedemptionDiscount(points % 250);
    if (points >= 100) return (points / 100) * 50.0;
    return 0.0;
}

double CustomerService::getRedemptionDiscount(int points) const {
    if (points >= 500) return 400.0;
    if (points >= 250) return 150.0;
    if (points >= 100) return 50.0;
    return 0.0;
}

void CustomerService::updateTier(Customer& c) const {
    // Tier based on lifetime spending (totalSpent)
    if      (c.totalSpent >= 50000.0) c.tier = "platinum";
    else if (c.totalSpent >= 25000.0) c.tier = "gold";
    else if (c.totalSpent >= 10000.0) c.tier = "silver";
    else                              c.tier = "regular";
}

}

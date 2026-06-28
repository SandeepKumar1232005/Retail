#include "CustomerService.h"
#include "../repository/CustomerRepository.h"

namespace RetailMS {

CustomerService::CustomerService(std::shared_ptr<CustomerRepository> repo, QObject* parent)
    : QObject(parent), m_repo(std::move(repo)) {}

std::optional<Customer> CustomerService::getCustomerById(int id) const {
    return m_repo->findById(id);
}

std::vector<Customer> CustomerService::searchCustomers(const QString& query) const {
    return m_repo->findWhere("name LIKE ? OR phone LIKE ?", 
                             {"%" + query + "%", "%" + query + "%"});
}

int CustomerService::saveCustomer(const Customer& customer) {
    return m_repo->save(customer);
}

bool CustomerService::updateCustomer(const Customer& customer) {
    return m_repo->update(customer);
}

bool CustomerService::deleteCustomer(int id) {
    return m_repo->remove(id);
}

int CustomerService::calculatePointsEarned(double grandTotal) const {
    // Basic settings logic - ideally fetched from SettingsManager
    double pointsPerRupee = 0.1; 
    return static_cast<int>(grandTotal * pointsPerRupee);
}

double CustomerService::pointsToRupees(int points) const {
    double redemptionRate = 0.1;
    return points * redemptionRate;
}

void CustomerService::updateTier(Customer& c) const {
    if      (c.loyaltyPoints >= 20000) c.tier = "platinum";
    else if (c.loyaltyPoints >= 5000)  c.tier = "gold";
    else                               c.tier = "silver";
}

}

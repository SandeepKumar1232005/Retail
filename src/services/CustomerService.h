#pragma once
#include <QObject>
#include <memory>
#include <optional>
#include "../models/Customer.h"

namespace RetailMS {

class CustomerRepository;

class CustomerService : public QObject {
    Q_OBJECT
public:
    explicit CustomerService(std::shared_ptr<CustomerRepository> repo, QObject* parent = nullptr);

    std::optional<Customer> getCustomerById(int id) const;
    std::optional<Customer> getCustomerByPhone(const QString& phone) const;
    std::vector<Customer> searchCustomers(const QString& query) const;
    std::vector<Customer> getAllCustomers() const;
    
    int saveCustomer(const Customer& customer);
    bool updateCustomer(const Customer& customer);
    bool deleteCustomer(int id);

    int calculatePointsEarned(double grandTotal) const;
    double pointsToRupees(int points) const;
    double getRedemptionDiscount(int points) const;
    void updateTier(Customer& c) const;

private:
    std::shared_ptr<CustomerRepository> m_repo;
};

}

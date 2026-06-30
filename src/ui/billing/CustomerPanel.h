#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QTimer>

namespace RetailMS {

class CustomerService;

class CustomerPanel : public QWidget {
    Q_OBJECT
public:
    explicit CustomerPanel(std::shared_ptr<CustomerService> customerService, QWidget* parent = nullptr);

    void setCustomerService(std::shared_ptr<CustomerService> svc);

signals:
    void customerSelected(int customerId);
    void newCustomerPhoneEntered(const QString& phone);
    void redeemPointsRequested(int points);
    void focusProductSearchRequested();

public slots:
    void clear();
    void setCustomerDetails(const QString& name, const QString& phone, int loyaltyPoints, const QString& tier,
                            double totalSpent, int totalOrders, const QString& lastVisit);

private slots:
    void onPhoneTextChanged(const QString& text);
    void onRedeemPointsClicked();

private:
    void setupUi();

    std::shared_ptr<CustomerService> m_customerService;
    QTimer* m_searchDebounce;

    QLineEdit* m_phoneInput;
    QLabel* m_nameLabel;
    QLabel* m_tierLabel;
    QLabel* m_pointsLabel;
    QLabel* m_totalSpentLabel;
    QLabel* m_totalOrdersLabel;
    QLabel* m_lastVisitLabel;
    QLabel* m_welcomeLabel;
    QPushButton* m_redeemBtn;

    int m_currentCustomerId{-1};
    int m_currentLoyaltyPoints{0};
};

} // namespace RetailMS

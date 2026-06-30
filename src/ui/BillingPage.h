#pragma once
#include <QWidget>
#include <QShortcut>
#include <memory>
#include "billing/CustomerPanel.h"
#include "billing/ProductSelectionPanel.h"
#include "billing/CartPanel.h"
#include "billing/InvoiceSummaryPanel.h"
#include "billing/PaymentActionPanel.h"

namespace RetailMS {

class BillingController;

class BillingPage : public QWidget {
    Q_OBJECT
public:
    explicit BillingPage(std::shared_ptr<BillingController> controller, QWidget* parent = nullptr);

public slots:
    void loadProducts(const QString& query = "", const QString& category = "All Categories");

private:
    void setupUi();
    void setupConnections();
    void setupShortcuts();

    std::shared_ptr<BillingController> m_controller;

    CustomerPanel* m_customerPanel;
    ProductSelectionPanel* m_productPanel;
    CartPanel* m_cartPanel;
    InvoiceSummaryPanel* m_summaryPanel;
    PaymentActionPanel* m_actionPanel;
};

} // namespace RetailMS

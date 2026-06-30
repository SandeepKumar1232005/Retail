#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QToolButton>
#include "../../models/Invoice.h"

namespace RetailMS {

class PaymentActionPanel : public QWidget {
    Q_OBJECT
public:
    explicit PaymentActionPanel(QWidget* parent = nullptr);

signals:
    void generateInvoiceRequested(Invoice::PaymentMode mode);
    void holdBillRequested();
    void resumeBillRequested();
    void cancelBillRequested();
    void printRequested();
    void clearCartRequested();
    void previewRequested();

public slots:
    void refreshTotals(const Invoice& invoice);

private:
    void setupUi();
    
    Invoice::PaymentMode m_selectedPaymentMode = Invoice::PaymentMode::Cash;

    QLabel* m_subtotalLabel;
    QLabel* m_discountLabel;
    QLabel* m_gstLabel;
    QLabel* m_netAmountLabel;
    
    QButtonGroup* m_paymentModeGroup;
    
    QPushButton* m_generateBtn;
    QPushButton* m_printBtn;
    QPushButton* m_clearBtn;
    QToolButton* m_advancedMenuBtn;
};

} // namespace RetailMS

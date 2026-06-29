#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include "../models/Invoice.h"

namespace RetailMS {

class CheckoutSuccessDialog : public QDialog {
    Q_OBJECT

public:
    explicit CheckoutSuccessDialog(const Invoice& invoice, const QString& paymentModeStr, QWidget* parent = nullptr);
    ~CheckoutSuccessDialog() override = default;

signals:
    void printRequested();
    void newTransactionRequested();

private:
    void setupUi();

    Invoice m_invoice;
    QString m_paymentModeStr;
};

} // namespace RetailMS

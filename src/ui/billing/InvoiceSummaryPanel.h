#pragma once
#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include "../../models/Invoice.h"

namespace RetailMS {

class InvoiceSummaryPanel : public QDialog {
    Q_OBJECT
public:
    explicit InvoiceSummaryPanel(QWidget* parent = nullptr);

public slots:
    void refreshSummary(const Invoice& invoice);

private:
    void setupUi();

    QTextEdit* m_receiptText;
};

} // namespace RetailMS

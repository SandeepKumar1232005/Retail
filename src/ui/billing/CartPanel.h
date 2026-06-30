#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include "../../models/Invoice.h"

namespace RetailMS {

class CartPanel : public QWidget {
    Q_OBJECT
public:
    explicit CartPanel(QWidget* parent = nullptr);

signals:
    void quantityChanged(int index, double newQty);
    void itemRemoved(int index);

public slots:
    void refreshCart(const Invoice& invoice);

private:
    void setupUi();

    QTableWidget* m_cartTable;
    QLabel* m_emptyLabel;
};

} // namespace RetailMS

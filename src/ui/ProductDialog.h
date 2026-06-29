#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include "../models/Product.h"

namespace RetailMS {

class ProductDialog : public QDialog {
    Q_OBJECT
public:
    explicit ProductDialog(const Product* product = nullptr, QWidget* parent = nullptr);
    Product getProduct() const;

private:
    void setupUi();

    int m_id;

    QLineEdit* m_barcodeInput;
    QLineEdit* m_nameInput;
    QLineEdit* m_skuInput;
    QComboBox* m_categoryInput;
    QDoubleSpinBox* m_priceInput;
    QDoubleSpinBox* m_costPriceInput;
    QDoubleSpinBox* m_mrpInput;
    QSpinBox* m_stockInput;
};

} // namespace RetailMS

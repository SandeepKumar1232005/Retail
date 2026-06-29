#include "ProductDialog.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QUuid>

namespace RetailMS {

ProductDialog::ProductDialog(const Product* product, QWidget* parent) : QDialog(parent) {
    setWindowTitle(product ? "Edit Product" : "Add New Product");
    setMinimumWidth(350);
    setupUi();
    if (product) {
        m_id = product->id;
        m_barcodeInput->setText(product->barcode);
        m_nameInput->setText(product->name);
        m_skuInput->setText(product->sku);
        m_costPriceInput->setValue(product->costPrice);
        m_priceInput->setValue(product->sellingPrice);
        m_mrpInput->setValue(product->mrp);
        m_stockInput->setValue(static_cast<int>(product->stockQuantity));
    } else {
        m_id = -1;
    }
}

void ProductDialog::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout();
    
    m_barcodeInput = new QLineEdit(this);
    m_nameInput = new QLineEdit(this);
    m_skuInput = new QLineEdit(this);
    
    // Auto-generate a default SKU
    m_skuInput->setText("SKU-" + QUuid::createUuid().toString().mid(1, 8).toUpper());

    m_priceInput = new QDoubleSpinBox(this);
    m_priceInput->setRange(0.0, 1000000.0);
    m_priceInput->setDecimals(2);
    m_priceInput->setValue(0.0);

    m_costPriceInput = new QDoubleSpinBox(this);
    m_costPriceInput->setRange(0.0, 1000000.0);
    m_costPriceInput->setDecimals(2);
    m_costPriceInput->setValue(0.0);

    m_mrpInput = new QDoubleSpinBox(this);
    m_mrpInput->setRange(0.0, 1000000.0);
    m_mrpInput->setDecimals(2);
    m_mrpInput->setValue(0.0);

    m_stockInput = new QSpinBox(this);
    m_stockInput->setRange(0, 1000000);
    m_stockInput->setValue(0);

    formLayout->addRow("Barcode:", m_barcodeInput);
    formLayout->addRow("Name:", m_nameInput);
    formLayout->addRow("SKU:", m_skuInput);
    formLayout->addRow("Cost Price (₹):", m_costPriceInput);
    formLayout->addRow("Selling Price (₹):", m_priceInput);
    formLayout->addRow("MRP (₹):", m_mrpInput);
    formLayout->addRow("Initial Stock:", m_stockInput);

    mainLayout->addLayout(formLayout);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* saveBtn = new QPushButton("Save", this);
    saveBtn->setStyleSheet("background-color: #4CAF50; color: white;");
    QPushButton* cancelBtn = new QPushButton("Cancel", this);
    
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);

    connect(saveBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

Product ProductDialog::getProduct() const {
    Product p;
    p.id = m_id;
    p.barcode = m_barcodeInput->text();
    p.name = m_nameInput->text();
    p.sku = m_skuInput->text();
    p.costPrice = m_costPriceInput->value();
    p.sellingPrice = m_priceInput->value();
    p.mrp = m_mrpInput->value();
    p.stockQuantity = m_stockInput->value();
    p.isActive = true;
    return p;
}

} // namespace RetailMS

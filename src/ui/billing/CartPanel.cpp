#include "CartPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QFrame>

namespace RetailMS {

CartPanel::CartPanel(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void CartPanel::setupUi() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QFrame* frame = new QFrame(this);
    frame->setObjectName("cartCard");
    frame->setStyleSheet("QFrame#cartCard { background-color: #1E2025; border-radius: 8px; padding: 10px; }");
    QVBoxLayout* frameLayout = new QVBoxLayout(frame);

    QLabel* title = new QLabel("Shopping Cart", frame);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF; margin-bottom: 5px;");
    frameLayout->addWidget(title);

    m_cartTable = new QTableWidget(frame);
    m_cartTable->setColumnCount(7);
    m_cartTable->setHorizontalHeaderLabels({"Product", "Qty", "Price", "Discount", "GST", "Total", "Actions"});
    m_cartTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_cartTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_cartTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_cartTable->setShowGrid(false);
    
    m_cartTable->setStyleSheet(
        "QTableWidget { background-color: transparent; border: none; color: #FFFFFF; font-size: 14px; }"
        "QTableWidget::item { border-bottom: 1px solid #2A2D35; padding: 10px 5px; }"
        "QHeaderView::section { background-color: #2A2D35; color: #A9B1BC; border: none; padding: 12px 5px; font-weight: bold; text-align: left; }"
    );
    
    frameLayout->addWidget(m_cartTable);

    m_emptyLabel = new QLabel("🛒 Cart is empty. Search for products to add.", frame);
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setStyleSheet("color: #8C94A1; font-size: 16px; margin: 40px;");
    frameLayout->addWidget(m_emptyLabel);

    layout->addWidget(frame);
}

void CartPanel::refreshCart(const Invoice& invoice) {
    if (invoice.items.empty()) {
        m_cartTable->hide();
        m_emptyLabel->show();
        m_cartTable->setRowCount(0);
        return;
    }
    
    m_cartTable->show();
    m_emptyLabel->hide();

    m_cartTable->setRowCount(invoice.items.size());
    for (int i = 0; i < invoice.items.size(); ++i) {
        const auto& item = invoice.items[i];
        
        QTableWidgetItem* nameItem = new QTableWidgetItem(item.productName + "\n" + item.barcode);
        nameItem->setFont(QFont("Segoe UI", 11, QFont::Bold));
        m_cartTable->setItem(i, 0, nameItem);
        
        QDoubleSpinBox* qtyBox = new QDoubleSpinBox(m_cartTable);
        qtyBox->setRange(1, 9999);
        qtyBox->setValue(item.quantity);
        qtyBox->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        qtyBox->setStyleSheet(
            "QDoubleSpinBox { background-color: #2A2D35; color: white; border: 1px solid #374151; border-radius: 4px; padding: 8px; font-size: 14px; font-weight: bold; }"
            "QDoubleSpinBox::up-button, QDoubleSpinBox::down-button { width: 30px; }"
        );
        connect(qtyBox, &QDoubleSpinBox::valueChanged, this, [this, i](double v) {
            emit quantityChanged(i, v);
        });
        m_cartTable->setCellWidget(i, 1, qtyBox);
        
        m_cartTable->setItem(i, 2, new QTableWidgetItem(QString::number(item.sellingPrice, 'f', 2)));
        m_cartTable->setItem(i, 3, new QTableWidgetItem(QString::number(item.discountAmt, 'f', 2)));
        m_cartTable->setItem(i, 4, new QTableWidgetItem(QString::number(item.cgstAmt + item.sgstAmt, 'f', 2)));
        
        QTableWidgetItem* totalItem = new QTableWidgetItem(QString::number(item.total, 'f', 2));
        totalItem->setFont(QFont("Segoe UI", 12, QFont::Bold));
        totalItem->setForeground(QBrush(QColor("#4CAF50")));
        m_cartTable->setItem(i, 5, totalItem);
        
        QPushButton* removeBtn = new QPushButton("🗑️", m_cartTable);
        removeBtn->setCursor(Qt::PointingHandCursor);
        removeBtn->setStyleSheet(
            "QPushButton { background-color: transparent; color: #F44336; border: none; font-size: 18px; }"
            "QPushButton:hover { color: #D32F2F; }"
        );
        connect(removeBtn, &QPushButton::clicked, this, [this, i]() {
            emit itemRemoved(i);
        });
        m_cartTable->setCellWidget(i, 6, removeBtn);
    }
    m_cartTable->resizeRowsToContents();
}

} // namespace RetailMS

#include "StatCardWidget.h"
#include <QHBoxLayout>

namespace RetailMS {

StatCardWidget::StatCardWidget(const QString& title, const QString& value, const QString& percentage, QWidget* parent)
    : QWidget(parent) {
    
    this->setObjectName("statCard");
    this->setFixedHeight(140);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(8);
    
    m_titleLabel = new QLabel(title, this);
    m_titleLabel->setObjectName("cardTitle");
    layout->addWidget(m_titleLabel);
    
    m_valueLabel = new QLabel(value, this);
    m_valueLabel->setObjectName("cardValue");
    layout->addWidget(m_valueLabel);
    
    m_percentageLabel = new QLabel(percentage, this);
    m_percentageLabel->setObjectName("cardPercentage");
    
    // Default to success
    setPercentage(percentage, true);
    
    layout->addWidget(m_percentageLabel);
    layout->addStretch();
}

void StatCardWidget::setValue(const QString& value) {
    m_valueLabel->setText(value);
}

void StatCardWidget::setPercentage(const QString& percentage, bool isPositive) {
    m_percentageLabel->setText(percentage);
    if (isPositive) {
        m_percentageLabel->setStyleSheet("color: #22C55E; font-size: 13px; font-weight: 600;");
    } else {
        m_percentageLabel->setStyleSheet("color: #EF4444; font-size: 13px; font-weight: 600;");
    }
}

} // namespace RetailMS

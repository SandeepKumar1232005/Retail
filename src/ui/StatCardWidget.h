#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

namespace RetailMS {

class StatCardWidget : public QWidget {
    Q_OBJECT
public:
    explicit StatCardWidget(const QString& title, const QString& value, const QString& percentage, QWidget* parent = nullptr);

    void setValue(const QString& value);
    void setPercentage(const QString& percentage, bool isPositive);

private:
    QLabel* m_titleLabel;
    QLabel* m_valueLabel;
    QLabel* m_percentageLabel;
};

} // namespace RetailMS

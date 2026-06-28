#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace RetailMS {

class DashboardPage : public QWidget {
    Q_OBJECT
public:
    explicit DashboardPage(QWidget* parent = nullptr);

private:
    void setupUi();
    
    QLabel* m_todaySalesLabel;
    QLabel* m_totalOrdersLabel;
    QLabel* m_lowStockLabel;
};

}

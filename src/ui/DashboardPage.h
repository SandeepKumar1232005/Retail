#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include "StatCardWidget.h"

// Qt Charts
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPieSeries>

namespace RetailMS {

class DashboardPage : public QWidget {
    Q_OBJECT
public:
    explicit DashboardPage(QWidget* parent = nullptr);

public slots:
    void refreshData();

private slots:
    void handleAIQuery();

private:
    void setupUi();
    QWidget* createChartsSection();
    QWidget* createAIPanel();
    QWidget* createRecentTransactionsPanel();

    StatCardWidget* m_todayRevenueCard;
    StatCardWidget* m_totalOrdersCard;
    StatCardWidget* m_customersCard;
    StatCardWidget* m_inventoryValueCard;
    
    // AI Panel
    QTextEdit* m_chatArea;
    QLineEdit* m_chatInput;
    QPushButton* m_chatSendBtn;

    // Charts
    QChart* m_chart;
    QLineSeries* m_chartSeries;
    QValueAxis* m_axisX;
    QValueAxis* m_axisY;
};

} // namespace RetailMS

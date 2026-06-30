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
    void setupUi(); // Will delegate based on role
    void setupAdminUi();
    void setupStaffUi();
    void refreshAdminData();
    void refreshStaffData();
    QWidget* createChartsSection();
    QWidget* createAIPanel();
    QWidget* createRecentTransactionsPanel();

    StatCardWidget* m_todayRevenueCard{nullptr};
    StatCardWidget* m_totalOrdersCard{nullptr};
    StatCardWidget* m_customersCard{nullptr};
    StatCardWidget* m_inventoryValueCard{nullptr};
    StatCardWidget* m_overallSalesCard{nullptr};
    StatCardWidget* m_totalBillsCard{nullptr};
    StatCardWidget* m_avgBillValueCard{nullptr};
    StatCardWidget* m_outOfStockCard{nullptr};
    
    // Staff UI
    StatCardWidget* m_staffPersonalSalesCard{nullptr};
    StatCardWidget* m_staffBillsGeneratedCard{nullptr};
    StatCardWidget* m_staffShiftStatusCard{nullptr};
    
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

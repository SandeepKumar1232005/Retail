#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <memory>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include "../../controllers/BillingController.h"
#include "../StatCardWidget.h"

namespace RetailMS {

class StaffManagementPage : public QWidget {
    Q_OBJECT
public:
    explicit StaffManagementPage(std::shared_ptr<BillingController> billingController, QWidget* parent = nullptr);

public slots:
    void refreshData();

private slots:
    void onStaffSelected();
    void onInvoiceDoubleClicked(int row, int column);
    
private:
    void setupUi();
    void setupConnections();
    void updateSummaryCards();
    void loadStaffList();
    void loadStaffDetails(int userId);
    void loadStaffCharts(int userId);
    void loadOverviewCharts();
    void showInvoiceDetailDialog(const QString& invoiceNumber);
    
    // QtCharts helpers
    void createBillsPerDayChart(QChartView* chartView, int userId = -1);
    void createSalesPerDayChart(QChartView* chartView, int userId = -1);
    void createPaymentMethodChart(QChartView* chartView, int userId = -1);
    void createTopCustomersChart(QChartView* chartView, int userId = -1);

    std::shared_ptr<BillingController> m_billingController;

    // Summary Cards
    StatCardWidget* m_cardTotalStaff;
    StatCardWidget* m_cardActiveStaff;
    StatCardWidget* m_cardInactiveStaff;
    StatCardWidget* m_cardBillsToday;
    StatCardWidget* m_cardRevenueToday;

    // Filters
    QLineEdit* m_searchField;
    QComboBox* m_statusFilter;
    QComboBox* m_roleFilter;
    QComboBox* m_sortOrder;
    QPushButton* m_refreshButton;

    // Main Table
    QTableWidget* m_staffTable;

    // Detail Panel
    QStackedWidget* m_detailStack;
    
    // Overview Widget (Page 0)
    QWidget* m_overviewWidget;
    QChartView* m_overviewBillsChart;
    QChartView* m_overviewSalesChart;
    QChartView* m_overviewPaymentChart;
    QChartView* m_overviewCustomersChart;

    // Profile Detail Widget (Page 1)
    QWidget* m_profileWidget;
    QLabel* m_detailId;
    QLabel* m_detailName;
    QLabel* m_detailUsername;
    QLabel* m_detailEmail;
    QLabel* m_detailPhone;
    QLabel* m_detailRole;
    QLabel* m_detailStatus;
    QLabel* m_detailJoinDate;
    QLabel* m_detailLastLogin;
    QLabel* m_detailTotalBills;
    QLabel* m_detailTotalRevenue;
    QLabel* m_detailTodayRevenue;
    QLabel* m_detailAvgBill;
    
    // Detailed Billing History Table
    QTableWidget* m_billingHistoryTable;
    
    // Detailed Performance Charts
    QChartView* m_staffBillsChart;
    QChartView* m_staffSalesChart;
    QChartView* m_staffPaymentChart;
    QChartView* m_staffCustomersChart;

    int m_selectedUserId{-1};
};

} // namespace RetailMS

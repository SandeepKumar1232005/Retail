#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include <QComboBox>
#include <memory>
#include "ProductAutocompletePopup.h"

namespace RetailMS {

class BillingController;

class BillingPage : public QWidget {
    Q_OBJECT
public:
    explicit BillingPage(std::shared_ptr<BillingController> controller, QWidget* parent = nullptr);

public slots:
    void reloadProductGrid();
    void onCheckout();
    void onPrintInvoice();
    void onEmailInvoice();
    void onCategoryFilter(const QString& category);
    void onAddOnClicked(const QString& productName, double price);

private:
    void setupUi();
    QWidget* createLeftPane();
    QWidget* createCenterPane();
    QWidget* createRightPane();
    QWidget* createBottomRecommendationsPane();
    
    void setupConnections();
    void refreshTable();
    void refreshTotals();

    std::shared_ptr<BillingController> m_controller;

    // Left Pane (Grid)
    QLineEdit* m_productSearchInput;
    QGridLayout* m_productGrid;
    QWidget* m_gridWidget;
    QString m_selectedCategoryFilter;

    // Center Pane (Cart)
    QLineEdit* m_barcodeInput;
    QTableWidget* m_cartTable;
    
    // Right Pane (Invoice)
    QLabel* m_subtotalLabel;
    QLabel* m_discountLabel;
    QLabel* m_taxLabel;
    QLabel* m_totalLabel;
    QComboBox* m_paymentModeCombo;
    
    ProductAutocompletePopup* m_autocompletePopup{nullptr};
};

} // namespace RetailMS

#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QTimer>
#include <QEvent>
#include <QKeyEvent>
#include <QVector>

namespace RetailMS {

struct AutocompleteResult {
    int id;
    QString name;
    QString barcode;
    double sellingPrice;
    double stockQuantity;
    QString category;
};

class ProductAutocompletePopup : public QWidget {
    Q_OBJECT

public:
    explicit ProductAutocompletePopup(QLineEdit* parentLineEdit);
    ~ProductAutocompletePopup() override = default;

    void updatePosition();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void hideEvent(QHideEvent* event) override;

signals:
    void productSelected(int productId);

private slots:
    void onSearchTextChanged(const QString& text);
    void performSearch();
    void onItemSelected(int row, int column);

private:
    void setupUi();
    void populateResults(const QVector<AutocompleteResult>& results, const QString& searchText);
    QString highlightMatch(const QString& text, const QString& match);

    QLineEdit* m_parentLineEdit;
    QTableWidget* m_tableWidget;
    QTimer* m_debounceTimer;
    QString m_currentSearchText;
};

} // namespace RetailMS

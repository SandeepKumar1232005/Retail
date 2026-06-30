#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QListWidget>

namespace RetailMS {

class ProductSelectionPanel : public QWidget {
    Q_OBJECT
public:
    explicit ProductSelectionPanel(QWidget* parent = nullptr);

    void clearSearch();
    void setFocusToSearch();

signals:
    void productSelected(int productId);
    void searchRequested(const QString& query, const QString& category);

public slots:
    void updateProducts(const QList<QVariantMap>& products);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupUi();
    void showSuggestions();
    void hideSuggestions();
    void adjustPopupPosition();
    QWidget* createSuggestionItemWidget(const QVariantMap& p);

    QLineEdit* m_searchInput;
    QListWidget* m_suggestionList;
};

} // namespace RetailMS

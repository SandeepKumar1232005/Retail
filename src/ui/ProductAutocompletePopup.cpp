#include "ProductAutocompletePopup.h"
#include "../database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QAbstractItemView>

namespace RetailMS {

ProductAutocompletePopup::ProductAutocompletePopup(QLineEdit* parentLineEdit)
    : QWidget(parentLineEdit->window(), Qt::Popup | Qt::FramelessWindowHint),
      m_parentLineEdit(parentLineEdit) {
    
    setupUi();
    
    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(200); // 200ms debounce
    
    connect(m_debounceTimer, &QTimer::timeout, this, &ProductAutocompletePopup::performSearch);
    connect(m_parentLineEdit, &QLineEdit::textEdited, this, &ProductAutocompletePopup::onSearchTextChanged);
    
    m_parentLineEdit->installEventFilter(this);
}

void ProductAutocompletePopup::setupUi() {
    this->setFixedWidth(600);
    this->setMinimumHeight(100);
    this->setMaximumHeight(350);
    this->setStyleSheet(
        "QWidget { background-color: #1A1D24; border: 1px solid #3A3D45; border-radius: 6px; }"
        "QTableWidget { background-color: #1A1D24; color: #FFFFFF; border: none; gridline-color: #2A2D35; }"
        "QTableWidget::item { padding: 5px; border-bottom: 1px solid #2A2D35; }"
        "QTableWidget::item:selected { background-color: #2D3748; color: #FFFFFF; }"
        "QHeaderView::section { background-color: #2A2D35; color: #A9B1BC; padding: 4px; font-weight: bold; border: none; }"
    );
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(5);
    m_tableWidget->setHorizontalHeaderLabels({"Name", "Barcode", "Price", "Stock", "Category"});
    m_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setShowGrid(false);
    m_tableWidget->setFocusPolicy(Qt::NoFocus); // keep focus on line edit
    
    layout->addWidget(m_tableWidget);
    
    connect(m_tableWidget, &QTableWidget::cellClicked, this, &ProductAutocompletePopup::onItemSelected);
}

void ProductAutocompletePopup::updatePosition() {
    QPoint pos = m_parentLineEdit->mapToGlobal(QPoint(0, m_parentLineEdit->height()));
    this->move(pos);
}

void ProductAutocompletePopup::onSearchTextChanged(const QString& text) {
    m_currentSearchText = text.trimmed();
    if (m_currentSearchText.isEmpty()) {
        m_debounceTimer->stop();
        this->hide();
    } else {
        m_debounceTimer->start();
    }
}

void ProductAutocompletePopup::performSearch() {
    if (m_currentSearchText.isEmpty()) {
        this->hide();
        return;
    }
    
    auto& db = DatabaseManager::instance();
    if (!db.isConnected()) return;

    QString queryStr = 
        "SELECT p.id, p.name, p.barcode, p.selling_price, p.stock_quantity, c.name as category_name "
        "FROM products p "
        "LEFT JOIN categories c ON p.category_id = c.id "
        "WHERE p.name LIKE ? OR p.barcode LIKE ? OR p.sku LIKE ? "
        "LIMIT 20";
        
    QString likeVal = "%" + m_currentSearchText + "%";
    QSqlQuery query = db.prepare(queryStr);
    query.addBindValue(likeVal);
    query.addBindValue(likeVal);
    query.addBindValue(likeVal);
    
    if (!query.exec()) {
        qDebug() << "Autocomplete search failed:" << query.lastError().text();
        return;
    }
    
    QVector<AutocompleteResult> results;
    while (query.next()) {
        AutocompleteResult res;
        res.id = query.value("id").toInt();
        res.name = query.value("name").toString();
        res.barcode = query.value("barcode").toString();
        res.sellingPrice = query.value("selling_price").toDouble();
        res.stockQuantity = query.value("stock_quantity").toDouble();
        res.category = query.value("category_name").toString();
        if (res.category.isEmpty()) res.category = "None";
        results.push_back(res);
    }
    
    populateResults(results, m_currentSearchText);
}

QString ProductAutocompletePopup::highlightMatch(const QString& text, const QString& match) {
    if (match.isEmpty()) return text;
    QString result = text;
    // Simple case-insensitive highlight using HTML
    int index = result.indexOf(match, 0, Qt::CaseInsensitive);
    if (index >= 0) {
        QString matchedText = result.mid(index, match.length());
        result.replace(index, match.length(), QString("<span style='color: #4F46E5; font-weight: bold;'>%1</span>").arg(matchedText));
    }
    return result;
}

void ProductAutocompletePopup::populateResults(const QVector<AutocompleteResult>& results, const QString& searchText) {
    m_tableWidget->clearContents();
    m_tableWidget->setRowCount(results.isEmpty() ? 1 : results.size());
    
    if (results.isEmpty()) {
        QLabel* emptyLbl = new QLabel("No products found");
        emptyLbl->setAlignment(Qt::AlignCenter);
        emptyLbl->setStyleSheet("color: #A9B1BC; padding: 10px;");
        m_tableWidget->setSpan(0, 0, 1, 5);
        m_tableWidget->setCellWidget(0, 0, emptyLbl);
    } else {
        for (int row = 0; row < results.size(); ++row) {
            const auto& res = results[row];
            
            QLabel* nameLbl = new QLabel(highlightMatch(res.name, searchText));
            nameLbl->setStyleSheet("background: transparent; color: white; padding-left: 5px;");
            
            QTableWidgetItem* barcodeItem = new QTableWidgetItem(res.barcode);
            QTableWidgetItem* priceItem = new QTableWidgetItem(QString("₹%1").arg(res.sellingPrice, 0, 'f', 2));
            QTableWidgetItem* stockItem = new QTableWidgetItem(QString::number(res.stockQuantity));
            QTableWidgetItem* categoryItem = new QTableWidgetItem(res.category);
            
            // store ID in UserRole of column 0
            nameLbl->setProperty("productId", res.id);
            barcodeItem->setData(Qt::UserRole, res.id);
            
            m_tableWidget->setCellWidget(row, 0, nameLbl);
            m_tableWidget->setItem(row, 1, barcodeItem);
            m_tableWidget->setItem(row, 2, priceItem);
            m_tableWidget->setItem(row, 3, stockItem);
            m_tableWidget->setItem(row, 4, categoryItem);
            
            // Make stock red if <= 5
            if (res.stockQuantity <= 5) {
                stockItem->setForeground(QColor("#EF4444"));
            }
        }
        
        m_tableWidget->selectRow(0); // Select first item by default
    }
    
    updatePosition();
    this->show();
}

void ProductAutocompletePopup::onItemSelected(int row, int column) {
    if (m_tableWidget->rowCount() > 0 && m_tableWidget->item(row, 1)) {
        int productId = m_tableWidget->item(row, 1)->data(Qt::UserRole).toInt();
        this->hide();
        m_parentLineEdit->clear();
        emit productSelected(productId);
    }
}

bool ProductAutocompletePopup::eventFilter(QObject* obj, QEvent* event) {
    if (obj == m_parentLineEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        
        if (this->isVisible()) {
            int currentRow = m_tableWidget->currentRow();
            int maxRow = m_tableWidget->rowCount() - 1;
            
            switch (keyEvent->key()) {
                case Qt::Key_Up:
                    if (currentRow > 0) m_tableWidget->selectRow(currentRow - 1);
                    return true;
                case Qt::Key_Down:
                    if (currentRow < maxRow) m_tableWidget->selectRow(currentRow + 1);
                    return true;
                case Qt::Key_Enter:
                case Qt::Key_Return:
                    if (currentRow >= 0) {
                        onItemSelected(currentRow, 0);
                        return true;
                    }
                    break;
                case Qt::Key_Escape:
                    this->hide();
                    return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void ProductAutocompletePopup::hideEvent(QHideEvent* event) {
    m_debounceTimer->stop();
    QWidget::hideEvent(event);
}

} // namespace RetailMS

#include "ProductSelectionPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QEvent>
#include <QKeyEvent>
#include <QTimer>

namespace RetailMS {

ProductSelectionPanel::ProductSelectionPanel(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void ProductSelectionPanel::setupUi() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Search by Product Name, Barcode, Category or Brand...");
    m_searchInput->setStyleSheet(
        "QLineEdit { "
        "   background-color: #1E2025; "
        "   border: 1px solid #2A2D35; "
        "   border-radius: 8px; "
        "   padding: 12px; "
        "   font-size: 16px; "
        "   color: #FFFFFF; "
        "}"
        "QLineEdit:focus { border: 1px solid #4CAF50; }"
    );
    layout->addWidget(m_searchInput);

    m_suggestionList = new QListWidget();
    m_suggestionList->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    m_suggestionList->setAttribute(Qt::WA_ShowWithoutActivating);
    m_suggestionList->setFocusPolicy(Qt::NoFocus);
    m_suggestionList->setStyleSheet(
        "QListWidget { "
        "   background-color: #1E2025; "
        "   border: 1px solid #2A2D35; "
        "   border-radius: 8px; "
        "   color: #FFFFFF; "
        "   outline: none; "
        "}"
        "QListWidget::item { border-bottom: 1px solid #2A2D35; }"
        "QListWidget::item:selected { background-color: #2A2D35; }"
        "QListWidget::item:hover { background-color: #2A2D35; }"
    );
    m_suggestionList->hide();

    // Event filter for keyboard navigation
    m_searchInput->installEventFilter(this);
    
    // Window movement handling to adjust popup
    if (parentWidget()) {
        parentWidget()->installEventFilter(this);
    }

    connect(m_searchInput, &QLineEdit::textChanged, this, [this](const QString& text) {
        if (text.trimmed().isEmpty()) {
            hideSuggestions();
            return;
        }
        // Emit search request, "All Categories" is passed to match the signal signature
        emit searchRequested(text.trimmed(), "All Categories");
    });
    
    connect(m_suggestionList, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        int id = item->data(Qt::UserRole).toInt();
        emit productSelected(id);
        clearSearch();
    });

    // Handle pressing Enter directly in the search input
    connect(m_searchInput, &QLineEdit::returnPressed, this, [this]() {
        if (m_suggestionList->isVisible()) {
            QListWidgetItem* item = m_suggestionList->currentItem();
            if (item) {
                int id = item->data(Qt::UserRole).toInt();
                emit productSelected(id);
                clearSearch();
            }
        }
    });
}

QWidget* ProductSelectionPanel::createSuggestionItemWidget(const QVariantMap& p) {
    QString query = m_searchInput->text().trimmed();
    
    auto highlight = [&query](QString text) {
        if (query.isEmpty() || text.isEmpty()) return text.toHtmlEscaped();
        QString escaped = text.toHtmlEscaped();
        QRegularExpression regex("(" + QRegularExpression::escape(query) + ")", QRegularExpression::CaseInsensitiveOption);
        escaped.replace(regex, "<b style='color:#FFEB3B;'>\\1</b>");
        return escaped;
    };

    QWidget* w = new QWidget();
    w->setMinimumHeight(40);
    QVBoxLayout* mainLyt = new QVBoxLayout(w);
    mainLyt->setContentsMargins(10, 5, 10, 5);
    mainLyt->setSpacing(2);

    QHBoxLayout* topLyt = new QHBoxLayout();
    QLabel* nameLbl = new QLabel(highlight(p["name"].toString()));
    nameLbl->setStyleSheet("font-size: 13px; font-weight: normal; color: #FFFFFF;");
    
    QLabel* priceLbl = new QLabel(QString("\u20B9%1").arg(p["price"].toDouble(), 0, 'f', 2));
    priceLbl->setStyleSheet("font-size: 13px; font-weight: bold; color: #4CAF50;");
    
    QLabel* stockLbl = new QLabel(QString("Stock: %1").arg(p["stock"].toDouble()));
    stockLbl->setStyleSheet("color: #8C94A1; font-size: 11px; margin-left: 10px;");
    
    topLyt->addWidget(nameLbl);
    topLyt->addStretch();
    topLyt->addWidget(stockLbl);
    topLyt->addWidget(priceLbl);
    
    mainLyt->addLayout(topLyt);
    
    return w;
}

void ProductSelectionPanel::updateProducts(const QList<QVariantMap>& products) {
    m_suggestionList->clear();
    
    if (products.isEmpty() || m_searchInput->text().trimmed().isEmpty()) {
        hideSuggestions();
        return;
    }

    for (const auto& p : products) {
        QListWidgetItem* item = new QListWidgetItem(m_suggestionList);
        item->setData(Qt::UserRole, p["id"].toInt());
        
        QWidget* w = createSuggestionItemWidget(p);
        item->setSizeHint(w->sizeHint());
        
        m_suggestionList->addItem(item);
        m_suggestionList->setItemWidget(item, w);
    }
    
    if (m_suggestionList->count() > 0) {
        m_suggestionList->setCurrentRow(0);
        showSuggestions();
    }
}

void ProductSelectionPanel::adjustPopupPosition() {
    QPoint pos = m_searchInput->mapToGlobal(QPoint(0, m_searchInput->height() + 2));
    m_suggestionList->setFixedWidth(m_searchInput->width());
    
    // Limit visible items to 6
    int itemHeight = 35; // approximate height of compact item
    if (m_suggestionList->count() > 0) {
        QListWidgetItem* firstItem = m_suggestionList->item(0);
        QWidget* w = m_suggestionList->itemWidget(firstItem);
        if (w) itemHeight = w->sizeHint().height();
    }
    
    int maxItems = std::min(m_suggestionList->count(), 6);
    m_suggestionList->setFixedHeight(maxItems * itemHeight + 2); // +2 for border
    
    m_suggestionList->move(pos);
}

void ProductSelectionPanel::showSuggestions() {
    adjustPopupPosition();
    m_suggestionList->show();
    m_suggestionList->raise();
}

void ProductSelectionPanel::hideSuggestions() {
    m_suggestionList->hide();
}

void ProductSelectionPanel::clearSearch() {
    m_searchInput->clear();
    hideSuggestions();
    setFocusToSearch();
}

void ProductSelectionPanel::setFocusToSearch() {
    m_searchInput->setFocus();
}

bool ProductSelectionPanel::eventFilter(QObject* obj, QEvent* event) {
    if (obj == m_searchInput && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (m_suggestionList->isVisible()) {
            if (keyEvent->key() == Qt::Key_Down) {
                int row = m_suggestionList->currentRow();
                if (row < m_suggestionList->count() - 1) {
                    m_suggestionList->setCurrentRow(row + 1);
                }
                return true;
            } else if (keyEvent->key() == Qt::Key_Up) {
                int row = m_suggestionList->currentRow();
                if (row > 0) {
                    m_suggestionList->setCurrentRow(row - 1);
                }
                return true;
            } else if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
                QListWidgetItem* item = m_suggestionList->currentItem();
                if (item) {
                    int id = item->data(Qt::UserRole).toInt();
                    emit productSelected(id);
                    clearSearch();
                }
                return true;
            } else if (keyEvent->key() == Qt::Key_Escape) {
                hideSuggestions();
                return true;
            }
        }
    } else if (obj == m_searchInput && event->type() == QEvent::FocusOut) {
        // Use a small delay to allow itemClicked to process if they clicked the popup
        QTimer::singleShot(100, this, &ProductSelectionPanel::hideSuggestions);
    } else if (event->type() == QEvent::Move || event->type() == QEvent::Resize) {
        if (m_suggestionList->isVisible()) {
            adjustPopupPosition();
        }
    }
    return QWidget::eventFilter(obj, event);
}

} // namespace RetailMS

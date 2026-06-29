#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QLabel>

namespace RetailMS {

class SidebarWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int sidebarWidth READ sidebarWidth WRITE setSidebarWidth)
public:
    explicit SidebarWidget(QWidget* parent = nullptr);

    int sidebarWidth() const { return this->width(); }
    void setSidebarWidth(int w) { this->setFixedWidth(w); }
    
    QString getPageName(int index) const;

signals:
    void pageSelected(int index);

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void setupUi();
    QPushButton* createNavButton(const QString& text, const QString& icon, int pageIndex);
    void toggleCollapse(bool collapse);

    QVBoxLayout* m_layout;
    QList<QPushButton*> m_navButtons;
    QLabel* m_logoLabel;
    
    QPropertyAnimation* m_animation;
    bool m_isCollapsed = false;
};

} // namespace RetailMS

#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

namespace RetailMS {

class SidebarWidget : public QWidget {
    Q_OBJECT
public:
    explicit SidebarWidget(QWidget* parent = nullptr);

signals:
    void pageSelected(int index);

private:
    void setupUi();
    QPushButton* createNavButton(const QString& text, const QString& iconPath, int pageIndex);

    QVBoxLayout* m_layout;
};

} // namespace RetailMS

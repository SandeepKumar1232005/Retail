#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <memory>

namespace RetailMS {

class ProductController;

class ProductPage : public QWidget {
    Q_OBJECT
public:
    explicit ProductPage(std::shared_ptr<ProductController> controller, QWidget* parent = nullptr);

private:
    void setupUi();
    void setupConnections();
    void refreshTable();

    std::shared_ptr<ProductController> m_controller;

    QLineEdit* m_searchInput;
    QTableWidget* m_productTable;
};

}

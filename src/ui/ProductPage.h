#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <memory>

namespace RetailMS {

class ProductController;

class ProductPage : public QWidget {
    Q_OBJECT
public:
    explicit ProductPage(std::shared_ptr<ProductController> controller, QWidget* parent = nullptr);

private slots:
    void onImportCsv();
    void onExportCsv();

private:
    void setupUi();
    void setupConnections();
    void refreshTable();

    std::shared_ptr<ProductController> m_controller;

    QLineEdit* m_searchInput;
    QComboBox* m_categoryFilter;
    QTableWidget* m_productTable;
    QPushButton* m_addProductButton;
    QPushButton* m_exportButton;
    QPushButton* m_importButton;
};

} // namespace RetailMS

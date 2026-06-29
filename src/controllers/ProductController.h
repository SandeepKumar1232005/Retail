#pragma once
#include "BaseController.h"
#include <memory>
#include <vector>
#include <optional>
#include "../models/Product.h"

namespace RetailMS {

class ProductService;

class ProductController : public BaseController {
    Q_OBJECT
public:
    explicit ProductController(std::shared_ptr<ProductService> productService, QObject* parent = nullptr);

    std::vector<Product> searchProducts(const QString& query) const;
    std::optional<Product> getProductByBarcode(const QString& barcode) const;
    
public slots:
    void saveProduct(Product p, bool notify = true);
    void deleteProduct(int id);
    void notifyProductsUpdated();

signals:
    void productListChanged();

private:
    std::shared_ptr<ProductService> m_productService;
};

}

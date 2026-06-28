#pragma once
#include "BaseController.h"
#include <memory>
#include <vector>
#include "../models/Product.h"

namespace RetailMS {

class ProductService;

class ProductController : public BaseController {
    Q_OBJECT
public:
    explicit ProductController(std::shared_ptr<ProductService> productService, QObject* parent = nullptr);

    std::vector<Product> searchProducts(const QString& query) const;
    
public slots:
    void saveProduct(const Product& p);
    void deleteProduct(int id);

signals:
    void productListChanged();

private:
    std::shared_ptr<ProductService> m_productService;
};

}

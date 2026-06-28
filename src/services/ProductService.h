#pragma once
#include <QObject>
#include <memory>
#include <vector>
#include <optional>
#include "../models/Product.h"

namespace RetailMS {

class ProductRepository;

class ProductService : public QObject {
    Q_OBJECT
public:
    explicit ProductService(std::shared_ptr<ProductRepository> repo, QObject* parent = nullptr);

    std::optional<Product> getProductById(int id) const;
    std::optional<Product> getProductByBarcode(const QString& barcode) const;
    std::vector<Product> searchProducts(const QString& query) const;
    
    int saveProduct(const Product& product);
    bool updateProduct(const Product& product);
    bool deleteProduct(int id);

private:
    std::shared_ptr<ProductRepository> m_repo;
};

} // namespace RetailMS

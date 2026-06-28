#include "ProductService.h"
#include "../repository/ProductRepository.h"
#include "../exceptions/AppException.h"

namespace RetailMS {

ProductService::ProductService(std::shared_ptr<ProductRepository> repo, QObject* parent)
    : QObject(parent), m_repo(std::move(repo)) {}

std::optional<Product> ProductService::getProductById(int id) const {
    return m_repo->findById(id);
}

std::optional<Product> ProductService::getProductByBarcode(const QString& barcode) const {
    auto results = m_repo->findWhere("barcode = ?", {barcode});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

std::vector<Product> ProductService::searchProducts(const QString& query) const {
    return m_repo->findWhere("name LIKE ? OR barcode LIKE ? OR sku LIKE ?", 
                             {"%" + query + "%", "%" + query + "%", "%" + query + "%"});
}

int ProductService::saveProduct(const Product& product) {
    if (product.name.isEmpty()) {
        throw ValidationException("Product name cannot be empty", "ERR_VALIDATION");
    }
    return m_repo->save(product);
}

bool ProductService::updateProduct(const Product& product) {
    return m_repo->update(product);
}

bool ProductService::deleteProduct(int id) {
    return m_repo->remove(id);
}

} // namespace RetailMS

#include "ProductController.h"
#include "../services/ProductService.h"
#include "../exceptions/AppException.h"

namespace RetailMS {

ProductController::ProductController(std::shared_ptr<ProductService> productService, QObject* parent)
    : BaseController(parent), m_productService(std::move(productService)) {}

std::vector<Product> ProductController::searchProducts(const QString& query) const {
    return m_productService->searchProducts(query);
}

void ProductController::saveProduct(const Product& p) {
    try {
        if (p.id < 0) {
            m_productService->saveProduct(p);
        } else {
            m_productService->updateProduct(p);
        }
        emit successMessage("Product saved successfully");
        emit productListChanged();
    } catch (const std::exception& e) {
        handleError("Save Product", e);
    }
}

void ProductController::deleteProduct(int id) {
    try {
        if (m_productService->deleteProduct(id)) {
            emit successMessage("Product deleted successfully");
            emit productListChanged();
        } else {
            emit errorOccurred("Failed to delete product");
        }
    } catch (const std::exception& e) {
        handleError("Delete Product", e);
    }
}

}

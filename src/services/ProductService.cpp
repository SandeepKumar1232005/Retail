#include "ProductService.h"
#include "../repository/ProductRepository.h"
#include "../exceptions/AppException.h"
#include "../services/SessionManager.h"
#include "../database/DatabaseManager.h"

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
    if (query.trimmed().isEmpty()) {
        return m_repo->findAll();
    }
    return m_repo->findWhere("name LIKE ? OR barcode LIKE ? OR sku LIKE ?", 
                             {"%" + query + "%", "%" + query + "%", "%" + query + "%"});
}

int ProductService::saveProduct(Product& product) {
    if (SessionManager::instance().isLoggedIn() && !SessionManager::instance().currentUser().isAdmin()) {
        throw AuthException("Unauthorized: Admin access required to save products.");
    }
    if (product.name.isEmpty()) {
        throw ValidationException("Product name cannot be empty", "ERR_VALIDATION");
    }
    
    product.categoryId = resolveCategoryId(product);
    
    int result = m_repo->save(product);
    if (result < 0) {
        throw AppException("Failed to save product to database. Check for duplicate barcode or SKU.", "ERR_DB_INSERT");
    }
    return result;
}

bool ProductService::updateProduct(Product& product) {
    if (SessionManager::instance().isLoggedIn() && !SessionManager::instance().currentUser().isAdmin()) {
        throw AuthException("Unauthorized: Admin access required to update products.");
    }
    
    product.categoryId = resolveCategoryId(product);
    
    bool ok = m_repo->update(product);
    if (!ok) {
        throw AppException("Failed to update product in database.", "ERR_DB_UPDATE");
    }
    return ok;
}

bool ProductService::deleteProduct(int id) {
    if (SessionManager::instance().isLoggedIn() && !SessionManager::instance().currentUser().isAdmin()) {
        throw AuthException("Unauthorized: Admin access required to delete products.");
    }
    return m_repo->remove(id);
}

int ProductService::resolveCategoryId(Product& product) {
    auto& db = DatabaseManager::instance();
    QString categoryName = product.categoryName.trimmed();
    
    if (categoryName.isEmpty()) {
        QString lowerName = product.name.toLower();
        if (lowerName.contains("apple") || lowerName.contains("banana") || lowerName.contains("orange") || lowerName.contains("mango") || lowerName.contains("fruit") || lowerName.contains("grape")) {
            categoryName = "Fruits";
        } else if (lowerName.contains("milk") || lowerName.contains("cheese") || lowerName.contains("butter") || lowerName.contains("paneer") || lowerName.contains("yogurt") || lowerName.contains("curd") || lowerName.contains("dairy")) {
            categoryName = "Dairy";
        } else if (lowerName.contains("carrot") || lowerName.contains("potato") || lowerName.contains("onion") || lowerName.contains("tomato") || lowerName.contains("cabbage") || lowerName.contains("broccoli") || lowerName.contains("veg")) {
            categoryName = "Vegetables";
        } else if (lowerName.contains("bread") || lowerName.contains("cake") || lowerName.contains("bun") || lowerName.contains("muffin") || lowerName.contains("biscuit") || lowerName.contains("cookie") || lowerName.contains("bakery") || lowerName.contains("rusk")) {
            categoryName = "Bakery";
        } else if (lowerName.contains("juice") || lowerName.contains("cola") || lowerName.contains("water") || lowerName.contains("soda") || lowerName.contains("tea") || lowerName.contains("coffee") || lowerName.contains("beverage") || lowerName.contains("drink")) {
            categoryName = "Beverages";
        } else if (lowerName.contains("chips") || lowerName.contains("snack") || lowerName.contains("popcorn") || lowerName.contains("kurkure") || lowerName.contains("lays") || lowerName.contains("doritos") || lowerName.contains("mixture") || lowerName.contains("bhujia")) {
            categoryName = "Snacks";
        } else if ((lowerName.contains("soap") || lowerName.contains("shampoo") || lowerName.contains("lotion") || lowerName.contains("cream") || lowerName.contains("deo") || lowerName.contains("perfume") || lowerName.contains("paste") || lowerName.contains("brush") || lowerName.contains("wash")) && !lowerName.contains("dish")) {
            categoryName = "Personal Care";
        } else if (lowerName.contains("clean") || lowerName.contains("detergent") || lowerName.contains("dish wash") || lowerName.contains("phenyl") || lowerName.contains("harpic") || lowerName.contains("vim") || lowerName.contains("surf")) {
            categoryName = "Cleaning Supplies";
        } else if (lowerName.contains("ice cream") || lowerName.contains("frozen") || lowerName.contains("peas") || lowerName.contains("nugget")) {
            categoryName = "Frozen Foods";
        } else if (lowerName.contains("diaper") || lowerName.contains("baby") || lowerName.contains("wipes") || lowerName.contains("cerelac") || lowerName.contains("pampers")) {
            categoryName = "Baby Care";
        } else if (lowerName.contains("dog") || lowerName.contains("cat") || lowerName.contains("pet") || lowerName.contains("pedigree") || lowerName.contains("whiskas")) {
            categoryName = "Pet Care";
        } else if (lowerName.contains("pen") || lowerName.contains("pencil") || lowerName.contains("book") || lowerName.contains("paper") || lowerName.contains("eraser") || lowerName.contains("stationery") || lowerName.contains("notebook")) {
            categoryName = "Stationery";
        } else if (lowerName.contains("phone") || lowerName.contains("cable") || lowerName.contains("charger") || lowerName.contains("battery") || lowerName.contains("electronic") || lowerName.contains("bulb")) {
            categoryName = "Electronics";
        } else if (lowerName.contains("mop") || lowerName.contains("broom") || lowerName.contains("bucket") || lowerName.contains("towel") || lowerName.contains("tissue")) {
            categoryName = "Household";
        }
        
        if (categoryName.isEmpty()) {
            categoryName = "Groceries";
        }
    }

    auto catRes = db.executeScalar("SELECT id FROM categories WHERE LOWER(TRIM(name)) = LOWER(?) LIMIT 1;", { categoryName });
    if (catRes && !catRes->isNull()) {
        return catRes->toInt();
    }
    
    db.executeNonQuery("INSERT INTO categories (name, description) VALUES (?, ?);", { categoryName, "Dynamically created category" });
    auto newCatRes = db.executeScalar("SELECT id FROM categories WHERE LOWER(TRIM(name)) = LOWER(?) LIMIT 1;", { categoryName });
    if (newCatRes && !newCatRes->isNull()) {
        return newCatRes->toInt();
    }
    
    return product.categoryId > 0 ? product.categoryId : 1;
}

} // namespace RetailMS

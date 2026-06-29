#pragma once
class QSqlQuery;

#include <vector>
#include <optional>
#include <QString>
#include <QVariantList>

namespace RetailMS {

template<typename T>
class IRepository {
public:
    virtual ~IRepository() = default;
    
    virtual std::optional<T> findById(int id) const = 0;
    virtual std::vector<T> findAll() const = 0;
    virtual std::vector<T> findWhere(const QString& condition, const QVariantList& params = {}) const = 0;
    
    virtual int save(const T& entity) = 0;    // insert or update, returns id if insert
    virtual bool update(const T& entity) = 0;
    virtual bool remove(int id) = 0;
    
    virtual int count() const = 0;
    virtual bool exists(int id) const = 0;
};

} // namespace RetailMS

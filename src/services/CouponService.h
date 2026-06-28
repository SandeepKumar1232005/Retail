#pragma once
#include <QObject>
#include <memory>
#include <optional>
#include "../models/Coupon.h"

namespace RetailMS {

class CouponRepository;

class CouponService : public QObject {
    Q_OBJECT
public:
    explicit CouponService(std::shared_ptr<CouponRepository> repo, QObject* parent = nullptr);

    std::optional<Coupon> getCouponByCode(const QString& code) const;
    bool validateCoupon(const QString& code, double orderValue, double& outDiscount) const;
    void recordCouponUsage(const QString& code);

private:
    std::shared_ptr<CouponRepository> m_repo;
};

}

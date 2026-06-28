#include "CouponService.h"
#include "../repository/CouponRepository.h"

namespace RetailMS {

CouponService::CouponService(std::shared_ptr<CouponRepository> repo, QObject* parent)
    : QObject(parent), m_repo(std::move(repo)) {}

std::optional<Coupon> CouponService::getCouponByCode(const QString& code) const {
    auto results = m_repo->findWhere("code = ?", {code});
    if (!results.empty()) return results.front();
    return std::nullopt;
}

bool CouponService::validateCoupon(const QString& code, double orderValue, double& outDiscount) const {
    outDiscount = 0.0;
    auto optCoupon = getCouponByCode(code);
    if (!optCoupon) return false;
    
    Coupon c = optCoupon.value();
    if (!c.isValid(orderValue)) return false;
    
    outDiscount = c.calculateDiscount(orderValue);
    return true;
}

void CouponService::recordCouponUsage(const QString& code) {
    auto optCoupon = getCouponByCode(code);
    if (optCoupon) {
        Coupon c = optCoupon.value();
        c.usedCount++;
        m_repo->update(c);
    }
}

}

#include "AuthService.h"
#include "../repository/UserRepository.h"
#include "SessionManager.h"
#include "../utils/CryptoUtils.h"
#include "../utils/Logger.h"
#include <QDateTime>

namespace RetailMS {

AuthService::AuthService(std::shared_ptr<UserRepository> userRepo)
    : m_userRepo(std::move(userRepo)) {
}

std::optional<User> AuthService::login(const QString& username, const QString& password) {
    auto users = m_userRepo->findWhere("username = ?", {username});
    if (users.empty()) {
        LOG_WARN("Login failed: user not found", username);
        return std::nullopt;
    }
    
    User user = users.front();
    if (!user.isActive) {
        LOG_WARN("Login failed: user inactive", username);
        return std::nullopt;
    }
    
    if (CryptoUtils::verifyPassword(password, user.salt, user.passwordHash)) {
        user.lastLogin = QDateTime::currentDateTime();
        m_userRepo->update(user);
        SessionManager::instance().setCurrentUser(user);
        LOG_INFO("User logged in successfully", username);
        return user;
    }
    
    LOG_WARN("Login failed: incorrect password", username);
    return std::nullopt;
}

void AuthService::logout() {
    if (SessionManager::instance().isLoggedIn()) {
        LOG_INFO("User logged out", SessionManager::instance().currentUser().username);
        SessionManager::instance().clear();
    }
}

bool AuthService::isLoggedIn() const {
    return SessionManager::instance().isLoggedIn();
}

const User& AuthService::currentUser() const {
    return SessionManager::instance().currentUser();
}

bool AuthService::hasPermission(const QString& permission) const {
    if (!isLoggedIn()) return false;
    UserRole role = currentUser().role;
    if (role == UserRole::Admin) return true;
    
    // Example basic RBAC
    if (role == UserRole::Manager) {
        if (permission.startsWith("DELETE_")) return false;
        return true;
    }
    
    if (role == UserRole::Cashier) {
        if (permission == "CREATE_INVOICE" || permission == "VIEW_PRODUCTS") return true;
        return false;
    }
    
    return false;
}

bool AuthService::changePassword(int userId, const QString& oldPwd, const QString& newPwd) {
    auto optUser = m_userRepo->findById(userId);
    if (!optUser) return false;
    
    User user = optUser.value();
    if (!CryptoUtils::verifyPassword(oldPwd, user.salt, user.passwordHash)) {
        return false;
    }
    
    user.salt = CryptoUtils::generateSalt();
    user.passwordHash = CryptoUtils::hashPassword(newPwd, user.salt);
    return m_userRepo->update(user);
}

} // namespace RetailMS

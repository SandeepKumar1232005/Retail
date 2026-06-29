#pragma once
#include <memory>
#include <optional>
#include <QString>
#include "../models/User.h"

namespace RetailMS {

class UserRepository;
class SessionManager;

class AuthService {
public:
    explicit AuthService(std::shared_ptr<UserRepository> userRepo);

    std::optional<User> login(const QString& username, const QString& password);
    void                logout();
    bool                isLoggedIn() const;
    const User&         currentUser() const;
    bool                hasPermission(const QString& permission) const;
    bool                changePassword(int userId, const QString& oldPwd, const QString& newPwd);

private:
    std::shared_ptr<UserRepository> m_userRepo;
};

} // namespace RetailMS

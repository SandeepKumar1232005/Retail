#pragma once
#include "../models/User.h"
#include <optional>

namespace RetailMS {
class SessionManager {
public:
    static SessionManager& instance() {
        static SessionManager inst;
        return inst;
    }
    
    void setCurrentUser(const User& user) { m_currentUser = user; }
    void clear() { m_currentUser = std::nullopt; }
    bool isLoggedIn() const { return m_currentUser.has_value(); }
    const User& currentUser() const { return m_currentUser.value(); }
    
private:
    SessionManager() = default;
    std::optional<User> m_currentUser;
};
}

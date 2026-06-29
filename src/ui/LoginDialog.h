#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <memory>
#include "../services/AuthService.h"

namespace RetailMS {

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(std::shared_ptr<AuthService> authService, QWidget* parent = nullptr);

private slots:
    void onLoginClicked();

private:
    void setupUi();

    std::shared_ptr<AuthService> m_authService;
    QLineEdit* m_usernameInput;
    QLineEdit* m_passwordInput;
    QLabel* m_errorLabel;
    QPushButton* m_loginButton;
};

} // namespace RetailMS

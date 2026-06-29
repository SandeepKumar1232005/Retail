#include "LoginDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include "../services/SessionManager.h"

namespace RetailMS {

LoginDialog::LoginDialog(std::shared_ptr<AuthService> authService, QWidget* parent)
    : QDialog(parent), m_authService(std::move(authService)) {
    setupUi();
}

void LoginDialog::setupUi() {
    this->setWindowTitle("RetailMS - Enterprise Login");
    this->setFixedSize(400, 350);
    this->setStyleSheet(
        "QDialog { background-color: #1A1D24; }"
        "QLabel { color: #FFFFFF; font-size: 14px; }"
        "QLineEdit { background-color: #2A2D35; color: white; border: 1px solid #3A3D45; border-radius: 6px; padding: 10px; font-size: 14px; }"
        "QPushButton { background-color: #4F46E5; color: white; border: none; border-radius: 6px; padding: 12px; font-size: 16px; font-weight: bold; }"
        "QPushButton:hover { background-color: #4338CA; }"
    );

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    QLabel* title = new QLabel("Welcome Back", this);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #FFFFFF;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    m_errorLabel = new QLabel("", this);
    m_errorLabel->setStyleSheet("color: #EF4444; font-size: 12px;");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->hide();
    layout->addWidget(m_errorLabel);

    m_usernameInput = new QLineEdit(this);
    m_usernameInput->setPlaceholderText("Username");
    layout->addWidget(m_usernameInput);

    m_passwordInput = new QLineEdit(this);
    m_passwordInput->setPlaceholderText("Password");
    m_passwordInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(m_passwordInput);

    layout->addSpacing(10);

    m_loginButton = new QPushButton("Log In", this);
    m_loginButton->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_loginButton);

    connect(m_loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(m_passwordInput, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    connect(m_usernameInput, &QLineEdit::returnPressed, [this]() { m_passwordInput->setFocus(); });
}

void LoginDialog::onLoginClicked() {
    QString username = m_usernameInput->text().trimmed();
    QString password = m_passwordInput->text();

    if (username.isEmpty() || password.isEmpty()) {
        m_errorLabel->setText("Please enter both username and password.");
        m_errorLabel->show();
        return;
    }

    m_loginButton->setEnabled(false);
    m_loginButton->setText("Logging in...");

    auto userOpt = m_authService->login(username, password);
    if (userOpt) {
        SessionManager::instance().setCurrentUser(*userOpt);
        this->accept();
    } else {
        m_errorLabel->setText("Invalid username or password.");
        m_errorLabel->show();
        m_loginButton->setEnabled(true);
        m_loginButton->setText("Log In");
    }
}

} // namespace RetailMS

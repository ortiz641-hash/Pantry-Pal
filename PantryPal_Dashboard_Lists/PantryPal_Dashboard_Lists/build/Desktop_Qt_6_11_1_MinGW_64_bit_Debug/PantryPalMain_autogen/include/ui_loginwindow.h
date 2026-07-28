/********************************************************************************
** Form generated from reading UI file 'loginwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWINDOW_H
#define UI_LOGINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginWindow
{
public:
    QGroupBox *SignInBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *UsernameBox;
    QLabel *UsernameLable;
    QLineEdit *usernameField;
    QHBoxLayout *PasswordBox;
    QLabel *PasswordLable;
    QLineEdit *passwordField;
    QPushButton *loginButton;
    QLabel *errorLabel;

    void setupUi(QWidget *LoginWindow)
    {
        if (LoginWindow->objectName().isEmpty())
            LoginWindow->setObjectName("LoginWindow");
        LoginWindow->setEnabled(true);
        LoginWindow->resize(400, 300);
        LoginWindow->setAutoFillBackground(false);
        LoginWindow->setStyleSheet(QString::fromUtf8("background-color: white; "));
        SignInBox = new QGroupBox(LoginWindow);
        SignInBox->setObjectName("SignInBox");
        SignInBox->setGeometry(QRect(50, 60, 301, 161));
        QFont font;
        SignInBox->setFont(font);
        SignInBox->setAutoFillBackground(false);
        SignInBox->setStyleSheet(QString::fromUtf8("color: black;"));
        verticalLayout = new QVBoxLayout(SignInBox);
        verticalLayout->setObjectName("verticalLayout");
        UsernameBox = new QHBoxLayout();
        UsernameBox->setObjectName("UsernameBox");
        UsernameLable = new QLabel(SignInBox);
        UsernameLable->setObjectName("UsernameLable");
        UsernameLable->setFont(font);
        UsernameLable->setStyleSheet(QString::fromUtf8("color: black;"));

        UsernameBox->addWidget(UsernameLable);

        usernameField = new QLineEdit(SignInBox);
        usernameField->setObjectName("usernameField");
        usernameField->setAutoFillBackground(false);
        usernameField->setStyleSheet(QString::fromUtf8(" color: black"));

        UsernameBox->addWidget(usernameField);


        verticalLayout->addLayout(UsernameBox);

        PasswordBox = new QHBoxLayout();
        PasswordBox->setObjectName("PasswordBox");
        PasswordLable = new QLabel(SignInBox);
        PasswordLable->setObjectName("PasswordLable");
        PasswordLable->setFont(font);
        PasswordLable->setStyleSheet(QString::fromUtf8("color: black;"));

        PasswordBox->addWidget(PasswordLable);

        passwordField = new QLineEdit(SignInBox);
        passwordField->setObjectName("passwordField");
        passwordField->setStyleSheet(QString::fromUtf8("color: black"));
        passwordField->setEchoMode(QLineEdit::EchoMode::Password);

        PasswordBox->addWidget(passwordField);


        verticalLayout->addLayout(PasswordBox);

        loginButton = new QPushButton(SignInBox);
        loginButton->setObjectName("loginButton");
        loginButton->setEnabled(true);
        loginButton->setAutoFillBackground(false);
        loginButton->setStyleSheet(QString::fromUtf8(" background-color: green;"));

        verticalLayout->addWidget(loginButton);

        errorLabel = new QLabel(LoginWindow);
        errorLabel->setObjectName("errorLabel");
        errorLabel->setGeometry(QRect(60, 230, 291, 20));
        errorLabel->setFont(font);
        errorLabel->setStyleSheet(QString::fromUtf8("color: red;"));

        retranslateUi(LoginWindow);

        QMetaObject::connectSlotsByName(LoginWindow);
    } // setupUi

    void retranslateUi(QWidget *LoginWindow)
    {
        LoginWindow->setWindowTitle(QCoreApplication::translate("LoginWindow", "PantryPal Login", nullptr));
        SignInBox->setTitle(QCoreApplication::translate("LoginWindow", "WELCOME TO PANTRYPAL!!!", nullptr));
        UsernameLable->setText(QCoreApplication::translate("LoginWindow", "Username", nullptr));
        PasswordLable->setText(QCoreApplication::translate("LoginWindow", "Password", nullptr));
        loginButton->setText(QCoreApplication::translate("LoginWindow", "Login", nullptr));
        errorLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class LoginWindow: public Ui_LoginWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H

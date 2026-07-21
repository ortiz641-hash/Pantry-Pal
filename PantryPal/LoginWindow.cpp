#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::handleLoginClicked);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::handleLoginClicked()
{
    QString username = ui->usernameField->text();
    QString password = ui->passwordField->text();

   
    if (username.isEmpty() || password.isEmpty()) {
        ui->errorLabel->setText("Username and password are required.");
        return;
    }

   
    if (username == "123" && password == "123") {
        ui->errorLabel->setText("");
        emit loginSuccessful();
    }
    else {
        ui->errorLabel->setText("Invalid username or password.");
    }
}
#include "loginwindow.h"
#include "ui_loginwindow.h" // include the neccesary folder, Notice that window doesn't care about file name ca, but linux does. 


LoginWindow::LoginWindow(QWidget* parent) // to build the foundation of a new log in window 
    : QWidget(parent), ui(new Ui::LoginWindow)

{
    ui->setupUi(this); // Takes what we build in the UI and attaches to the login window
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::handleLoginClicked); // for signal and slot system
}

LoginWindow::~LoginWindow()

{
    delete ui; // When Login window object is destroyed, deelte the UI object that was created for memory clean uop
}

void LoginWindow::handleLoginClicked()

{
    QString username = ui->usernameField->text(); // to grab the user input from username and password field and stores it in those two variable to check if it match. 
    QString password = ui->passwordField->text();

   
    if (username.isEmpty() || password.isEmpty())
    {
        ui->errorLabel->setText("Username and password are required.");
        return;
    }

   
    if (username == "123" && password == "123") 
    {
        ui->errorLabel->setText("");
        emit loginSuccessful();
    }
    else {
        ui->errorLabel->setText("Invalid username or password.");
    }
}
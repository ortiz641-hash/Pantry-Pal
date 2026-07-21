#include "mainwindow.h"
#include "LoginWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    application.setApplicationName("PantryPal Main");

    LoginWindow* loginWindow = new LoginWindow();
    MainWindow* mainWindow = new MainWindow();

    
    QObject::connect(loginWindow, &LoginWindow::loginSuccessful, [loginWindow, mainWindow]() 
        {
            loginWindow->close();
            mainWindow->show();
        }
    );

    loginWindow->show();

    return application.exec();
}

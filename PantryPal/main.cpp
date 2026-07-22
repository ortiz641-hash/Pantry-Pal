#include "mainwindow.h"
#include "loginwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    application.setApplicationName("PantryPal Main");

    LoginWindow loginWindow;
    MainWindow mainWindow;
    
    QObject::connect(&loginWindow, &LoginWindow::loginSuccessful, [&]()
        
        {

        loginWindow.close();
        mainWindow.show();

        }
    );

    loginWindow.show();
    return application.exec();
}

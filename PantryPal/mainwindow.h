#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "DatabaseManager.h" //had to add databasemanager to create the buttons that will update the database and the gui when clicked.

class QPushButton;
class QWidget;
class QLabel;
class QVBoxLayout;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QPushButton *createNavigationButton(const QString &text);
    QPushButton *createStatButton(const QString &title);

    void refreshDashboard();

    DatabaseManager dbManager;

    
    QPushButton *totalItemsButton; // these qpushbuttons are for the dashboard stats, they will be updated when the pantry page is updated. 
    QPushButton *expiringSoonButton; 
    QPushButton *expiredButton;
    QPushButton *runningLowButton;
    QVBoxLayout *recentLayout;
};

#endif // MAINWINDOW_H
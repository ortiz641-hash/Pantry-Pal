#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QPushButton;
class QWidget;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QPushButton *createNavigationButton(const QString &text);
    QPushButton *createStatButton(const QString &title);
};

#endif // MAINWINDOW_H

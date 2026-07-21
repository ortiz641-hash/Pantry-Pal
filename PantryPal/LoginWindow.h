#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H // PREVENT the file from loading twice

#include <QWidget> // for visual in QT

namespace Ui
{
    class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT // This is needed to use signals and slots

public:
    explicit LoginWindow(QWidget* parent = nullptr);
    ~LoginWindow();

signals:
    void loginSuccessful();

private slots:
    void handleLoginClicked();

private:
    Ui::LoginWindow* ui;
};

#endif // LOGINWINDOW_H

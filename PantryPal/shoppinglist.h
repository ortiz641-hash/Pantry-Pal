#ifndef SHOPPINGLIST_H
#define SHOPPINGLIST_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>

class ShoppingList : public QWidget
{
    Q_OBJECT

public:
    explicit ShoppingList(QWidget* parent = nullptr);

private slots:
    void refreshShoppingList();
    void removePurchased();

private:
    QLabel* title;
    QListWidget* listWidget;
    QPushButton* refreshButton;
    QPushButton* removeButton;
};

#endif

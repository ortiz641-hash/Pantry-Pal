#pragma once
#ifndef SHOPPINGLIST_H
#define SHOPPINGLIST_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QDate>

struct Item
{
    QString name;
    int quantity;
    int minimumQuantity;
    QDate expirationDate;
};

namespace Ui {
    class ShoppingList;
}

class ShoppingList : public QWidget
{
    Q_OBJECT

public:
    explicit ShoppingList(QWidget* parent = nullptr);
    ~ShoppingList();

    void loadPantryItems(const QVector<Item>& items);

private:
    Ui::ShoppingList* ui;

    QVector<Item> pantryItems;

    void generateShoppingList();
};

#endif

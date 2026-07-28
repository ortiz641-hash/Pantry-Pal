
#ifndef SHOPPINGLIST_H
#define SHOPPINGLIST_H

#include "DatabaseManager.h"

#include <QWidget>


namespace Ui {
    class ShoppingList;
}

class ShoppingList : public QWidget
{
    Q_OBJECT

public:
    explicit ShoppingList(DatabaseManager *dbManager, QWidget* parent = nullptr);
    ~ShoppingList();

    void refreshShoppingList();

private:
    Ui::ShoppingList* ui;
    DatabaseManager *dbManager

    void generateShoppingList(const QVector<ShoppingListItem> &items);
};

#endif

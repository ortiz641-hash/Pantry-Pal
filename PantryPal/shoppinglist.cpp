#include "shoppinglist.h"
#include "ui_shoppinglist.h"

ShoppingList::ShoppingList(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ShoppingList)
{
    ui->setupUi(this);
}

ShoppingList::~ShoppingList()
{
    delete ui;
}

void ShoppingList::loadPantryItems(const QVector<Item>& items)
{
    pantryItems = items;
    generateShoppingList();
}

void ShoppingList::generateShoppingList()
{
    ui->listWidget->clear();

    QDate today = QDate::currentDate();

    for (const Item& item : pantryItems)
    {
        bool low = item.quantity <= item.minimumQuantity;
        bool expired = item.expirationDate < today;

        if (low || expired)
        {
            QListWidgetItem* shoppingItem =
                new QListWidgetItem(item.name);

            shoppingItem->setFlags(
                shoppingItem->flags() |
                Qt::ItemIsUserCheckable);

            shoppingItem->setCheckState(Qt::Unchecked);

            ui->listWidget->addItem(shoppingItem);
        }
    }
}

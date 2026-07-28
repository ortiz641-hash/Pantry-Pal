#include "shoppinglist.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>

ShoppingList::ShoppingList(QWidget* parent)
    : QWidget(parent)
{
    title = new QLabel("Shopping List");
    title->setAlignment(Qt::AlignCenter);

    QFont font;
    font.setPointSize(18);
    font.setBold(true);
    title->setFont(font);

    listWidget = new QListWidget();

    refreshButton = new QPushButton("Refresh");
    removeButton = new QPushButton("Remove Purchased");

    QHBoxLayout* buttons = new QHBoxLayout();
    buttons->addWidget(refreshButton);
    buttons->addWidget(removeButton);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(title);
    layout->addWidget(listWidget);
    layout->addLayout(buttons);

    connect(refreshButton,
        &QPushButton::clicked,
        this,
        &ShoppingList::refreshShoppingList);

    connect(removeButton,
        &QPushButton::clicked,
        this,
        &ShoppingList::removePurchased);

    refreshShoppingList();
}

void ShoppingList::refreshShoppingList()
{
    listWidget->clear();

    // Temporary demo data
    QStringList items =
    {
        "Milk",
        "Bread",
        "Eggs",
        "Chicken",
        "Cheese"
    };

    for (const QString& name : items)
    {
        QListWidgetItem* item = new QListWidgetItem(name);

        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);

        listWidget->addItem(item);
    }
}

void ShoppingList::removePurchased()
{
    for (int i = listWidget->count() - 1; i >= 0; --i)
    {
        if (listWidget->item(i)->checkState() == Qt::Checked)
        {
            delete listWidget->takeItem(i);
        }
    }
}

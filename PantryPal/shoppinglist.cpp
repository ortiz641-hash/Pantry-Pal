#include "shoppinglist.h"
#include "ui_shoppinglist.h"

ShoppingList::ShoppingList(DatabaseManager *dbManager, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ShoppingList)
    , dbManager(dbManager)
{
    ui->setupUi(this);

    //wiring widget items here and not generateShoppingList so it doesnt duplicate connection points
    connect(ui->listWidget, &QListWidget::itemChanged, this, [this](QListWidgetItem *item) {
    int id = item->data(Qt::UserRole).toInt();
    bool checked = item->checkState() == Qt::Checked;
    dbManager->setShoppingListItemChecked(id, checked);
});

    refreshShoppingList();
}

ShoppingList::~ShoppingList()
{
    delete ui;
}

void ShoppingList::refreshShoppingList(){
    dbManager->syncShoppingListFromPantry();
    const QVector<ShoppingListItem> items = dbManager -> getAllShoppingListItems();
    generateShoppingList(items);
}


void ShoppingList::generateShoppingList(const QVector<ShoppingListItem> &items)
{
    ui->listWidget->clear();

    for (const ShoppingListItem &item : items)
    {
        QListWidgetItem *listItem = new QListWidgetItem(item.name);
        listItem->setData(Qt::UserRole, item.id);
        listItem->setFlags(listItem->flags() | Qt::ItemIsUserCheckable);
        listItem->setCheckState(item.isChecked ? Qt::Checked : Qt::Unchecked);
        ui->listWidget->addItem(listItem);
        
    }
}

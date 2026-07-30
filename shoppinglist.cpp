#include "shoppinglist.h"
#include "DatabaseManager.h"

#include <QAbstractItemView>
#include <QDate>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QShowEvent>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QtGlobal>

namespace
{
QDate parseExpirationDate(const QString &value)
{
    const QString trimmedValue = value.trimmed();

    QDate parsedDate = QDate::fromString(trimmedValue, Qt::ISODate);
    if (parsedDate.isValid()) {
        return parsedDate;
    }

    const QStringList supportedFormats = {
        "MM/dd/yyyy",
        "M/d/yyyy",
        "MM-dd-yyyy"
    };

    for (const QString &format : supportedFormats) {
        parsedDate = QDate::fromString(trimmedValue, format);
        if (parsedDate.isValid()) {
            return parsedDate;
        }
    }

    return QDate();
}

double lowStockThreshold(const PantryItem &item)
{
    return qMax(1.0, item.minimumQuantity);
}
}

ShoppingList::ShoppingList(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent)
    , dbManager(dbManager)
{
    auto *title = new QLabel("Shopping List", this);
    addButton = new QPushButton("+  Add Item", this);
    addButton->setMinimumSize(120, 40);

    auto *topRow = new QHBoxLayout;
    topRow->addWidget(title);
    topRow->addStretch();
    topRow->addWidget(addButton);

    shoppingTable = new QTableWidget(this);
    shoppingTable->setColumnCount(3);
    shoppingTable->setHorizontalHeaderLabels(
        {"Item", "Quantity Needed", "Reason"});
    shoppingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    shoppingTable->setSelectionMode(QAbstractItemView::SingleSelection);
    shoppingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    shoppingTable->verticalHeader()->setVisible(false);
    shoppingTable->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::Stretch);
    shoppingTable->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::ResizeToContents);
    shoppingTable->horizontalHeader()->setSectionResizeMode(
        2, QHeaderView::Stretch);

    removeButton = new QPushButton("Remove Selected", this);

    auto *actionRow = new QHBoxLayout;
    actionRow->addWidget(removeButton);
    actionRow->addStretch();

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(25, 25, 25, 25);
    layout->setSpacing(12);
    layout->addLayout(topRow);
    layout->addWidget(shoppingTable, 1);
    layout->addLayout(actionRow);

    connect(addButton, &QPushButton::clicked,
            this, &ShoppingList::addManualItem);
    connect(removeButton, &QPushButton::clicked,
            this, &ShoppingList::removeSelectedItem);

    refreshList();
}

void ShoppingList::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    refreshList();
}

void ShoppingList::refreshList()
{
    const QVector<ShoppingListItem> manualItems =
        dbManager->getShoppingListItems();
    const QVector<PantryItem> pantryItems =
        dbManager->getAllItems();
    const QDate today = QDate::currentDate();

    shoppingTable->setRowCount(0);

    for (const PantryItem &pantryItem : pantryItems) {
        const QDate expirationDate =
            parseExpirationDate(pantryItem.expirationDate);
        const bool expired =
            expirationDate.isValid() && expirationDate <= today;
        const double threshold = lowStockThreshold(pantryItem);
        const bool low = pantryItem.quantity <= threshold;

        if (!low && !expired) {
            continue;
        }

        ShoppingListItem item;
        item.id = pantryItem.id;
        item.name = pantryItem.name;
        item.quantity = expired
            ? qMax(1.0, threshold)
            : qMax(1.0, threshold - pantryItem.quantity);
        item.source = "automatic";
        item.pantryItemId = pantryItem.id;
        item.reason = expired ? "Expired" : "Running low";

        const int row = shoppingTable->rowCount();
        shoppingTable->insertRow(row);

        auto *nameItem = new QTableWidgetItem(item.name);
        nameItem->setData(Qt::UserRole, item.id);
        nameItem->setData(Qt::UserRole + 1, item.source);

        shoppingTable->setItem(row, 0, nameItem);
        shoppingTable->setItem(
            row,
            1,
            new QTableWidgetItem(
                QString::number(item.quantity, 'g', 12)));
        shoppingTable->setItem(
            row,
            2,
            new QTableWidgetItem(item.reason));
    }

    for (const ShoppingListItem &item : manualItems) {
        const int row = shoppingTable->rowCount();
        shoppingTable->insertRow(row);

        auto *nameItem = new QTableWidgetItem(item.name);
        nameItem->setData(Qt::UserRole, item.id);
        nameItem->setData(Qt::UserRole + 1, item.source);

        auto *quantityItem =
            new QTableWidgetItem(QString::number(item.quantity, 'g', 12));
        auto *reasonItem = new QTableWidgetItem(item.reason);

        shoppingTable->setItem(row, 0, nameItem);
        shoppingTable->setItem(row, 1, quantityItem);
        shoppingTable->setItem(row, 2, reasonItem);
    }
}

void ShoppingList::addManualItem()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Add Item");
    dialog.setMinimumWidth(320);

    auto *nameField = new QLineEdit(&dialog);

    auto *quantityField = new QDoubleSpinBox(&dialog);
    quantityField->setRange(0.01, 100000.0);
    quantityField->setDecimals(2);
    quantityField->setValue(1.0);

    auto *reasonField = new QLineEdit(&dialog);

    auto *form = new QFormLayout;
    form->addRow("Item name:", nameField);
    form->addRow("Quantity needed:", quantityField);
    form->addRow("Reason:", reasonField);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        &dialog);

    auto *layout = new QVBoxLayout(&dialog);
    layout->addLayout(form);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::rejected,
            &dialog, &QDialog::reject);
    connect(buttons->button(QDialogButtonBox::Ok),
            &QPushButton::clicked,
            &dialog,
            [this, &dialog, nameField, quantityField, reasonField]() {
                const QString name = nameField->text().trimmed();
                const QString reason = reasonField->text().trimmed();

                if (name.isEmpty() || reason.isEmpty()) {
                    QMessageBox::information(
                        &dialog,
                        "Required fields",
                        "Enter an item name and reason.");
                    return;
                }

                ShoppingListItem item;
                item.name = name;
                item.quantity = quantityField->value();
                item.reason = reason;

                if (!dbManager->addShoppingListItem(item)) {
                    QMessageBox::information(
                        &dialog,
                        "Item not added",
                        "That item is already on the shopping list or "
                        "could not be saved.");
                    return;
                }

                dialog.accept();
            });

    if (dialog.exec() == QDialog::Accepted) {
        refreshList();
    }
}

void ShoppingList::removeSelectedItem()
{
    const int row = shoppingTable->currentRow();
    if (row < 0) {
        QMessageBox::information(
            this, "No selection",
            "Select an item to remove from the shopping list.");
        return;
    }

    QTableWidgetItem *nameItem = shoppingTable->item(row, 0);
    const int id = nameItem->data(Qt::UserRole).toInt();
    const QString source =
        nameItem->data(Qt::UserRole + 1).toString();

    if (source == "automatic") {
        QMessageBox::information(
            this, "Automatic item",
            "Update or remove this item from the Pantry tab first.");
        return;
    }

    if (!dbManager->removeShoppingListItem(id)) {
        QMessageBox::warning(
            this, "Could not remove item",
            "The selected item could not be removed.");
        return;
    }

    refreshList();
}

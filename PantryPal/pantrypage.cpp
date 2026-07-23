#include "PantryPage.h"
#include "ItemDialog.h"

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAbstractItemView>

PantryPage::PantryPage(DatabaseManager *dbManager, QWidget *parent) //edited to add functionality to the buttons of add edit and delete
    : QWidget(parent), dbManager(dbManager)
{
    addItemButton = new QPushButton("Add Item", this); //similar to what we had before but using more q widgets to work with SQL database easier.
    searchField = new QLineEdit(this);
    searchField->setPlaceholderText("Search...");
    locationFilter = new QComboBox(this);
    locationFilter->addItem("All");
    inventoryTable = new QTableWidget(this);
    editButton = new QPushButton("Edit", this);
    deleteButton = new QPushButton("Delete", this);

    auto *topBar = new QHBoxLayout;
    topBar->addWidget(searchField);
    topBar->addWidget(locationFilter);
    topBar->addWidget(addItemButton);
    topBar->addWidget(editButton);
    topBar->addWidget(deleteButton);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(topBar);
    layout->addWidget(inventoryTable);

    configureTable();
    refreshTable();

    connect(addItemButton, &QPushButton::clicked, this, &PantryPage::openAddItemDialog); //connects the add item button to the open add item dialog function so i had to change the old connect string here
    connect(editButton, &QPushButton::clicked, this, &PantryPage::editSelectedItem);
    connect(deleteButton, &QPushButton::clicked, this, &PantryPage::deleteSelectedItem);
    connect(searchField, &QLineEdit::textChanged, this, &PantryPage::applyFilters);
    connect(locationFilter, &QComboBox::currentTextChanged, this, &PantryPage::applyFilters);
}

void PantryPage::configureTable() //had to replace old table config for sql so its dynamic now.
{
    inventoryTable->setColumnCount(6);
    inventoryTable->setHorizontalHeaderLabels(
        {"Name", "Quantity", "Unit", "Category", "Expiration", "Notes"});
    inventoryTable->horizontalHeader()->setStretchLastSection(true);
    inventoryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    inventoryTable->setSelectionMode(QAbstractItemView::SingleSelection);
    inventoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void PantryPage::refreshTable() //replaced the old hard coded stuff with the refresh table fucntion.
{
    inventoryTable->setRowCount(0);
    const QVector<PantryItem> items = dbManager->getAllItems();
    for (const PantryItem &item : items) {
        addTableRow(item);
    }
}

void PantryPage::addTableRow(const PantryItem &item)
{
    int row = inventoryTable->rowCount();
    inventoryTable->insertRow(row);

    auto *nameItem = new QTableWidgetItem(item.name);
    nameItem->setData(Qt::UserRole, item.id);

    inventoryTable->setItem(row, 0, nameItem);
    inventoryTable->setItem(row, 1, new QTableWidgetItem(QString::number(item.quantity)));
    inventoryTable->setItem(row, 2, new QTableWidgetItem(item.unit));
    inventoryTable->setItem(row, 3, new QTableWidgetItem(item.category));
    inventoryTable->setItem(row, 4, new QTableWidgetItem(item.expirationDate));
    inventoryTable->setItem(row, 5, new QTableWidgetItem(item.notes));
}

int PantryPage::selectedItemId() const
{
    int row = inventoryTable->currentRow();
    if (row < 0) return -1;
    return inventoryTable->item(row, 0)->data(Qt::UserRole).toInt();
}

void PantryPage::openAddItemDialog() //pulls from new item dialog files i added to the project, this is called when the add item button is clicked and it opens a dialog to add a new item to the pantry.
{
    ItemDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        bool ok = dbManager->addItem(dialog.item());
        qDebug() << "Add item returned:" << ok;
        refreshTable();
        emit inventoryChanged();
    }
}

void PantryPage::editSelectedItem() //added the proper edit function over the placeholder we had for the demo same deal as the add item
{
    int id = selectedItemId();
    if (id < 0) {
        QMessageBox::information(this, "No selection", "Select an item to edit.");
        return;
    }

    PantryItem existing = dbManager->getItemById(id);
    ItemDialog dialog(this, existing);
    if (dialog.exec() == QDialog::Accepted) {
        PantryItem updated = dialog.item();
        updated.id = id;
        dbManager->updateItem(updated);
        refreshTable();
        emit inventoryChanged();
    }
}

void PantryPage::deleteSelectedItem() //you get the idea
{
    int id = selectedItemId();
    if (id < 0) {
        QMessageBox::information(this, "No selection", "Select an item to delete.");
        return;
    }

    auto confirm = QMessageBox::question(this, "Delete item",
        "Are you sure you want to delete this item?");
    if (confirm == QMessageBox::Yes) {
        dbManager->removeItem(id);
        refreshTable();
        emit inventoryChanged();
    }
}

void PantryPage::applyFilters() //more or less the same as the old filtering function
{
    const QString search = searchField->text().trimmed();
    const QString location = locationFilter->currentText();

    for (int row = 0; row < inventoryTable->rowCount(); ++row) {
        bool matchesSearch = search.isEmpty()
            || inventoryTable->item(row, 0)->text().contains(search, Qt::CaseInsensitive);

        bool matchesLocation = location.isEmpty()
            || location == "All"
            || inventoryTable->item(row, 3)->text() == location;

        inventoryTable->setRowHidden(row, !(matchesSearch && matchesLocation));
    }
}
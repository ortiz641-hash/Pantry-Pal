#include "pantrypage.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

PantryPage::PantryPage(QWidget *parent)
    : QWidget(parent),
    searchField(new QLineEdit(this)),
    locationFilter(new QComboBox(this)),
    inventoryTable(new QTableWidget(this)),
    addItemButton(new QPushButton("+  Add Item", this)),
    editButton(new QPushButton("Edit Item", this)),
    deleteButton(new QPushButton("Delete Item", this))
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(16);

    auto *title = new QLabel("Pantry Inventory", this);
    title->setObjectName("pageTitle");

    auto *description = new QLabel(
        "View and organize food by storage location.",
        this);
    description->setObjectName("pageDescription");

    auto *headerLayout = new QHBoxLayout;
    auto *titleLayout = new QVBoxLayout;

    titleLayout->addWidget(title);
    titleLayout->addWidget(description);

    addItemButton->setMinimumSize(120, 40);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(addItemButton, 0, Qt::AlignTop);

    mainLayout->addLayout(headerLayout);

    // Search and filtering controls
    auto *filterLayout = new QHBoxLayout;

    searchField->setPlaceholderText("Search by item name...");
    searchField->setClearButtonEnabled(true);

    locationFilter->addItem("All Locations");
    locationFilter->addItem("Pantry");
    locationFilter->addItem("Refrigerator");
    locationFilter->addItem("Freezer");
    locationFilter->addItem("Other");

    filterLayout->addWidget(searchField, 1);
    filterLayout->addWidget(locationFilter);

    mainLayout->addLayout(filterLayout);

    configureTable();
    mainLayout->addWidget(inventoryTable, 1);

    // Edit/Delete controls
    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);

    mainLayout->addLayout(buttonLayout);

    connect(
        searchField,
        &QLineEdit::textChanged,
        this,
        [this]() {
            applyFilters();
        });

    connect(
        locationFilter,
        &QComboBox::currentTextChanged,
        this,
        [this]() {
            applyFilters();
        });

    connect(
        addItemButton,
        &QPushButton::clicked,
        this,
        &PantryPage::openAddItemDialog);

    connect(
        editButton,
        &QPushButton::clicked,
        this,
        [this]() {
            editSelectedItem();
        });

    connect(
        deleteButton,
        &QPushButton::clicked,
        this,
        [this]() {
            deleteSelectedItem();
        });

    addSampleItems();
}

void PantryPage::configureTable()
{
    inventoryTable->setColumnCount(7);

    inventoryTable->setHorizontalHeaderLabels({
        "Item",
        "Quantity",
        "Unit",
        "Category",
        "Location",
        "Expiration Date",
        "Minimum Quantity"
    });

    inventoryTable->setSelectionBehavior(
        QAbstractItemView::SelectRows);

    inventoryTable->setSelectionMode(
        QAbstractItemView::SingleSelection);

    inventoryTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers);

    inventoryTable->setAlternatingRowColors(true);
    inventoryTable->setSortingEnabled(true);

    inventoryTable->verticalHeader()->setVisible(false);

    inventoryTable->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
}

void PantryPage::addSampleItems()
{
    addTableRow(
        "Milk",
        1,
        "gallon",
        "Dairy",
        "Refrigerator",
        "07/24/2026",
        1);

    addTableRow(
        "Chicken",
        2,
        "pounds",
        "Meat",
        "Freezer",
        "07/27/2026",
        1);

    addTableRow(
        "Rice",
        3,
        "bags",
        "Grains",
        "Pantry",
        "12/10/2026",
        1);

    addTableRow(
        "Vitamins",
        1,
        "bottle",
        "Health",
        "Other",
        "01/15/2027",
        1);
}

void PantryPage::addTableRow(
    const QString &name,
    double quantity,
    const QString &unit,
    const QString &category,
    const QString &location,
    const QString &expirationDate,
    double minimumQuantity)
{
    // Temporarily turn sorting off so a row cannot move
    // while its individual cells are being inserted.
    inventoryTable->setSortingEnabled(false);

    const int row = inventoryTable->rowCount();
    inventoryTable->insertRow(row);

    inventoryTable->setItem(
        row,
        0,
        new QTableWidgetItem(name));

    inventoryTable->setItem(
        row,
        1,
        new QTableWidgetItem(QString::number(quantity)));

    inventoryTable->setItem(
        row,
        2,
        new QTableWidgetItem(unit));

    inventoryTable->setItem(
        row,
        3,
        new QTableWidgetItem(category));

    inventoryTable->setItem(
        row,
        4,
        new QTableWidgetItem(location));

    inventoryTable->setItem(
        row,
        5,
        new QTableWidgetItem(expirationDate));

    inventoryTable->setItem(
        row,
        6,
        new QTableWidgetItem(
            QString::number(minimumQuantity)));

    inventoryTable->setSortingEnabled(true);
}

void PantryPage::applyFilters()
{
    const QString searchText =
        searchField->text().trimmed();

    const QString selectedLocation =
        locationFilter->currentText();

    for (int row = 0;
         row < inventoryTable->rowCount();
         ++row) {

        QTableWidgetItem *nameItem =
            inventoryTable->item(row, 0);

        QTableWidgetItem *locationItem =
            inventoryTable->item(row, 4);

        if (!nameItem || !locationItem) {
            continue;
        }

        const bool matchesSearch =
            nameItem->text().contains(
                searchText,
                Qt::CaseInsensitive);

        const bool matchesLocation =
            selectedLocation == "All Locations"
            || locationItem->text() == selectedLocation;

        inventoryTable->setRowHidden(
            row,
            !(matchesSearch && matchesLocation));
    }
}

void PantryPage::openAddItemDialog()
{
    QMessageBox::information(
        this,
        "Add Item",
        "Under development. The Add Item dialog will be connected here.");
}

void PantryPage::editSelectedItem()
{
    const int row = inventoryTable->currentRow();

    if (row < 0) {
        QMessageBox::information(
            this,
            "Edit Item",
            "Select an item to edit.");

        return;
    }

    // Temporary behavior for the first demo.
    // A proper edit dialog can be added in another card.
    QMessageBox::information(
        this,
        "Edit Item",
        "Edit functionality will be connected next.");
}

void PantryPage::deleteSelectedItem()
{
    const int row = inventoryTable->currentRow();

    if (row < 0) {
        QMessageBox::information(
            this,
            "Delete Item",
            "Select an item to delete.");

        return;
    }

    const QString itemName =
        inventoryTable->item(row, 0)->text();

    const QMessageBox::StandardButton result =
        QMessageBox::question(
            this,
            "Delete Item",
            "Remove " + itemName + " from the pantry?",
            QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        inventoryTable->removeRow(row);
    }
}
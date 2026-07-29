#include "ItemDialog.h"

#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDate>

ItemDialog::ItemDialog(QWidget *parent, const PantryItem &existing) //mostly GUI stuff for adding and removing dialog and storing added and edited items. also can do pre fills if the user is editing a item theyve already added to the pantrt
    : QDialog(parent)
{
    setWindowTitle(existing.id < 0 ? "Add Item" : "Edit Item");
    setMinimumWidth(320);

    nameEdit = new QLineEdit(this);
    categoryEdit = new QLineEdit(this);

    quantitySpin = new QDoubleSpinBox(this);
    quantitySpin->setRange(1.0, 100000.0);
    quantitySpin->setDecimals(2);
    minimumQuantitySpin = new QDoubleSpinBox(this); // new field for minimum quantity lets the user set a minimum quantity for the item, if the item goes below this quantity it will be highlighted in the main window.
    minimumQuantitySpin->setRange(0.0, 100000.0);
    minimumQuantitySpin->setDecimals(2);

    locationCombo = new QComboBox(this);
    locationCombo->setEditable(true); // Combo box that lets the user select the location of where their food item is going to go.
    locationCombo->addItems({"Pantry", "Fridge", "Freezer", "Cabinet"});


    unitEdit = new QLineEdit(this);
    unitEdit->setPlaceholderText("e.g. kg, oz, count"); //thought place holder text was cool here 

    expirationEdit = new QDateEdit(this);
    expirationEdit->setCalendarPopup(true); //got a cool lil calander popup from the qt library
    expirationEdit->setDisplayFormat("yyyy-MM-dd");
    expirationEdit->setDate(QDate::currentDate());

    notesEdit = new QLineEdit(this);

    auto *form = new QFormLayout; //q widget for a clean layot of the dialog box for adding and editing items
    form->addRow("Name:", nameEdit);
    form->addRow("Category:", categoryEdit);
    form->addRow("Quantity:", quantitySpin);
    form->addRow("Unit:", unitEdit);
    form->addRow("Expiration date:", expirationEdit);
    form->addRow("Notes:", notesEdit);
    form->addRow("Location:", locationCombo); // new field for location
    form->addRow("Minimum Quantity:", minimumQuantitySpin); // new field for minimum quantity

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);

    // Pre-fill fields when editing an existing item
    if (existing.id >= 0) {
        nameEdit->setText(existing.name);
        categoryEdit->setText(existing.category);
        quantitySpin->setValue(existing.quantity);
        unitEdit->setText(existing.unit);
        notesEdit->setText(existing.notes);
        locationCombo->setCurrentText(existing.location); // pre-fills the location combo box with the existing location
        minimumQuantitySpin->setValue(existing.minimumQuantity); // pre-fills the minimum quantity spin box with the existing minimum quantity

        QDate parsedDate = QDate::fromString(existing.expirationDate, "yyyy-MM-dd");
        if (parsedDate.isValid()) {
            expirationEdit->setDate(parsedDate);
        }
    }
}

PantryItem ItemDialog::item() const
{
    PantryItem item;
    item.name = nameEdit->text().trimmed();
    item.category = categoryEdit->text().trimmed();
    item.location = locationCombo->currentText().trimmed(); // gets the location from the combo box
    item.quantity = quantitySpin->value();
    item.minimumQuantity = minimumQuantitySpin->value(); // gets the minimum quantity from the spin box
    item.unit = unitEdit->text().trimmed();
    item.expirationDate = expirationEdit->date().toString("yyyy-MM-dd");
    item.notes = notesEdit->text().trimmed();
    return item;
}
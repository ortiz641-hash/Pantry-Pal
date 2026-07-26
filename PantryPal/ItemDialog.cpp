#include "ItemDialog.h"

#include <QLineEdit>
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
    quantitySpin->setRange(0.0, 100000.0);
    quantitySpin->setDecimals(2);

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
    item.quantity = quantitySpin->value();
    item.unit = unitEdit->text().trimmed();
    item.expirationDate = expirationEdit->date().toString("yyyy-MM-dd");
    item.notes = notesEdit->text().trimmed();
    return item;
}
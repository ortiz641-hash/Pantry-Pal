
#ifndef ITEMDIALOG_H
#define ITEMDIALOG_H

#include <QDialog>
#include "DatabaseManager.h"

class QLineEdit;
class QDoubleSpinBox;
class QComboBox;
class QDateEdit;
class QPushButton;
class ItemDialog : public QDialog
{
public:
    explicit ItemDialog(QWidget *parent = nullptr, const PantryItem &existing = PantryItem());
    PantryItem item() const;

private:
    QLineEdit *nameEdit;
    QLineEdit *categoryEdit;
    QDoubleSpinBox *quantitySpin;
    QComboBox *locationCombo; // new field for location
    QDoubleSpinBox *minimumQuantitySpin; // new field for minimum quantity
    QLineEdit *unitEdit;
    QDateEdit *expirationEdit;
    QLineEdit *notesEdit;
};

#endif // ITEMDIALOG_H
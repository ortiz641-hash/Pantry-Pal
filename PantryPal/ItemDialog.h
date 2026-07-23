// ItemDialog.h
#ifndef ITEMDIALOG_H
#define ITEMDIALOG_H

#include <QDialog>
#include "DatabaseManager.h"

class QLineEdit;
class QDoubleSpinBox;
class QDateEdit;

class ItemDialog : public QDialog
{
public:
    explicit ItemDialog(QWidget *parent = nullptr, const PantryItem &existing = PantryItem());
    PantryItem item() const;

private:
    QLineEdit *nameEdit;
    QLineEdit *categoryEdit;
    QDoubleSpinBox *quantitySpin;
    QLineEdit *unitEdit;
    QDateEdit *expirationEdit;
    QLineEdit *notesEdit;
};

#endif // ITEMDIALOG_H
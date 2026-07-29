#ifndef PANTRYPAGE_H
#define PANTRYPAGE_H

#include <QWidget>
#include "DatabaseManager.h"

class QComboBox;
class QLineEdit;
class QPushButton;
class QTableWidget;
class PantryPage : public QWidget
{
    Q_OBJECT // q object macro that lets the class use signals and slots, which is a way to communicate between objects in Qt.

public:
    explicit PantryPage(DatabaseManager *dbManager, QWidget *parent = nullptr); //had to change to take a pointer to the database manager so it can access the database functions.

    void openAddItemDialog(); // opens the dialog to add a new item to the pantry, this is called when the add item button is clicked.

signals:
    void inventoryChanged(); //signal for when the invo changes

private:
    void configureTable();
    void refreshTable(); //refresh function for the table to update the gui when the database is updated, this is called after an item is manipulated at all.
    void addTableRow(const PantryItem &item); // added a table row func to the pantry page that sql can use 

    void applyFilters();
    void editSelectedItem();
    void deleteSelectedItem();

    int selectedItemId() const; // returns the id of the selected item in the table, this is used to edit or delete the selected item.

    DatabaseManager *dbManager; //dbmanager pointer

    QPushButton *addItemButton;
    QLineEdit *searchField;
    QComboBox *locationFilter;
    QTableWidget *inventoryTable;
    QPushButton *editButton;
    QPushButton *deleteButton;
};

#endif // PANTRYPAGE_H
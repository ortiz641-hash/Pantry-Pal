#ifndef PANTRYPAGE_H
#define PANTRYPAGE_H

#include <QWidget>

class QComboBox;
class QLineEdit;
class QPushButton;
class QTableWidget;
class QPushButton;

class PantryPage : public QWidget
{
public:
    explicit PantryPage(QWidget *parent = nullptr);

private:
    void configureTable();
    void addSampleItems();
    void addTableRow(
        const QString &name,
        double quantity,
        const QString &unit,
        const QString &category,
        const QString &location,
        const QString &expirationDate,
        double minimumQuantity);

    void applyFilters();
    void openAddItemDialog();
    void editSelectedItem();
    void deleteSelectedItem();

    QPushButton *addItemButton;
    QLineEdit *searchField;
    QComboBox *locationFilter;
    QTableWidget *inventoryTable;
    QPushButton *editButton;
    QPushButton *deleteButton;
};

#endif // PANTRYPAGE_H
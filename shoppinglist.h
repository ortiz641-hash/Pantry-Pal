#ifndef SHOPPINGLIST_H
#define SHOPPINGLIST_H

#include <QWidget>

class DatabaseManager;
class QPushButton;
class QShowEvent;
class QTableWidget;

class ShoppingList : public QWidget
{
    Q_OBJECT

public:
    explicit ShoppingList(DatabaseManager *dbManager,
                          QWidget *parent = nullptr);

    void refreshList();

protected:
    void showEvent(QShowEvent *event) override;

private:
    void addManualItem();
    void removeSelectedItem();

    DatabaseManager *dbManager;
    QPushButton *addButton;
    QTableWidget *shoppingTable;
    QPushButton *removeButton;
};

#endif // SHOPPINGLIST_H

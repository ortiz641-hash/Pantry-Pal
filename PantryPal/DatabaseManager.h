#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H


#include <QSqlDatabase>
#include <QString>
#include <QVector>


struct PantryItem
{ //struct that represents a single pantry item, these will all be editable in the gui, and will be stored in the database.
    int id = -1;
    QString name;
    QString category;
    double quantity = 0.0;
    double minimumQuantity = 0.0; // new field for minimum quantity
    QString location; // new field for location
    QString unit;
    QString expirationDate; // stored as 'YYYY-MM-DD'
    QString notes;
};

struct ShoppingListItem //for meekeh
{
    int id = -1;
    QString name; //name of shopping list item
    double quantityNeeded = 0.0; //quantity needed for shopping
    QString unit; 
    bool isChecked = false;
    QString source = "manual";
    int pantryItemId = -1;   // -1 means not linked to a pantry item

};

class DatabaseManager
{
public:
    DatabaseManager();

    bool connect(); //connects the database if it doesn't exist, it will create it and the table for the pantry items.
    void createTables(); //creates the table for the pantry items
    void ShoppingListTable(); //creates table for the shopping list

    // CRUD
    bool addItem(const PantryItem &item); // adds a new item to the database
    bool removeItem(int id); // removes an item from the database by its id
    bool updateItem(const PantryItem &item); // updates an existing item in the database by its id with the edit button in the gui. should update the gui if the edit makes it expired or running low.
    QVector<PantryItem> getAllItems(); // a vector to retrieve all the items 
    PantryItem getItemById(int id); // will get a specific pantry item by its ID
    

    //functions for the shoppinglist actions for meekeh
    bool addShoppingListItem(const ShoppingListItem &item); //add something to shopping list
    bool removeShoppingListItem(int id); //remove from shopping list function
    bool setShoppingListItemChecked(int id, bool checked); // 
    QVector<ShoppingListItem> getAllShoppingListItems(); //just a q vector for the items added to the shopping list 
    void syncShoppingListFromPantry();  // should auto-adds low/expired pantry items


private:
    QSqlDatabase db; // just the database object, used to connect to the database and perform queries.
};

#endif
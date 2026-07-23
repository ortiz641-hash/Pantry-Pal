#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

DatabaseManager::DatabaseManager()
{
}

bool DatabaseManager::connect()
{
    // Store the db in a proper app data location instead of next to the exe
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation); // This will be something like C:/Users/<User>/AppData/Roaming/PantryPal on Windows
    QDir dir(dataPath); // Create the directory if it doesn't exist
    if (!dir.exists()) { 
        dir.mkpath(".");
    }
    QString dbPath = dir.filePath("pantrypal.db"); // names the filepath

    db = QSqlDatabase::addDatabase("QSQLITE"); //connecting the database to sqlite drivers 
    db.setDatabaseName(dbPath); // 

    if (!db.open()) {
        qWarning() << "Failed to open database:" << db.lastError().text();
        return false;
    }


    return true;
}

void DatabaseManager::createTables() //table creation function uses sql statement to make a table for the pantru items. 
{
    QSqlQuery query(db);

    bool ok = query.exec(R"(
        CREATE TABLE IF NOT EXISTS pantry_items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            category TEXT,
            quantity REAL NOT NULL DEFAULT 0,
            unit TEXT,
            expiration_date TEXT,
            date_added TEXT DEFAULT CURRENT_TIMESTAMP,
            notes TEXT
        )
    )");

    if (!ok) {
        qWarning() << "Failed to create pantry_items table:" << query.lastError().text(); //qWarning is a macro that prints a warning message to the console mad useful for debuging.
    }

    query.exec("CREATE INDEX IF NOT EXISTS idx_pantry_name ON pantry_items(name)");
}

bool DatabaseManager::addItem(const PantryItem &item)
{
    QSqlQuery query(db); // query object is used to execute SQL statements and retrieve results from the databaseIt is initialized with the database connection db.
    query.prepare(R"(
        INSERT INTO pantry_items (name, category, quantity, unit, expiration_date, notes) 
        VALUES (:name, :category, :quantity, :unit, :expiration_date, :notes)
    )");
    query.bindValue(":name", item.name); //the next 6 lines are for locking the pantry item values to the statement above. keeps the format correct.
    query.bindValue(":category", item.category);
    query.bindValue(":quantity", item.quantity);
    query.bindValue(":unit", item.unit);
    query.bindValue(":expiration_date", item.expirationDate);
    query.bindValue(":notes", item.notes);

    if (!query.exec()) {
        qWarning() << "addItem failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::removeItem(int id)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM pantry_items WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "removeItem failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateItem(const PantryItem &item)
{
    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE pantry_items
        SET name = :name,
            category = :category,
            quantity = :quantity,
            unit = :unit,
            expiration_date = :expiration,
            notes = :notes
        WHERE id = :id
    )");
    query.bindValue(":name", item.name);
    query.bindValue(":category", item.category);
    query.bindValue(":quantity", item.quantity);
    query.bindValue(":unit", item.unit);
    query.bindValue(":expiration", item.expirationDate);
    query.bindValue(":notes", item.notes);
    query.bindValue(":id", item.id);

    if (!query.exec()) {
        qWarning() << "updateItem failed:" << query.lastError().text();
        return false;
    }
    return true;
}

QVector<PantryItem> DatabaseManager::getAllItems() //function returns a vector of all the pantry items. simple loop nothin crazy
{
    QVector<PantryItem> items;
    QSqlQuery query(db);
    query.exec(R"(
        SELECT id, name, category, quantity, unit, expiration_date, notes
        FROM pantry_items
        ORDER BY name
    )");

    while (query.next()) {
        PantryItem item;
        item.id = query.value(0).toInt();
        item.name           = query.value(1).toString();
        item.category       = query.value(2).toString();
        item.quantity       = query.value(3).toDouble();
        item.unit           = query.value(4).toString();
        item.expirationDate = query.value(5).toString();
        item.notes          = query.value(6).toString();
        items.append(item);
    }
    return items;
}

PantryItem DatabaseManager::getItemById(int id) //prob most important function, uses the ID to grab specific pantry items
{
    PantryItem item;
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT id, name, category, quantity, unit, expiration_date, notes
        FROM pantry_items WHERE id = :id
    )");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        item.id             = query.value(0).toInt();
        item.name           = query.value(1).toString();
        item.category       = query.value(2).toString();
        item.quantity       = query.value(3).toDouble();
        item.unit           = query.value(4).toString();
        item.expirationDate = query.value(5).toString();
        item.notes          = query.value(6).toString();
    }
    return item;
}
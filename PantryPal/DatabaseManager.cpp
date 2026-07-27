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

void DatabaseManager::createTables()
{
    QSqlQuery query(db);

    query.exec(R"(
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

    // ADDED a sql pragma, was running into an issue where db file still thought there was only 6 columns on the table after i updated it, this should prevent that from happening.
    query.exec("PRAGMA user_version");
    int version = 0;
    if (query.next()) {
        version = query.value(0).toInt();
    }

    // Migration 1: add location + minimum_quantity columns
    if (version < 1) {
        query.exec("ALTER TABLE pantry_items ADD COLUMN location TEXT");
        query.exec("ALTER TABLE pantry_items ADD COLUMN minimum_quantity REAL NOT NULL DEFAULT 0");
        query.exec("PRAGMA user_version = 1");
    }

    query.exec("CREATE INDEX IF NOT EXISTS idx_pantry_name ON pantry_items(name)");
}

bool DatabaseManager::addItem(const PantryItem &item)
{
    QSqlQuery query(db);
    bool prepared = query.prepare(R"(
        INSERT INTO pantry_items (name, category, location, quantity, minimum_quantity, unit, expiration_date, notes)
        VALUES (:name, :category, :location, :quantity, :minimumQuantity, :unit, :expiration_date, :notes)
    )");

     if (!prepared) {
        qWarning() << "prepare failed:" << query.lastError().text();
        return false;
    }

    
    query.bindValue(":name", item.name);
    query.bindValue(":category", item.category);
    query.bindValue(":location", item.location);
    query.bindValue(":quantity", item.quantity);
    query.bindValue(":minimumQuantity", item.minimumQuantity);
    query.bindValue(":unit", item.unit);
    query.bindValue(":expiration_date", item.expirationDate);
    query.bindValue(":notes", item.notes);

    if (!query.exec()) {
        qDebug() << "Bound value count:" << query.boundValueNames();
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

bool DatabaseManager::updateItem(const PantryItem &item)//updated for location and min quant here too
{
    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE pantry_items
        SET name = :name,
            category = :category,
            location = :location, 
            quantity = :quantity,
            minimum_quantity = :minimum_quantity, 
            unit = :unit,
            expiration_date = :expiration,
            notes = :notes
        WHERE id = :id
    )");
    query.bindValue(":name", item.name);
    query.bindValue(":category", item.category);
    query.bindValue(":quantity", item.quantity);
    query.bindValue(":minimum_quantity", item.minimumQuantity); //binds the minimum quantity value to the statement above
    query.bindValue(":location", item.location); //binds the location value to the statement    
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
    QVector<PantryItem> items; //updated for min quant and location functionality
    QSqlQuery query(db);
    query.exec(R"(
        SELECT id, name, category, location, quantity, minimum_quantity, unit, expiration_date, notes
        FROM pantry_items
        ORDER BY name
    )");

    while (query.next()) {
        PantryItem item;
        item.id             = query.value(0).toInt();
        item.name           = query.value(1).toString();
        item.category       = query.value(2).toString();
        item.location       = query.value(3).toString(); // Assuming location is stored in the 4th column
        item.quantity       = query.value(4).toDouble();
        item.minimumQuantity= query.value(5).toDouble(); // Assuming minimum quantity is stored in the 6th column
        item.unit           = query.value(6).toString();
        item.expirationDate = query.value(7).toString();
        item.notes          = query.value(8).toString();
        items.append(item);
    }
    return items;
}

PantryItem DatabaseManager::getItemById(int id) //prob most important function, uses the ID to grab specific pantry items
{
    PantryItem item; //updated for min quant and loc
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT id, name, category, location, quantity, minimum_quantity, unit, expiration_date, notes 
        FROM pantry_items WHERE id = :id
    )");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        item.id             = query.value(0).toInt();
        item.name           = query.value(1).toString();
        item.category       = query.value(2).toString();
        item.location       = query.value(3).toString(); // Assuming location is stored in the 4th column
        item.quantity       = query.value(4).toDouble(); 
        item.minimumQuantity= query.value(5).toDouble(); // Assuming minimum quantity is stored in the 6th column
        item.unit           = query.value(6).toString();
        item.expirationDate = query.value(7).toString();
        item.notes          = query.value(8).toString();
    }                                   
    return item;
}
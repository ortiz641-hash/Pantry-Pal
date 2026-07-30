#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QDate>
#include <QStringList>
#include <QtGlobal>

namespace
{
QDate parseExpirationDate(const QString &value)
{
    const QString trimmedValue = value.trimmed();

    QDate parsedDate = QDate::fromString(trimmedValue, Qt::ISODate);
    if (parsedDate.isValid()) {
        return parsedDate;
    }

    const QStringList supportedFormats = {
        "MM/dd/yyyy",
        "M/d/yyyy",
        "MM-dd-yyyy"
    };

    for (const QString &format : supportedFormats) {
        parsedDate = QDate::fromString(trimmedValue, format);
        if (parsedDate.isValid()) {
            return parsedDate;
        }
    }

    return QDate();
}

double lowStockThreshold(const PantryItem &item)
{
    return qMax(1.0, item.minimumQuantity);
}
}

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

    QSqlQuery foreignKeys(db);
    foreignKeys.exec("PRAGMA foreign_keys = ON");

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

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS shopping_list_items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            quantity REAL NOT NULL DEFAULT 1,
            unit TEXT,
            checked INTEGER NOT NULL DEFAULT 0,
            source TEXT NOT NULL DEFAULT 'manual',
            pantry_item_id INTEGER,
            reason TEXT,
            date_added TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (pantry_item_id) REFERENCES pantry_items(id) ON DELETE CASCADE
        )
    )");

    query.exec(R"(
        DELETE FROM shopping_list_items
        WHERE source = 'automatic'
          AND pantry_item_id IS NOT NULL
          AND id NOT IN (
              SELECT MIN(id)
              FROM shopping_list_items
              WHERE source = 'automatic'
                AND pantry_item_id IS NOT NULL
              GROUP BY pantry_item_id
          )
    )");

    query.exec(R"(
        CREATE UNIQUE INDEX IF NOT EXISTS idx_shopping_automatic_pantry_item
        ON shopping_list_items(pantry_item_id)
        WHERE source = 'automatic' AND pantry_item_id IS NOT NULL
    )");

    query.exec("CREATE INDEX IF NOT EXISTS idx_shopping_name ON shopping_list_items(name)");
    query.exec("DELETE FROM shopping_list_items WHERE source = 'automatic'");
    query.exec("PRAGMA user_version = 2");
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

bool DatabaseManager::addShoppingListItem(const ShoppingListItem &item)
{
    const QString trimmedName = item.name.trimmed();
    const QString trimmedReason = item.reason.trimmed();
    if (trimmedName.isEmpty()
        || item.quantity <= 0.0
        || trimmedReason.isEmpty()) {
        return false;
    }

    QSqlQuery duplicateQuery(db);
    duplicateQuery.prepare(R"(
        SELECT 1
        FROM shopping_list_items
        WHERE source = 'manual'
          AND LOWER(TRIM(name)) = LOWER(TRIM(:name))
        LIMIT 1
    )");
    duplicateQuery.bindValue(":name", trimmedName);

    if (!duplicateQuery.exec()) {
        qWarning() << "Shopping list duplicate check failed:"
                   << duplicateQuery.lastError().text();
        return false;
    }

    if (duplicateQuery.next()) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO shopping_list_items
            (name, quantity, unit, checked, source, pantry_item_id, reason)
        VALUES
            (:name, :quantity, '', 0, 'manual', NULL, :reason)
    )");
    query.bindValue(":name", trimmedName);
    query.bindValue(":quantity", item.quantity);
    query.bindValue(":reason", trimmedReason);

    if (!query.exec()) {
        qWarning() << "addShoppingListItem failed:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::removeShoppingListItem(int id)
{
    QSqlQuery query(db);
    query.prepare(R"(
        DELETE FROM shopping_list_items
        WHERE id = :id AND source = 'manual'
    )");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "removeShoppingListItem failed:"
                   << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool DatabaseManager::setShoppingListItemChecked(int id, bool checked)
{
    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE shopping_list_items
        SET checked = :checked
        WHERE id = :id
    )");
    query.bindValue(":checked", checked ? 1 : 0);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "setShoppingListItemChecked failed:"
                   << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool DatabaseManager::clearCheckedShoppingListItems()
{
    if (!db.transaction()) {
        return false;
    }

    QSqlQuery deleteManual(db);
    if (!deleteManual.exec(R"(
        DELETE FROM shopping_list_items
        WHERE checked = 1 AND source = 'manual'
    )")) {
        qWarning() << "Could not clear checked manual shopping items:"
                   << deleteManual.lastError().text();
        db.rollback();
        return false;
    }

    // Automatic entries remain until the pantry item is no longer low or
    // expired, so clearing them resets their checkmark instead of deleting.
    QSqlQuery resetAutomatic(db);
    if (!resetAutomatic.exec(R"(
        UPDATE shopping_list_items
        SET checked = 0
        WHERE checked = 1 AND source = 'automatic'
    )")) {
        qWarning() << "Could not reset automatic shopping items:"
                   << resetAutomatic.lastError().text();
        db.rollback();
        return false;
    }

    return db.commit();
}

bool DatabaseManager::syncShoppingListWithPantry()
{
    const QVector<PantryItem> pantryItems = getAllItems();
    const QDate today = QDate::currentDate();

    if (!db.transaction()) {
        return false;
    }

    for (const PantryItem &item : pantryItems) {
        const QDate expirationDate =
            parseExpirationDate(item.expirationDate);
        const bool expired =
            expirationDate.isValid() && expirationDate <= today;
        const double threshold = lowStockThreshold(item);
        const bool low = item.quantity <= threshold;

        if (!low && !expired) {
            QSqlQuery removeResolvedItem(db);
            removeResolvedItem.prepare(R"(
                DELETE FROM shopping_list_items
                WHERE source = 'automatic' AND pantry_item_id = :pantryId
            )");
            removeResolvedItem.bindValue(":pantryId", item.id);

            if (!removeResolvedItem.exec()) {
                qWarning() << "Could not remove resolved automatic item:"
                           << removeResolvedItem.lastError().text();
                db.rollback();
                return false;
            }
            continue;
        }

        const QString reason = expired ? "Expired" : "Running low";

        double suggestedQuantity = 1.0;
        if (expired) {
            suggestedQuantity = qMax(1.0, threshold);
        } else {
            suggestedQuantity =
                qMax(1.0, threshold - item.quantity);
        }

        QSqlQuery existingQuery(db);
        existingQuery.prepare(R"(
            SELECT id
            FROM shopping_list_items
            WHERE source = 'automatic' AND pantry_item_id = :pantryId
        )");
        existingQuery.bindValue(":pantryId", item.id);

        if (!existingQuery.exec()) {
            qWarning() << "Could not find automatic shopping item:"
                       << existingQuery.lastError().text();
            db.rollback();
            return false;
        }

        if (existingQuery.next()) {
            QSqlQuery updateQuery(db);
            updateQuery.prepare(R"(
                UPDATE shopping_list_items
                SET name = :name,
                    quantity = :quantity,
                    unit = :unit,
                    reason = :reason
                WHERE id = :id
            )");
            updateQuery.bindValue(":name", item.name);
            updateQuery.bindValue(":quantity", suggestedQuantity);
            updateQuery.bindValue(":unit", item.unit);
            updateQuery.bindValue(":reason", reason);
            updateQuery.bindValue(":id", existingQuery.value(0).toInt());

            if (!updateQuery.exec()) {
                qWarning() << "Could not update automatic shopping item:"
                           << updateQuery.lastError().text();
                db.rollback();
                return false;
            }
        } else {
            QSqlQuery manualDuplicateQuery(db);
            manualDuplicateQuery.prepare(R"(
                SELECT 1
                FROM shopping_list_items
                WHERE source = 'manual'
                  AND LOWER(TRIM(name)) = LOWER(TRIM(:name))
                LIMIT 1
            )");
            manualDuplicateQuery.bindValue(":name", item.name);

            if (!manualDuplicateQuery.exec()) {
                qWarning() << "Could not check for a manual shopping item:"
                           << manualDuplicateQuery.lastError().text();
                db.rollback();
                return false;
            }

            if (manualDuplicateQuery.next()) {
                continue;
            }

            QSqlQuery insertQuery(db);
            insertQuery.prepare(R"(
                INSERT INTO shopping_list_items
                    (name, quantity, unit, checked, source, pantry_item_id, reason)
                VALUES
                    (:name, :quantity, :unit, 0, 'automatic', :pantryId, :reason)
            )");
            insertQuery.bindValue(":name", item.name);
            insertQuery.bindValue(":quantity", suggestedQuantity);
            insertQuery.bindValue(":unit", item.unit);
            insertQuery.bindValue(":pantryId", item.id);
            insertQuery.bindValue(":reason", reason);

            if (!insertQuery.exec()) {
                qWarning() << "Could not add automatic shopping item:"
                           << insertQuery.lastError().text();
                db.rollback();
                return false;
            }
        }
    }

    QSqlQuery removeDeletedItems(db);
    if (!removeDeletedItems.exec(R"(
        DELETE FROM shopping_list_items
        WHERE source = 'automatic'
          AND pantry_item_id NOT IN (SELECT id FROM pantry_items)
    )")) {
        qWarning() << "Could not remove deleted pantry items from shopping list:"
                   << removeDeletedItems.lastError().text();
        db.rollback();
        return false;
    }

    return db.commit();
}

QVector<ShoppingListItem> DatabaseManager::getShoppingListItems()
{
    QVector<ShoppingListItem> items;
    QSqlQuery query(db);

    if (!query.exec(R"(
        SELECT id, name, quantity, unit, checked, source, pantry_item_id, reason
        FROM shopping_list_items
        WHERE source = 'manual'
        ORDER BY name COLLATE NOCASE
    )")) {
        qWarning() << "getShoppingListItems failed:"
                   << query.lastError().text();
        return items;
    }

    while (query.next()) {
        ShoppingListItem item;
        item.id = query.value(0).toInt();
        item.name = query.value(1).toString();
        item.quantity = query.value(2).toDouble();
        item.unit = query.value(3).toString();
        item.checked = query.value(4).toInt() != 0;
        item.source = query.value(5).toString();
        item.pantryItemId = query.value(6).isNull()
            ? -1
            : query.value(6).toInt();
        item.reason = query.value(7).toString();
        items.append(item);
    }

    return items;
}

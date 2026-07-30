#include "mainwindow.h"
#include "PantryPage.h"
#include "profilepage.h"
#include "shoppinglist.h"
#include <QStackedWidget>



#include <QAbstractItemView>
#include <QDialog>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QTableWidget>
#include <QVBoxLayout>
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("PantryPal Main");
    resize(1280, 760);
    setMinimumSize(960, 600);

    dbManager.connect(); // connect to database in mainwindow
    dbManager.createTables(); //creates the table for the stats in the main window

    auto *mainView = new QWidget(this);
    setCentralWidget(mainView);

    auto *windowLayout = new QHBoxLayout(mainView);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->setSpacing(0);

    auto *sidebar = new QFrame;
    sidebar->setFrameShape(QFrame::StyledPanel);
    sidebar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(15, 20, 15, 20);
    sidebarLayout->setSpacing(8);

    auto *brandName = new QLabel("PantryPal");

    sidebarLayout->addWidget(brandName);
    sidebarLayout->addSpacing(20);

    auto *dashboardButton = createNavigationButton("Dashboard");
    auto *pantryButton = createNavigationButton("Pantry");
    auto *shoppingListButton = createNavigationButton("Shopping List");

    sidebarLayout->addWidget(dashboardButton);
    sidebarLayout->addWidget(pantryButton);
    sidebarLayout->addWidget(shoppingListButton);
    
    auto* profileButton = createNavigationButton("Profile"); 
    sidebarLayout->addWidget(profileButton);
   
    sidebarLayout->addStretch();

    auto *dashboard = new QFrame;
    dashboard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *dashboardLayout = new QVBoxLayout(dashboard);
    dashboardLayout->setContentsMargins(25, 25, 25, 25);
    dashboardLayout->setSpacing(20);

    auto *topRow = new QHBoxLayout;
    auto *addItemButton = new QPushButton("+  Add Item");
    addItemButton->setMinimumSize(120, 40);

    topRow->addStretch();
    topRow->addWidget(addItemButton, 0, Qt::AlignTop);
    dashboardLayout->addLayout(topRow);

    auto *statsLayout = new QHBoxLayout;
    statsLayout->setSpacing(12);

    totalItemsButton = createStatButton("Total Items"); //these 4 buttons hold the stats for the dashboard.
    expiringSoonButton = createStatButton("Expiring Soon");
    expiredButton = createStatButton("Expired");
    runningLowButton = createStatButton("Running Low");

    statsLayout->addWidget(totalItemsButton);
    statsLayout->addWidget(expiringSoonButton);
    statsLayout->addWidget(expiredButton);
    statsLayout->addWidget(runningLowButton);
    dashboardLayout->addLayout(statsLayout);

    connect(totalItemsButton, &QPushButton::clicked,
            this, [this]() {
                showItemListWindow(ItemListFilter::All);
            });

    connect(expiringSoonButton, &QPushButton::clicked,
            this, [this]() {
                showItemListWindow(ItemListFilter::ExpiringSoon);
            });

    connect(expiredButton, &QPushButton::clicked,
            this, [this]() {
                showItemListWindow(ItemListFilter::Expired);
            });

    connect(runningLowButton, &QPushButton::clicked,
            this, [this]() {
                showItemListWindow(ItemListFilter::RunningLow);
            });

    auto *recentItemsPanel = new QFrame;
    recentItemsPanel->setFrameShape(QFrame::StyledPanel);

    recentLayout = new QVBoxLayout(recentItemsPanel); // holds a list of recent items using a vertical layout. the list is updated when the pantry page is updated.
    recentLayout->setContentsMargins(18, 16, 18, 16);
    recentLayout->setSpacing(0);

    auto *recentTitle = new QLabel("Recent Items");
    recentLayout->addWidget(recentTitle);
    recentLayout->addStretch();

    dashboardLayout->addWidget(recentItemsPanel, 1);

    auto *pageStack = new QStackedWidget;
    auto *pantryPage = new PantryPage(&dbManager); // passing the dbManager pointer to the PantryPage constructor so it can access the database functions.
    auto *shoppingListPage = new ShoppingList(&dbManager);

    //--- page stack ---- 

    pageStack->addWidget(dashboard);          // index 0
    pageStack->addWidget(pantryPage);         // index 1, used jons pagestack to switch between the dashboard and pantry page when the buttons are clicked.
    pageStack->addWidget(shoppingListPage);   // index 2
    pageStack->addWidget(new ProfilePage);    // index 3
      

    connect(dashboardButton, &QPushButton::clicked,
            pageStack, [this, pageStack]() { // connect the dashboard button to switch to the dashboard page
                pageStack->setCurrentIndex(0);
                refreshDashboard();   // stats can go stale while on the pantry page so we needed a refresh when returning to the dashboard
            });

    connect(pantryButton, &QPushButton::clicked,
            pageStack, [pageStack]() {
                pageStack->setCurrentIndex(1);
            });

    connect(shoppingListButton, &QPushButton::clicked,
            pageStack, [pageStack, shoppingListPage]() {
                shoppingListPage->refreshList();
                pageStack->setCurrentIndex(2);
            });
   
    connect(profileButton, &QPushButton::clicked,
            pageStack, [pageStack]() {
                pageStack->setCurrentIndex(3);
            });

    //---- layout ----- 

    // gave the dashboard its own add item button and it jumps to the pantry page and refreshes whjen you go back to dashboard 
    connect(addItemButton, &QPushButton::clicked,
            pageStack, [pageStack, pantryPage]() {
                pageStack->setCurrentIndex(1);
                pantryPage->openAddItemDialog();
            });

    // When PantryPage changes data dashboard stats should update too
    connect(pantryPage, &PantryPage::inventoryChanged,
            this, &MainWindow::refreshDashboard);
    connect(pantryPage, &PantryPage::inventoryChanged,
            shoppingListPage, &ShoppingList::refreshList);

    windowLayout->addWidget(sidebar);
    windowLayout->addWidget(pageStack);
    windowLayout->setStretch(0, 1);
    windowLayout->setStretch(1, 4);

    refreshDashboard(); // call the refresh funct for when the app opens so the stats dont start stale
}

QPushButton *MainWindow::createNavigationButton(const QString &text)
{
    auto *button = new QPushButton(text);
    button->setMinimumHeight(40);
    return button;
}

QPushButton *MainWindow::createStatButton(const QString &title)
{
    auto *button = new QPushButton(title);
    button->setMinimumHeight(110);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return button;
}

void MainWindow::refreshDashboard() // refresh function.
{
    QVector<PantryItem> items = dbManager.getAllItems();

    totalItemsButton->setText(QString("Total Items\n%1").arg(items.size()));

    int expiringSoon = 0, expired = 0, runningLow = 0;
    QDate today = QDate::currentDate();

    for (const PantryItem &item : items) { //loops throught the items and checks their stats to see if they need to be updated if they do the dashboard should show these updates
        QDate exp = parseExpirationDate(item.expirationDate);
        if (exp.isValid()) {
            if (exp <= today) {
                expired++;
            } else if (today.daysTo(exp) <= 7) {
                expiringSoon++;
            }
        }
        if (item.quantity <= lowStockThreshold(item)) {
            runningLow++;
        }
    }

    expiringSoonButton->setText(QString("Expiring Soon\n%1").arg(expiringSoon)); // updates the main window stats when the pantry page gets updates, could maybe call refreshdashboard here??
    expiredButton->setText(QString("Expired\n%1").arg(expired));
    runningLowButton->setText(QString("Running Low\n%1").arg(runningLow));

    // Clear old recent-item labels (keep the title + stretch, remove everything between)
    while (recentLayout->count() > 2) {
        QLayoutItem *child = recentLayout->takeAt(1);
        delete child->widget();
        delete child;
    }

    int shown = 0;
    for (auto it = items.crbegin(); it != items.crend() && shown < 5; ++it, ++shown) {
        auto *label = new QLabel(QString("%1  —  %2 %3").arg(it->name).arg(it->quantity).arg(it->unit));
        recentLayout->insertWidget(recentLayout->count() - 1, label);
    }
}

void MainWindow::showItemListWindow(ItemListFilter filter)
{
    const QVector<PantryItem> pantryItems = dbManager.getAllItems();
    QVector<PantryItem> filteredItems;
    const QDate today = QDate::currentDate();
    QString title;

    switch (filter) {
    case ItemListFilter::All:
        title = "Total Items";
        break;
    case ItemListFilter::ExpiringSoon:
        title = "Expiring Soon";
        break;
    case ItemListFilter::Expired:
        title = "Expired";
        break;
    case ItemListFilter::RunningLow:
        title = "Running Low";
        break;
    }

    for (const PantryItem &item : pantryItems) {
        const QDate expirationDate =
            parseExpirationDate(item.expirationDate);

        bool matches = false;
        switch (filter) {
        case ItemListFilter::All:
            matches = true;
            break;
        case ItemListFilter::ExpiringSoon:
            matches = expirationDate.isValid()
                && expirationDate > today
                && today.daysTo(expirationDate) <= 7;
            break;
        case ItemListFilter::Expired:
            matches = expirationDate.isValid()
                && expirationDate <= today;
            break;
        case ItemListFilter::RunningLow:
            matches = item.quantity <= lowStockThreshold(item);
            break;
        }

        if (matches) {
            filteredItems.append(item);
        }
    }

    auto *listWindow = new QDialog(this);
    listWindow->setAttribute(Qt::WA_DeleteOnClose);
    listWindow->setWindowTitle(title);
    listWindow->resize(950, 500);

    auto *layout = new QVBoxLayout(listWindow);
    auto *heading = new QLabel(
        QString("%1 (%2)").arg(title).arg(filteredItems.size()),
        listWindow);
    layout->addWidget(heading);

    auto *table = new QTableWidget(listWindow);
    table->setColumnCount(8);
    table->setHorizontalHeaderLabels({
        "Name",
        "Quantity",
        "Minimum Quantity",
        "Unit",
        "Category",
        "Location",
        "Expiration",
        "Notes"
    });
    table->setRowCount(filteredItems.size());
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    for (int row = 0; row < filteredItems.size(); ++row) {
        const PantryItem &item = filteredItems.at(row);
        table->setItem(row, 0, new QTableWidgetItem(item.name));
        table->setItem(row, 1,
                       new QTableWidgetItem(QString::number(item.quantity)));
        table->setItem(row, 2,
                       new QTableWidgetItem(
                           QString::number(item.minimumQuantity)));
        table->setItem(row, 3, new QTableWidgetItem(item.unit));
        table->setItem(row, 4, new QTableWidgetItem(item.category));
        table->setItem(row, 5, new QTableWidgetItem(item.location));
        table->setItem(row, 6,
                       new QTableWidgetItem(item.expirationDate));
        table->setItem(row, 7, new QTableWidgetItem(item.notes));
    }

    table->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    table->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(table);

    listWindow->show();
}

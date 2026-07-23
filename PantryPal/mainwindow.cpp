#include "mainwindow.h"
#include "pantrypage.h"
#include "profilepage.h"
#include <QStackedWidget>



#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("PantryPal Main");
    resize(1280, 760);
    setMinimumSize(960, 600);

    auto *mainView = new QWidget(this);
    setCentralWidget(mainView);

    auto *windowLayout = new QHBoxLayout(mainView);
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->setSpacing(0);

    // The sidebar is embedded into the main window, so it cannot be closed
    // separately. Stretch factors keep it at 1/5 of the available width.
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

    sidebarLayout->addWidget(dashboardButton);
    sidebarLayout->addWidget(pantryButton);
    sidebarLayout->addWidget(createNavigationButton("Shopping List"));
    
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

    auto *totalItemsButton = createStatButton("Total Items");
    auto *expiringSoonButton = createStatButton("Expiring Soon");
    auto *expiredButton = createStatButton("Expired");
    auto *runningLowButton = createStatButton("Running Low");

    statsLayout->addWidget(totalItemsButton);
    statsLayout->addWidget(expiringSoonButton);
    statsLayout->addWidget(expiredButton);
    statsLayout->addWidget(runningLowButton);
    dashboardLayout->addLayout(statsLayout);

    auto *recentItemsPanel = new QFrame;
    recentItemsPanel->setFrameShape(QFrame::StyledPanel);

    auto *recentLayout = new QVBoxLayout(recentItemsPanel);
    recentLayout->setContentsMargins(18, 16, 18, 16);
    recentLayout->setSpacing(0);

    auto *recentTitle = new QLabel("Recent Items");
    recentLayout->addWidget(recentTitle);
    recentLayout->addStretch();

    dashboardLayout->addWidget(recentItemsPanel, 1);

    auto *pageStack = new QStackedWidget;

    //--- page stack ---- 

    pageStack->addWidget(dashboard);          // index 0
    pageStack->addWidget(new PantryPage);     // index 1
    pageStack->addWidget(new ProfilePage);    // index 2

    //--- connection -----

    connect(dashboardButton, &QPushButton::clicked,
            pageStack, [pageStack]() {
                pageStack->setCurrentIndex(0);
            });

    connect(pantryButton, &QPushButton::clicked,
            pageStack, [pageStack]() {
                pageStack->setCurrentIndex(1);
            });
   
    connect(profileButton, &QPushButton::clicked,
            pageStack, [pageStack]() {
                pageStack->setCurrentIndex(2);
            });

    //---- layout ----- 

    windowLayout->addWidget(sidebar);
    windowLayout->addWidget(pageStack);
    windowLayout->setStretch(0, 1);
    windowLayout->setStretch(1, 4);
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

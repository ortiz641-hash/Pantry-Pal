#include "mainwindow.h"

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

    sidebarLayout->addWidget(createNavigationButton("Dashboard"));
    sidebarLayout->addWidget(createNavigationButton("Pantry"));
    sidebarLayout->addWidget(createNavigationButton("Shopping List"));
    sidebarLayout->addWidget(createNavigationButton("Profile"));
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

    windowLayout->addWidget(sidebar);
    windowLayout->addWidget(dashboard);
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

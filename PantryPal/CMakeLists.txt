cmake_minimum_required(VERSION 3.16)

project(PantryPalMain VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

add_executable(PantryPalMain
    main.cpp
    mainwindow.cpp
    mainwindow.h
    pantrypage.h
    pantrypage.cpp
    loginwindow.h
    loginwindow.cpp
    loginwindow.ui
    profilepage.cpp 
    profilepage.h
    DatabaseManager.h
    DatabaseManager.cpp
    ItemDialog.h
    ItemDialog.cpp
    shoppinglist.cpp
    shoppinglist.h
)
    
target_link_libraries(PantryPalMain PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

if(WIN32)
    set_target_properties(PantryPalMain PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
 
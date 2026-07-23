cmake_minimum_required(VERSION 3.16)

project(PantryPalMain VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

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
)
target_link_libraries(PantryPalMain PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(WIN32)
    set_target_properties(PantryPalMain PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

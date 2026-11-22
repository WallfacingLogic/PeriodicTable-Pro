cmake_minimum_required(VERSION 3.16)
project(PeriodicTable VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp
    mainwindow.h
    elementdata.cpp
    elementdata.h
    periodictablewidget.cpp
    periodictablewidget.h
    elementdetailpanel.cpp
    elementdetailpanel.h
    glassstyle.cpp
    glassstyle.h
    titlebar.cpp
    titlebar.h
    resources.qrc
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
    CXX_STANDARD 17
)

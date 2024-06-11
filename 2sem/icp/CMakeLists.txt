# Author: Tomas Brablec
# Project: pacppman (ICP project)
# License: GPL 3.0 (https://www.gnu.org/licenses/gpl-3.0.txt)
#
cmake_minimum_required(VERSION 3.0)

project(pacppman)

set(CMAKE_CXX_STANDARD 17)

# debug symbols
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(pacppman
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/game_map.cpp
    src/game_map.h
    src/utils.cpp
    src/utils.h
    src/game.cpp
    src/game.h
    assets/assets.qrc
)

target_link_libraries(pacppman Qt5::Widgets)

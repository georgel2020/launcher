cmake_minimum_required(VERSION 4.0)
project(launcher)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt.
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# Add source directory.
add_subdirectory("src")
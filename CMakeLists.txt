cmake_minimum_required(VERSION 4.0)
project(launcher)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(ENABLE_WIDE_CHAR ON)

# Find Qt.
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# Add source directory.
add_subdirectory("src")

# Add muparser source directory.
add_subdirectory("third-party/muparser")
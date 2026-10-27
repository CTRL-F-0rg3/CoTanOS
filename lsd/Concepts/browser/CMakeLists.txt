cmake_minimum_required(VERSION 3.16)
project(trangorge_browser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(WEBKIT REQUIRED IMPORTED_TARGET webkitgtk-6.0)

add_executable(trangorge_browser src/main.cpp)
target_link_libraries(trangorge_browser PRIVATE
    PkgConfig::GTK4
    PkgConfig::WEBKIT
)